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

    Serial.println("STM32 Hardware Encoder Started!");
}

void encoder_loop()
{
    // Static variables keep their value between function calls
    static uint32_t last_check_time = 0;
    static uint16_t last_count = 0;

    // ASYNC TIMING: Only run this block every 50 milliseconds
    // This allows the rest of your program to run freely in the meantime
    if (millis() - last_check_time >= 50) 
    {
        last_check_time = millis(); // Reset the timer

        // Read the background hardware register
        uint16_t raw_count = TIM4->CNT;

        // ASYNC EVENT: Only process and print if the encoder actually moved!
        if (raw_count != last_count) 
        {
            last_count = raw_count; // Save the new position

            // Your math calculations
            uint16_t constrained_count = raw_count % 14400;
            float angle = (float)constrained_count * (360.0 / 14400.0);

            // Print the results
            Serial.print("Raw Count: ");
            Serial.print(raw_count);
            Serial.print(" | Angle: ");
            Serial.print(angle, 2);
            Serial.println(" degrees");
        }
    }
}