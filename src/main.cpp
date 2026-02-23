#include "main.h"
#include "encoder.h"
#include "WebServerEthernet.h"
#include "motor.h"
// The user LED on the Olimex STM32-E407 is mapped to pin PC13
const int USER_LED = PC13; 


HardwareSerial MySerial(PB7, PB6); // RX = PB7 (D0), TX = PB6 (D1)
void setup() {
  MySerial.begin(1000000);
  pinMode(USER_LED, OUTPUT);
  encoder_setup();
  WebServerInit();
  mottor_setup();
  #ifdef debug
  MySerial.println("\n--- SYSTEM BOOTING ON D0/D1 ---");
  #endif
}

void loop() {
  encoder_loop();
  mottor_loop();
  webServerUpdateLoop();
}