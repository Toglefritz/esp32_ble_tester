/**
 * Matrix Service Implementation
 * 
 * Provides LED matrix control functionality for the ESP32 BLE tester.
 * Handles solid colors and time-based animated effects for status indication.
 */

#include "matrix_service.h"

void MatrixService::begin() {
    // Clear LED matrix and set moderate brightness
    M5.dis.clear();
    M5.dis.setBrightness(50);
    
    // Initialize timing
    lastUpdateTime = millis();
}

void MatrixService::setSolidColor(CRGB color) {
    // Stop any running animation
    isPulsing = false;
    
    // Set all LEDs to the specified color
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        M5.dis.drawpix(i, color);
    }
}

void MatrixService::startPulsingBlue() {
    // Reset animation state
    pulsePhase = 0;
    isPulsing = true;
    lastUpdateTime = millis();
}

void MatrixService::update() {
    // Only update if pulsing animation is active
    if (!isPulsing) {
        return;
    }
    
    // Check if it's time to update the animation
    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime >= UPDATE_INTERVAL_MS) {
        updatePulsingAnimation();
        lastUpdateTime = currentTime;
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