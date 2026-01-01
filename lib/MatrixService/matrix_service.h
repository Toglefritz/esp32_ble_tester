/**
 * Matrix Service for ESP32 BLE Tester
 * 
 * Manages the 5x5 LED matrix on the M5 Stack ATOM Matrix.
 * Provides methods for solid colors and animated effects.
 */

#ifndef MATRIX_SERVICE_H
#define MATRIX_SERVICE_H

#include <M5Atom.h>

/**
 * Service class for managing LED matrix operations.
 * 
 * Handles all matrix-related functionality including solid colors,
 * animations, and status indicators for the BLE testing device.
 */
class MatrixService {
public:
    /**
     * Initializes the matrix service.
     * 
     * Sets up the LED matrix with appropriate brightness and
     * clears any existing display.
     */
    void begin();

    /**
     * Sets all matrix LEDs to a solid color.
     * 
     * Stops any running animations and sets a static color.
     * 
     * @param color RGB color value for all LEDs
     */
    void setSolidColor(CRGB color);

    /**
     * Starts the pulsing blue animation for advertising mode.
     * 
     * Creates a smooth breathing effect with blue color.
     * Call update() regularly in the main loop to maintain animation.
     */
    void startPulsingBlue();

    /**
     * Updates the animation state.
     * 
     * Call this regularly in the main loop to update any active animations.
     * Uses millis() for smooth timing without interrupts.
     */
    void update();

private:
    /// Number of LEDs in the ATOM Matrix (5x5 grid)
    static const uint8_t LED_COUNT = 25;
    
    /// Animation update interval in milliseconds (20ms = 50Hz)
    static const uint32_t UPDATE_INTERVAL_MS = 20;
    
    /// Current pulse phase for animation (0-255)
    uint8_t pulsePhase = 0;
    
    /// Pulse animation speed
    static const uint8_t PULSE_SPEED = 2;
    
    /// Flag to indicate if pulsing animation is active
    bool isPulsing = false;
    
    /// Last update time for smooth animation timing
    unsigned long lastUpdateTime = 0;
    
    /**
     * Updates the pulsing blue animation.
     * 
     * Called by update() when pulsing animation is active.
     */
    void updatePulsingAnimation();
};

#endif // MATRIX_SERVICE_H