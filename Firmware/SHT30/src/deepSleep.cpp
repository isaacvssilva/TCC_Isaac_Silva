/*
 * =====================================================================================
 *
 *       Filename:  deepSleep.cpp
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

#ifdef MODE_DEEP_SLEEP

#include <rosc.h>
#include <sleep.h>
#include "hardware/rtc.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/xosc.h"
#include "hardware/structs/scb.h"
#include "hardware/uart.h"
#include "pico/runtime_init.h"
#include "../lib/sht30/SHT30.hpp"
#include "../lib/ds3231_rtc/ds3231.hpp"

#define UART_ID uart0
#define BAUD_RATE 9600
#define UART_TX_PIN 0

#define I2C_SDA_PIN 26
#define I2C_SCL_PIN 27
#define WAKE_GPIO 28

extern SensorSHT30 sht30;
extern DS3231 rtc_ds3231;

/* Habilitando função de callback para tratar interrupções na GPIO */
void gpio_callback(uint gpio, uint32_t events) {}

/* Declarando variáveis para salvar o estado atual dos clocks */
static uint scb_orig;
static uint clock0_orig;
static uint clock1_orig;

/*
* ===  FUNCTION  ======================================================================
*         Name:  enter_low_power_sleep_until_interrupt
*  Description:  Função auxiliar para configurar o microcontrolador para entrar
*                em modo dormant (deep sleep), desligando todos os periféricos 
*                mas deixando o clock de PWM para leitura do sensor hall em deep sleep.
* =====================================================================================
*/

void enter_low_power_sleep_until_interrupt(void) {
  /* Salvando o estado atual do registrador SCR (System Control Register) */
  scb_orig = scb_hw->scr;

  /* Salvando o estado atual dos registradores de clock dos periféricos */
  clock0_orig = clocks_hw->sleep_en0;
  clock1_orig = clocks_hw->sleep_en1;

  /* Mantendo apenas o clock do PWM ativo durante o modo sleep */
  clocks_hw->sleep_en0 = CLOCKS_SLEEP_EN0_CLK_SYS_PWM_BITS;
  clocks_hw->sleep_en1 = 0x0;

  /* Ativando o bit de deep sleep no registrador SCR */
  uint save = scb_hw->scr;
  scb_hw->scr = save | M0PLUS_SCR_SLEEPDEEP_BITS;

  /* Entrando em modo de baixo consumo até que ocorra uma interrupção */
  __wfi();
}

/*
* ===  FUNCTION  ======================================================================
*         Name:  recover_from_sleep
*  Description:  Função auxiliar para restaurar os clocks do microcontrolador após 
*                sair do modo dormant (deep sleep).
* =====================================================================================
*/
void recover_from_sleep(uint scb_orig, uint clock0_orig, uint clock1_orig) {
  rosc_write(&rosc_hw->ctrl, ROSC_CTRL_ENABLE_BITS);
  scb_hw->scr = scb_orig;
  clocks_hw->sleep_en0 = clock0_orig;
  clocks_hw->sleep_en1 = clock1_orig;
  clocks_init();
}


void setup() {

  /* Inicializando UART */
  uart_init(UART_ID, BAUD_RATE);
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART); 

  /* Inicializando o módulo DS3231 com a instância I2C e os pinos definidos */
  inicializa_ds3231(&rtc_ds3231, i2c1, DS3231_I2C_ADDR, I2C_SDA_PIN, I2C_SCL_PIN);

  /* Inicializando sensor SHT30 via barramento I2C */
  inicializa_sensor_sht30(&sht30, i2c1, 0x44, I2C_SDA_PIN, I2C_SCL_PIN);

  uart_init(UART_ID, BAUD_RATE);
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART); 

  /* Configurando GPIO de wake-up como entrada (SQW/INT) */
  gpio_init(WAKE_GPIO);
  gpio_set_dir(WAKE_GPIO, GPIO_IN);

  /* Limpando possível alarme pendente */
  uint8_t stat; /* Declarando variável para armazenar o conteúdo do registrador de status */

  /* Lendo o registrador de status (0x0F) para verificar flags ativas */
  ds3231_read_reg(rtc_ds3231.i2c, DS3231_REG_STATUS, &stat);

  if (stat & DS3231_STAT_A1F) {
    /* Limpando a flag A1F (bit 0), necessária para permitir futuros alarmes */
    stat &= ~DS3231_STAT_A1F;

    /* Escrevendo de volta no registrador de status com a flag A1F zerada */
    ds3231_write_reg(rtc_ds3231.i2c, DS3231_REG_STATUS, stat);
  }

  /* Agendando um novo alarme relativo */
  agenda_alarme_em(rtc_ds3231.i2c, 0, 10);  // Alarme em 1 minuto e 30 segundos

  /* Configurando interrupção na GPIO de wake-up para borda de descida */
  gpio_set_irq_enabled_with_callback(
    WAKE_GPIO,
    GPIO_IRQ_EDGE_FALL,
    true,
    &gpio_callback
  );

  uart_puts(UART_ID, "Sistema iniciado!!\n\r");
  uart_default_tx_wait_blocking();
}

      
void loop() {
    
  /* Configurando sistema para executar a partir do cristal externo (XOSC) */
  sleep_run_from_xosc();
  
  /* Entrando em modo de baixo consumo até ocorrência de interrupção */
  enter_low_power_sleep_until_interrupt();
  
  /* Restaurando estado dos clocks após o modo Sleep */
  recover_from_sleep(scb_orig, clock0_orig, clock1_orig);
  
  /* Reconfigurando UART e notificando início do envio LoRa */
  uart_init(UART_ID, BAUD_RATE);
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);

  uart_puts(UART_ID, "Entrando no modo operacao\n\r");
  uart_default_tx_wait_blocking();
  
  /* Lendo dados do sensor SHT30 (umidade e temperatura) */
  if (!ler_sensor_sht30(&sht30)) {
    /* Informando erro na leitura do sensor via UART */
    uart_puts(UART_ID, "Erro ao ler sensor SHT30!\n\r");
    uart_default_tx_wait_blocking();
    return;
  }

  /* Formatando mensagem com dados lidos (umidade, temperatura e precipitação) */
  char message[100];
  snprintf(message, sizeof(message),
          "Hum=%.1f%%,Temp=%.1fC",
          sht30.umidade, sht30.temperatura);

  /* Enviando mensagem formatada via UART */
  uart_puts(UART_ID, message);
  uart_puts(UART_ID, "\n\r");
  uart_default_tx_wait_blocking();


  /* Reagendando alarme */
  uint8_t stat;
  ds3231_read_reg(i2c1, DS3231_REG_STATUS, &stat);

  if (stat & DS3231_STAT_A1F) {
    /* Limpando a flag A1F para permitir que futuros eventos de alarme */
    stat &= ~DS3231_STAT_A1F;
    ds3231_write_reg(i2c1, DS3231_REG_STATUS, stat);

    /* Agendando um novo alarme */
    agenda_alarme_em(i2c1, 0, 10);
    // uart_puts(UART_ID, ">> Alarme tratado e reagendado <<\n\r");
    // uart_tx_wait_blocking(UART_ID);
  }

  uart_puts(UART_ID, ">> Indo dormir novamente... <<\n\r");
  uart_tx_wait_blocking(UART_ID);
}

#endif
/*****************************END OF FILE**************************************/