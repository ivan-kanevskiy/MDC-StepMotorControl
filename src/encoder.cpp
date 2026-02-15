#include "main.h"
#include "encoder.h"

void encoder_setup()
{
    // ---------------------------------------------------------
    // 1. ENABLE CLOCKS FOR PORT D AND TIMER 4
    // ---------------------------------------------------------
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Turn on power to Port D
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;  // Turn on power to Timer 4

    // ---------------------------------------------------------
    // 2. CONFIGURE PINS PD12 & PD13 FOR HARDWARE TIMER
    // ---------------------------------------------------------
    GPIOD->MODER &= ~((3 << 24) | (3 << 26));
    GPIOD->MODER |= ((2 << 24) | (2 << 26));

    GPIOD->AFR[1] &= ~((0xF << 16) | (0xF << 20));
    GPIOD->AFR[1] |= ((2 << 16) | (2 << 20));

    // ---------------------------------------------------------
    // 3. CONFIGURE TIMER 4 AS A QUADRATURE ENCODER
    // ---------------------------------------------------------
    TIM4->SMCR = 3;       
    TIM4->CCMR1 = 0x0101; 
    TIM4->CCER = 0x0011;  
    TIM4->ARR = 0xFFFF;   
    TIM4->CNT = 0;        
    TIM4->CR1 = 1;        

    MySerial.println("STM32 Hardware Encoder Started!");
}

void encoder_loop()
{
    static uint32_t last_check_time = 0;
    static int16_t last_count = 0; // Changed to signed!

    if (millis() - last_check_time >= 50) 
    {
        last_check_time = millis();

        // 1. Cast the hardware register to a SIGNED 16-bit integer
        int16_t raw_count = (int16_t)TIM4->CNT;

        if (raw_count != last_count) 
        {
            last_count = raw_count;

            // 2. Calculate continuous angle (can go negative!)
            float angle = (float)raw_count * (360.0 / 14400.0);

            // 3. Optional: Constrain the angle purely between 0.00 and 359.99
            // If you want it to act like a compass instead of a continuous dial
            float constrained_angle = fmod(angle, 360.0);
            if (constrained_angle < 0) {
                constrained_angle += 360.0;
            }

            MySerial.print("Raw Count: ");
            MySerial.print(raw_count);
            MySerial.print(" | Angle: ");
            MySerial.print(constrained_angle, 2);
            MySerial.println(" degrees");
        }
    }
}