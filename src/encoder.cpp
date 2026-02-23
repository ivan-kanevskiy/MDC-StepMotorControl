#include "main.h"
#include "encoder.h"
#include <math.h>

float constrained_angle;
int16_t raw_count;

// Allow this file to call the update function in the web server
extern void webServerBroadcastStatus();

void encoder_setup()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    GPIOD->MODER &= ~((3 << 24) | (3 << 26));
    GPIOD->MODER |= ((2 << 24) | (2 << 26));

    GPIOD->AFR[1] &= ~((0xF << 16) | (0xF << 20));
    GPIOD->AFR[1] |= ((2 << 16) | (2 << 20));

    TIM4->SMCR = 3;
    TIM4->CCMR1 = 0x0101;
    TIM4->CCER = 0x0011;
    TIM4->ARR = 0xFFFF;
    TIM4->CNT = 0;
    TIM4->CR1 = 1;
#ifdef debug
    MySerial.println("STM32 Hardware Encoder Started!");
#endif
}
String get16BitBinary(int16_t val) {
    String binStr = "";
    // Loop through all 16 bits from left (15) to right (0)
    for (int i = 15; i >= 0; i--) {
        // bitRead checks if the specific bit is a 1 or a 0
        if (bitRead(val, i)) {
            binStr += "1";
        } else {
            binStr += "0";
        }
    }
    return binStr;
}
void encoder_loop()
{
    static uint32_t last_check_time = 0;
    static int16_t last_count = 0;

    if (millis() - last_check_time >= 50)
    {
        last_check_time = millis();

        raw_count = (int16_t)TIM4->CNT;

        // If the encoder has been moved...
        if (raw_count != last_count)
        {
            last_count = raw_count;
            if(raw_count < 0)
                raw_count = -raw_count;
            if(raw_count > 3600){
                TIM4->CNT = 0;
                raw_count = (int16_t)TIM4->CNT;
            }
            MySerial.println(get16BitBinary(raw_count));

            float angle = (float)raw_count * (360.0 / 14400.0);

            constrained_angle = fmod(angle, 360.0);
            if (constrained_angle < 0)
            {
                constrained_angle += 360.0;
            }

#ifdef debug
            MySerial.print("Raw Count: ");
            MySerial.print(raw_count);
            MySerial.print(" | Angle: ");
            MySerial.println(constrained_angle, 2);
#endif
        }
    }
}