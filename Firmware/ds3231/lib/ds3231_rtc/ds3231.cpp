/*
 * =====================================================================================
 *
 *       Filename:  ds3231.cpp
 *
 *    Description:  -
 *
 *        Version:  1.0
 *        Created:  16/04/2025 12:58:16
 *       Revision:  none
 *       Compiler:  -
 *
 *         Author:  Isaac Vinicius, isaacvinicius2121@alu.ufc.br
 *   Organization:  UFC-Quixadá
 *
 * =====================================================================================
*/
#include "ds3231.hpp"

/* Declarando estrutura global para armazenar dados do módulo DS3231 */
DS3231 rtc_ds3231;

/**
 * @brief Lê um único registrador do DS3231 via I2C
*/
bool ds3231_read_reg(i2c_inst_t *i2c, uint8_t reg_addr, uint8_t *dest) {
    /* Enviando o endereço do registrador a ser lido (sem STOP) */
    if (i2c_write_blocking(i2c, DS3231_I2C_ADDR, &reg_addr, 1, true) != 1)
        return false;
    /* Lê o dado do registrador (com STOP) */
    if (i2c_read_blocking(i2c, DS3231_I2C_ADDR, dest, 1, false) != 1)
        return false;

    return true;
}

/**
 * @brief Escreve em um único registrador do DS3231 via I2C
*/

bool ds3231_write_reg(i2c_inst_t *i2c, uint8_t reg_addr, uint8_t value) {
    uint8_t buffer[2] = {reg_addr, value};
    return (i2c_write_blocking(i2c, DS3231_I2C_ADDR, buffer, 2, false) == 2);
}

/**
 * @brief Inicializa a comunicação com o DS3231 e os pinos I2C
*/
void inicializa_ds3231(DS3231 *mod_rtc, i2c_inst_t *i2c, uint8_t endereco, uint sda_pin, uint scl_pin){
    /* Armazenando endereço e instância de I2C na estrutura */
    mod_rtc->endereco = endereco;
    mod_rtc->i2c = i2c;
  
    /* Inicializando comunicação I2C com frequência de 400 kHz */
    i2c_init(i2c, 400 * 1000);
  
    /* Configurando pinos SDA e SCL para função I2C */
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
  
    /* Habilitando resistores de pull-up nos pinos SDA e SCL */
    // gpio_pull_up(sda_pin);
    // gpio_pull_up(scl_pin);
}


/* ============================================================================
 *  Conversões entre decimal e BCD
 * ============================================================================ 
*/

 uint8_t decimal_to_bcd(uint8_t val) {
    return ((val / 10) << 4) | (val % 10);
}

uint8_t bcd_to_decimal(uint8_t val) {
    return ((val >> 4) * 10) + (val & 0x0F);
}


/* ============================================================================
 *  Leitura de horário atual
 * ============================================================================ 
*/

/**
 * @brief Lê a hora atual do RTC (segundos, minutos e horas).
 * 
 * @param i2c  Instância da I2C conectada ao RTC
 * @param hora Ponteiro para estrutura HoraRTC a ser preenchida
 * @return true se a leitura foi bem-sucedida
*/
bool hora_atual_rtc(i2c_inst_t *i2c, HoraRTC *hora) {
    uint8_t buffer[3];
    if (i2c_write_blocking(i2c, DS3231_I2C_ADDR, (uint8_t[]){DS3231_REG_SECONDS}, 1, true) != 1)
        return false;
    if (i2c_read_blocking(i2c, DS3231_I2C_ADDR, buffer, 3, false) != 3)
        return false;

    hora->segundos = bcd_to_decimal(buffer[0]);
    hora->minutos = bcd_to_decimal(buffer[1]);
    hora->horas = bcd_to_decimal(buffer[2] & 0x3F);
    return true;
}



/* ============================================================================
 *  Configuração de Alarmes
 * ============================================================================ 
*/
/**
 * @brief Configura o alarme 1 do DS3231 para disparar em um horário específico (minuto e segundo).
 * 
 * A configuração considera:
 * - A1M1 = 0: Comparação com segundos
 * - A1M2 = 0: Comparação com minutos
 * - A1M3 = 1: Ignora horas
 * - A1M4 = 1: Ignora dia/data
 * 
 * Também ativa o modo de interrupção no pino INT/SQW.
 * 
 * @param i2c Instância da I2C conectada ao RTC
 * @param min Minuto exato para disparo
 * @param seg Segundo exato para disparo
 * @return true se o alarme foi configurado com sucesso
*/

bool alarme_para_horario(i2c_inst_t *i2c, uint8_t min, uint8_t seg) {
    uint8_t bcd_min = decimal_to_bcd(min);
    uint8_t bcd_seg = decimal_to_bcd(seg);

    /* Alarm1: Match segundos e minutos */
    if (!ds3231_write_reg(i2c, DS3231_REG_ALARM1_SEC, bcd_seg)) return false;  // A1M1 = 0
    if (!ds3231_write_reg(i2c, DS3231_REG_ALARM1_MIN, bcd_min)) return false;  // A1M2 = 0
    if (!ds3231_write_reg(i2c, DS3231_REG_ALARM1_HOUR, 0x80)) return false;    // A1M3 = 1 (ignora horas)
    if (!ds3231_write_reg(i2c, DS3231_REG_ALARM1_DAY_DATE, 0x80)) return false;// A1M4 = 1 (ignora dia)

    /* Habilitando alarme e INT/SQW como interrupção */
    uint8_t ctrl;
    if (!ds3231_read_reg(i2c, DS3231_REG_CONTROL, &ctrl)) return false;
    ctrl |= DS3231_CTRL_INTCN | DS3231_CTRL_A1IE;
    return ds3231_write_reg(i2c, DS3231_REG_CONTROL, ctrl);
}


  
/**
 * @brief Agenda um alarme para disparar após um intervalo relativo a partir da hora atual.
 * 
 * Calcula o horário futuro com base nos minutos e segundos de offset, e agenda o alarme
 * com 'alarme_para_horario()'.
 * 
 * @param i2c         Instância da I2C conectada ao RTC
 * @param offset_min  Minutos a partir de agora
 * @param offset_seg  Segundos a partir de agora
 * @return true se o alarme foi agendado corretamente
*/
bool agenda_alarme_em(i2c_inst_t *i2c, uint8_t offset_min, uint8_t offset_seg) {
    HoraRTC agora;
    if (!hora_atual_rtc(i2c, &agora)) return false;

    uint16_t total_segundos = agora.segundos + offset_seg;
    uint8_t novo_seg = total_segundos % 60;

    uint16_t total_minutos = agora.minutos + offset_min + (total_segundos / 60);
    uint8_t novo_min = total_minutos % 60;

    return alarme_para_horario(i2c, novo_min, novo_seg);
}