#include "main.h"
#include "encoder.h"
// The user LED on the Olimex STM32-E407 is mapped to pin PC13
const int USER_LED = PC13; 


HardwareSerial MySerial(PB7, PB6); // RX = PB7 (D0), TX = PB6 (D1)
void setup() {
  MySerial.begin(1000000);
  pinMode(USER_LED, OUTPUT);
  encoder_setup();
  MySerial.println("\n--- SYSTEM BOOTING ON D0/D1 ---");
}

void loop() {
  encoder_loop();
  static uint32_t last_led_time = 0;
  static bool led_state = false;

  if (millis() - last_led_time >= 500) {
      last_led_time = millis(); // Reset timer

      led_state = !led_state;   // Flip the state

      if (led_state) {
          digitalWrite(USER_LED, HIGH);
          MySerial.println("On");
      } else {
          digitalWrite(USER_LED, LOW);
          MySerial.println("OFF"); 
      }
  }
}