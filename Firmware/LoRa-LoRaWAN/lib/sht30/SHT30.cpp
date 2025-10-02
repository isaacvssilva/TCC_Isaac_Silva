/*
 * =====================================================================================
 *
 *       Filename:  SHT30.cpp
 *
 *    Description:  -
 *
 *        Version:  1.0
 *        Created:  21/02/2025 10:49:22
 *       Revision:  none
 *       Compiler:  -
 *
 *         Author:  Isaac Vinicius, isaacvinicius2121@alu.ufc.br
 *   Organization:  UFC-Quixadá
 *
 * =====================================================================================
*/

#include "SHT30.hpp"

/* Declarando estrutura global para armazenar dados do sensor SHT30 */
SensorSHT30 sht30;

void inicializa_sensor_sht30(SensorSHT30 *sensor, i2c_inst_t *i2c, uint8_t endereco, uint sda_pin, uint scl_pin) {
  /* Inicializando valores de temperatura e umidade como zero */
  sensor->temperatura = 0.0;
  sensor->umidade = 0.0;

  /* Armazenando endereço e instância de I2C na estrutura */
  sensor->endereco = endereco;
  sensor->i2c = i2c;

  /* Inicializando comunicação I2C com frequência de 400 kHz */
  i2c_init(i2c, 400 * 1000);

  /* Configurando pinos SDA e SCL para função I2C */
  gpio_set_function(sda_pin, GPIO_FUNC_I2C);
  gpio_set_function(scl_pin, GPIO_FUNC_I2C);

  /* Habilitando resistores de pull-up nos pinos SDA e SCL */
  // gpio_pull_up(sda_pin);
  // gpio_pull_up(scl_pin);
}

bool ler_sensor_sht30(SensorSHT30 *sensor) {
  /* Enviando comando de medição para o sensor */
  uint8_t config[2] = {0x2C, 0x06};
  if (i2c_write_blocking(sensor->i2c, sensor->endereco, config, 2, false) != 2) {
    return false;  /* Retornando erro se não for possível enviar o comando */
  }

  /* Aguardando tempo necessário para a medição (15 ms) */
  sleep_ms(15);

  /* Lendo 6 bytes com os dados de temperatura e umidade */
  uint8_t data[6] = {0};
  if (i2c_read_blocking(sensor->i2c, sensor->endereco, data, 6, false) != 6) {
    return false;  /* Retornando erro se leitura falhar */
  }

  /* Convertendo dados brutos em temperatura e umidade reais */
  uint16_t raw_temp = (data[0] << 8) | data[1];
  uint16_t raw_humidity = (data[3] << 8) | data[4];

  /* Calculando temperatura em graus Celsius */
  sensor->temperatura = ((175.0 * raw_temp) / 65535.0) - 45.0;

  /* Calculando umidade relativa em porcentagem */
  sensor->umidade = (100.0 * raw_humidity) / 65535.0;

  return true;  /* Retornando sucesso na leitura */
}

void exibe_dados_sht30(SensorSHT30 *sensor) {
  /* Formatando string com temperatura e umidade */
  char buffer[100];
  snprintf(buffer, sizeof(buffer), 
           "Temperatura: %.2f C | Umidade: %.2f%%\n\r", 
           sensor->temperatura, sensor->umidade);

  /* Enviando dados formatados via UART */
  uart_puts(UART_ID, buffer);
  uart_default_tx_wait_blocking();
}


/*****************************END OF FILE**************************************/