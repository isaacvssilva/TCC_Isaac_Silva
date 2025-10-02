/*
  RadioLib LoRaWAN ABP Example

  ABP = Activation by Personalisation, an alternative
  to OTAA (Over the Air Activation). OTAA is preferable.

  This example will send uplink packets to a LoRaWAN network. 
  Before you start, you will have to register your device at 
  https://www.thethingsnetwork.org/
  After your device is registered, you can run this example.
  The device will join the network and start uploading data.

  LoRaWAN v1.0.4/v1.1 requires the use of persistent storage.
  As this example does not use persistent storage, running this 
  examples REQUIRES you to check "Resets frame counters"
  on your LoRaWAN dashboard. Refer to the notes or the 
  network's documentation on how to do this.
  To comply with LoRaWAN's persistent storage, refer to
  https://github.com/radiolib-org/radiolib-persistence

  For default module settings, see the wiki page
  https://github.com/jgromes/RadioLib/wiki/Default-configuration

  For full API reference, see the GitHub Pages
  https://jgromes.github.io/RadioLib/

  For LoRaWAN details, see the wiki page
  https://github.com/jgromes/RadioLib/wiki/LoRaWAN

*/

#ifndef MODE_DEEP_SLEEP

#include "configABP.h"
#include "utilsLorawan.h"

#define UART_ID uart0
#define BAUD_RATE 9600
#define UART_TX_PIN 0
#define UART_RX_PIN 1


void setup() {
  
  // Serial.begin(115200);
  // while(!Serial);
  // uart_init(UART_ID, BAUD_RATE);
  // gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  delay(5000);  // Give time to switch to the serial monitor
  // Serial.println(F("\nSetup ... "));
  // Serial.println(F("Initialise the radio"));
  // uart_puts(UART_ID, "Sistema iniciado\n\r");
  RadioBeginSPI();
  int state = radio.begin();
  debug(state != RADIOLIB_ERR_NONE, F("Initialise radio failed"), state, true);
  // Serial.println(F("Initialise LoRaWAN Network credentials"));
  // uart_puts(UART_ID, "Initialise LoRaWAN Network credentials\n\r");
  node.setDutyCycle(false);
  node.setDwellTime(false);
  node.beginABP(devAddr, NULL, NULL, nwkSEncKey, appSKey);
  node.activateABP(DR_SF9);
  // debug(state != RADIOLIB_ERR_NONE, F("Activate ABP failed"), state, true);
  // Serial.println(F("Ready!\n"));
  // Serial.printf("0x%x\n", node.getDevAddr());
  // uart_puts(UART_ID, "Ready!\n\r");
}

void loop() {
  // Serial.println(F("Sending uplink"));
  // uart_puts(UART_ID, "Ready!\n\r");

  // Build payload byte array
  uint8_t uplinkPayload[242];
  strcpy((char*) uplinkPayload, "Hello World");
  
  // Perform an uplink
  int state = node.sendReceive(uplinkPayload, strlen((char*) uplinkPayload));
  // Serial.println(node.getMaxPayloadLen());      
  debug(state < RADIOLIB_ERR_NONE, F("Error in sendReceive"), state, false);
  // Check if a downlink was received 
  // (state 0 = no downlink, state 1/2 = downlink in window Rx1/Rx2)
  if(state > 0) {
    // Serial.println(F("Received a downlink"));
  } else {
    // Serial.println(F("No downlink received"));
  }
  // Serial.print(F("Next uplink in "));
  // Serial.print(uplinkIntervalSeconds);
  // Serial.println(F(" seconds\n"));
  
  // Wait until next uplink - observing legal & TTN FUP constraints
  delay(uplinkIntervalSeconds * 1000UL);  // delay needs milli-seconds
}

#endif
