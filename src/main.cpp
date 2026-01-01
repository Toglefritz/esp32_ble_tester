/**
 * ESP32 BLE Testing Device - Main Firmware
 * 
 * Basic firmware for M5 Stack ATOM Matrix that initializes serial communication
 * and displays a rainbow animation on the LED matrix. This serves as the foundation
 * for the BLE testing platform.
 * 
 * Hardware: M5 Stack ATOM Matrix (ESP32-PICO-D4)
 * Features: 5x5 RGB LED matrix, WiFi/BLE capability, USB-C
 */

#include <M5Atom.h>

/// Number of LEDs in the ATOM Matrix (5x5 grid)
const uint8_t LED_COUNT = 25;

/// Current hue value for rainbow animation (0-255)
uint8_t currentHue = 0;

/// Animation speed delay in milliseconds
const uint16_t ANIMATION_DELAY_MS = 50;

/// Serial baud rate for debugging output
const uint32_t SERIAL_BAUD_RATE = 115200;

/**
 * Converts HSV color values to RGB for LED display.
 * 
 * This function creates smooth color transitions for the rainbow effect
 * by converting hue, saturation, and value parameters to RGB values.
 * 
 * @param hue Color hue (0-255, wraps around for continuous rainbow)
 * @param saturation Color saturation (0-255, 255 = fully saturated)
 * @param value Brightness value (0-255, 255 = maximum brightness)
 * @returns CRGB color value for FastLED library
 */
CRGB hsvToRgb(uint8_t hue, uint8_t saturation, uint8_t value) {
  CRGB color;
  color.setHSV(hue, saturation, value);
  return color;
}

/**
 * Updates the LED matrix with a rainbow pattern.
 * 
 * Creates a flowing rainbow effect across all 25 LEDs by assigning
 * different hue values to each LED position. The pattern shifts
 * continuously as the currentHue value increments.
 */
void updateRainbowAnimation() {
  // Calculate hue offset for smooth rainbow distribution across LEDs
  const uint8_t hueStep = 255 / LED_COUNT;
  
  for (uint8_t ledIndex = 0; ledIndex < LED_COUNT; ledIndex++) {
    // Calculate hue for this LED position with current animation offset
    const uint8_t ledHue = currentHue + (ledIndex * hueStep);
    
    // Set LED color with full saturation and moderate brightness
    const CRGB ledColor = hsvToRgb(ledHue, 255, 100);
    M5.dis.drawpix(ledIndex, ledColor);
  }
  
  // Advance animation by incrementing hue (wraps at 255)
  currentHue++;
}

/**
 * Initializes the device hardware and serial communication.
 * 
 * Sets up the M5 Stack ATOM Matrix hardware, initializes the LED matrix,
 * and establishes serial communication for debugging output.
 */
void setup() {
  // Initialize M5 Stack ATOM hardware
  // Parameters: SerialEnable, I2CEnable, DisplayEnable
  M5.begin(true, false, true);
  
  // Initialize serial communication for debugging
  Serial.begin(SERIAL_BAUD_RATE);
  
  // Wait for serial connection to stabilize
  delay(100);
  
  // Print startup message
  Serial.println("=== ESP32 BLE Testing Device ===");
  Serial.println("Hardware: M5 Stack ATOM Matrix");
  Serial.println("Status: Initializing...");
  Serial.println();
  
  // Clear LED matrix and set initial brightness
  M5.dis.clear();
  M5.dis.setBrightness(50); // Moderate brightness to prevent eye strain
  
  Serial.println("LED matrix initialized");
  Serial.println("Starting rainbow animation...");
  Serial.println("Ready for development!");
}

/**
 * Main program loop.
 * 
 * Continuously updates the rainbow animation on the LED matrix
 * and handles any button presses or other hardware events.
 */
void loop() {
  // Update hardware state (buttons, etc.)
  M5.update();
  
  // Update LED matrix with rainbow animation
  updateRainbowAnimation();
  
  // Control animation speed
  delay(ANIMATION_DELAY_MS);
  
  // Optional: Handle button press for future functionality
  if (M5.Btn.wasPressed()) {
    Serial.println("Button pressed - ready for BLE functionality!");
  }
}