/*
 * =====================================================================================
 *
 *       Filename:  ds3231.hpp
 *
 *    Description:  -
 *
 *        Version:  1.0
 *        Created:  14/04/2025 16:38:25
 *       Revision:  none
 *       Compiler:  -
 *
 *         Author:  Isaac Vinicius, isaacvinicius2121@alu.ufc.br
 *   Organization:  UFC-Quixadá
 *
 * =====================================================================================
*/

#ifndef DS3231_HPP
#define DS3231_HPP
#include <Arduino.h>
#include "hardware/i2c.h"

/****************************************************************************
**                MACRO REGISTER ADDRESS CONFIG DS3231
*****************************************************************************/

/* Slave Addr DS3231 */
const uint8_t DS3231_I2C_ADDR = 0x68;

/* Timekeeping Registers (0x00–0x06) */
#define DS3231_REG_SECONDS            0x00  // Segundos (BCD)
#define DS3231_REG_MINUTES            0x01  // Minutos (BCD)
#define DS3231_REG_HOURS              0x02  // Horas (modo 12/24h com AM/PM)
#define DS3231_REG_DAY                0x03  // Dia da semana (1–7)
#define DS3231_REG_DATE               0x04  // Dia do mês (01–31)
#define DS3231_REG_MONTH_CENTURY      0x05  // Mês + Century (bit 7)
#define DS3231_REG_YEAR               0x06  // Ano (00–99)

/* Alarm 1 Registers (0x07–0x0A) */
#define DS3231_REG_ALARM1_SEC         0x07  // Segundos
#define DS3231_REG_ALARM1_MIN         0x08  // Minutos
#define DS3231_REG_ALARM1_HOUR        0x09  // Horas
#define DS3231_REG_ALARM1_DAY_DATE    0x0A  // Dia/Data + Máscara DY/DT

/* Alarm 2 Registers (0x0B–0x0D) */
#define DS3231_REG_ALARM2_MIN         0x0B
#define DS3231_REG_ALARM2_HOUR        0x0C
#define DS3231_REG_ALARM2_DAY_DATE    0x0D

/* Control and Status Registers (0x0E–0x0F) */
#define DS3231_REG_CONTROL            0x0E  // Controle de interrupções e modo de operação
#define DS3231_REG_STATUS             0x0F  // Flags de status e alarme

/* Aging Offset and Temperature (0x10–0x12) */
#define DS3231_REG_AGING_OFFSET       0x10  // Compensação de envelhecimento do cristal
#define DS3231_REG_TEMP_MSB           0x11  // Parte inteira da temperatura (signed)
#define DS3231_REG_TEMP_LSB           0x12  // Parte fracionária da temperatura (bits 7:6)

/****************************************************************************
**                BIT MASKS FOR CONTROL AND STATUS REGISTERS
*****************************************************************************/

/* Control Register (0x0E) */
#define DS3231_CTRL_A1IE              0x01  // Alarm 1 Interrupt Enable
#define DS3231_CTRL_A2IE              0x02  // Alarm 2 Interrupt Enable
#define DS3231_CTRL_INTCN             0x04  // Interrupt Control (1 = INT/SQW como interrupção)
#define DS3231_CTRL_RS1               0x08  // Square-wave rate select bit 1
#define DS3231_CTRL_RS2               0x10  // Square-wave rate select bit 2
#define DS3231_CTRL_CONV              0x20  // Manual temperature conversion
#define DS3231_CTRL_BBSQW             0x40  // Battery-backed square-wave enable
#define DS3231_CTRL_EOSC              0x80  // Enable Oscillator (0 = ligado)

/* Status Register (0x0F) */
#define DS3231_STAT_A1F               0x01  // Alarm 1 Flag
#define DS3231_STAT_A2F               0x02  // Alarm 2 Flag
#define DS3231_STAT_BSY               0x04  // Temperature conversion busy
#define DS3231_STAT_EN32KHZ           0x08  // Enable 32kHz output
#define DS3231_STAT_OSF               0x80  // Oscillator Stop Flag

typedef struct {
    i2c_inst_t *i2c;
    uint8_t endereco;
} DS3231;

typedef struct {
    uint8_t segundos;
    uint8_t minutos;
    uint8_t horas;
} HoraRTC;


/****************************************************************************
**                            FUNÇÕES AUXILIARES DE USO
*****************************************************************************/

/**
 * @brief Inicializa o DS3231 com pinos I2C e frequência de 400kHz
*/
void inicializa_ds3231(DS3231 *mod_rtc, i2c_inst_t *i2c, uint8_t endereco, uint sda_pin, uint scl_pin);

/**
 * @brief Lê um registrador do DS3231
*/
bool ds3231_read_reg(i2c_inst_t *i2c, uint8_t reg_addr, uint8_t *dest);

/**
 * @brief Escreve em um registrador do DS3231
 */
bool ds3231_write_reg(i2c_inst_t *i2c, uint8_t reg_addr, uint8_t value);


/**
 * @brief Converte de decimal para BCD
*/
uint8_t decimal_to_bcd(uint8_t val);

/**
 * @brief Converte de BCD para decimal
*/
uint8_t bcd_to_decimal(uint8_t val);

/**
 * @brief Retorna a hora atual lida do RTC
*/
bool hora_atual_rtc(i2c_inst_t *i2c, HoraRTC *hora);

/**
 * @brief Configura o alarme para um horário exato (minuto e segundo)
*/
bool alarme_para_horario(i2c_inst_t *i2c, uint8_t min, uint8_t seg);

/**
 * @brief Agenda o alarme para um intervalo de tempo relativo (minutos e segundos a partir de agora)
*/
bool agenda_alarme_em(i2c_inst_t *i2c, uint8_t offset_min, uint8_t offset_seg);

/*****************************END OF FILE**************************************/
#endif
