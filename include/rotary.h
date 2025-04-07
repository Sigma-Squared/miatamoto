#ifndef ROTARY_H
#define ROTARY_H
#define DEBUG true
#include <DEBUG.h>
#include <Arduino.h>
// Define encoder pins
#define ENCODER_PIN_A 12
#define ENCODER_PIN_B 13

volatile int encoderPosition = 0;
volatile int lastEncoded = 0;

void IRAM_ATTR ISR_encoder(void)
{
    // Read both pins
    int MSB = digitalRead(ENCODER_PIN_A); // Most significant bit
    int LSB = digitalRead(ENCODER_PIN_B); // Least significant bit

    int encoded = (MSB << 1) | LSB; // Convert the two digital reads into a 2-bit number
    int sum = (lastEncoded << 2) | encoded;

    // Detect direction based on state changes (gray code logic)
    if (sum == 0b0010 || sum == 0b1001 || sum == 0b0110 || sum == 0b1100)
    {
        encoderPosition++;
    }
    else if (sum == 0b0001 || sum == 0b1010 || sum == 0b0101 || sum == 0b1111)
    {
        encoderPosition--;
    }

    lastEncoded = encoded;
}

void setup_encoder()
{
    // Configure pins as inputs with internal pullups
    pinMode(ENCODER_PIN_A, INPUT_PULLUP);
    pinMode(ENCODER_PIN_B, INPUT_PULLUP);

    attachInterrupt(ENCODER_PIN_A, ISR_encoder, CHANGE);
    attachInterrupt(ENCODER_PIN_B, ISR_encoder, CHANGE);
    DEBUG_PRINTLN("Encoder setup complete.");
}
#endif