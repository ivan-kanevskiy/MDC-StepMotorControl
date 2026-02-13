#include "main.h"
#include "encoder.h"
// The user LED on the Olimex STM32-E407 is mapped to pin PC13
const int USER_LED = PC13; 

void setup() {

  pinMode(USER_LED, OUTPUT);
  encoder_setup();
}

void loop() {
  encoder_loop();
  digitalWrite(USER_LED, HIGH);  // Turn the LED on
  delay(500);                    // Wait for half a second (500ms)
  
  digitalWrite(USER_LED, LOW);   // Turn the LED off
  delay(500);                    // Wait for another half second
}