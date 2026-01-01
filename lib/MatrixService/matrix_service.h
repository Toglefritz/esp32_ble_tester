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
     * Creates a smooth breathing effect with blue color using a hardware timer.
     * This is a "fire and forget" method - the animation continues automatically
     * until setSolidColor() is called or another animation is started.
     */
    void startPulsingBlue();

    /**
     * Timer callback function for smooth pulsing animation.
     * 
     * This function is called by the hardware timer at regular intervals
     * to update the LED brightness for smooth animation.
     */
    static void IRAM_ATTR onTimer();

private:
    /// Number of LEDs in the ATOM Matrix (5x5 grid)
    static const uint8_t LED_COUNT = 25;
    
    /// Timer interval in microseconds (20ms = 50Hz for smooth animation)
    static const uint32_t TIMER_INTERVAL_US = 20000;
    
    /// Current pulse phase for animation (0-255)
    static uint8_t pulsePhase;
    
    /// Pulse animation speed
    static const uint8_t PULSE_SPEED = 2;
    
    /// Flag to indicate if pulsing animation is active
    static bool isPulsing;
    
    /// Hardware timer handle
    static hw_timer_t* animationTimer;
    
    /// Static instance pointer for timer callback access
    static MatrixService* instance;
    
    /**
     * Updates the pulsing blue animation.
     * 
     * Called by the timer interrupt to update LED brightness.
     */
    void updatePulsingAnimation();
};

#endif // MATRIX_SERVICE_H