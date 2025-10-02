/*
 * =====================================================================================
 *
 *       Filename:  SHT30.hpp
 *
 *    Description:  -
 *
 *        Version:  1.0
 *        Created:  21/02/2025 10:46:35
 *       Revision:  none
 *       Compiler:  -
 *
 *         Author:  Isaac Vinicius, isaacvinicius2121@alu.ufc.br
 *   Organization:  UFC-Quixadá
 *
 * =====================================================================================
*/

#ifndef SHT30_H
#define SHT30_H

#include <Arduino.h>
#include "hardware/i2c.h"
#define UART_ID uart0

/* Definindo estrutura para armazenar os dados e configuração do sensor SHT30 */
typedef struct {
    float temperatura;   /* Armazenando temperatura medida em graus Celsius */
    float umidade;       /* Armazenando umidade relativa em porcentagem */
    uint8_t endereco;    /* Armazenando endereço I2C do sensor */
    i2c_inst_t *i2c;     /* Armazenando instância de I2C utilizada na comunicação */
} SensorSHT30;

void inicializa_sensor_sht30(SensorSHT30 *sensor, i2c_inst_t *i2c, uint8_t endereco, uint sda_pin, uint scl_pin);
bool ler_sensor_sht30(SensorSHT30 *sensor);
void exibe_dados_sht30(SensorSHT30 *sensor);
#endif
/*****************************END OF FILE**************************************/