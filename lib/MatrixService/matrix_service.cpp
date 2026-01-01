/**
 * Matrix Service Implementation
 * 
 * Provides LED matrix control functionality for the ESP32 BLE tester.
 * Handles solid colors and timer-based animated effects for status indication.
 */

#include "matrix_service.h"

// Static member definitions
uint8_t MatrixService::pulsePhase = 0;
bool MatrixService::isPulsing = false;
hw_timer_t* MatrixService::animationTimer = nullptr;
MatrixService* MatrixService::instance = nullptr;

void MatrixService::begin() {
    // Set static instance pointer for timer callback access
    instance = this;
    
    // Clear LED matrix and set moderate brightness
    M5.dis.clear();
    M5.dis.setBrightness(50);
    
    // Initialize hardware timer for smooth animations
    animationTimer = timerBegin(0, 80, true); // Timer 0, prescaler 80 (1MHz), count up
    timerAttachInterrupt(animationTimer, &onTimer, true); // Attach interrupt, edge type
    timerAlarmWrite(animationTimer, TIMER_INTERVAL_US, true); // Set interval, auto-reload
}

void MatrixService::setSolidColor(CRGB color) {
    // Stop any running animation
    if (isPulsing) {
        isPulsing = false;
        timerAlarmDisable(animationTimer);
    }
    
    // Set all LEDs to the specified color
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        M5.dis.drawpix(i, color);
    }
}

void MatrixService::startPulsingBlue() {
    // Reset animation state
    pulsePhase = 0;
    isPulsing = true;
    
    // Start the timer for smooth animation
    timerAlarmEnable(animationTimer);
}

void IRAM_ATTR MatrixService::onTimer() {
    // Call the instance method to update animation
    if (instance && isPulsing) {
        instance->updatePulsingAnimation();
    }
}

void MatrixService::updatePulsingAnimation() {
    // Calculate brightness using sine wave for smooth pulsing
    const float radians = (pulsePhase * 2.0 * PI) / 255.0;
    const float sineValue = sin(radians);
    
    // Convert sine wave (-1 to 1) to brightness (30 to 150)
    const uint8_t brightness = 30 + (uint8_t)((sineValue + 1.0) * 60.0);
    
    // Create blue color with calculated brightness
    const CRGB pulseColor = CRGB(0, 0, brightness);
    
    // Apply color to all LEDs
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        M5.dis.drawpix(i, pulseColor);
    }
    
    // Advance animation phase
    pulsePhase += PULSE_SPEED;
}