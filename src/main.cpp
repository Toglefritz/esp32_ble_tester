/**
 * ESP32 BLE Testing Device - Main Firmware
 * 
 * BLE testing firmware for M5 Stack ATOM Matrix that provides standardized
 * BLE peripheral functionality for testing the Flutter Splendid BLE plugin.
 * 
 * Features:
 * - BLE advertising with configurable device name
 * - Connection state management with LED status indicators
 * - Serial logging for debugging and monitoring
 * - Visual feedback via 5x5 RGB LED matrix
 * 
 * LED Status Indicators:
 * - Blue: Advertising (ready for connections)
 * - Green: Connected to a client
 * - Red: Error state
 * 
 * Hardware: M5 Stack ATOM Matrix (ESP32-PICO-D4)
 */

#include <M5Atom.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

/// Number of LEDs in the ATOM Matrix (5x5 grid)
const uint8_t LED_COUNT = 25;

/// Serial baud rate for debugging output
const uint32_t SERIAL_BAUD_RATE = 115200;

/// BLE device name for advertising
const char* BLE_DEVICE_NAME = "SplendidBLE-Tester";

/// BLE server instance
BLEServer* bleServer = nullptr;

/// Connection status tracking
bool deviceConnected = false;

/**
 * BLE Server Callbacks
 * 
 * Handles connection and disconnection events from BLE clients.
 * Updates the connection status and triggers LED status changes.
 */
class ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* server) {
        deviceConnected = true;
        Serial.println("BLE Client connected");
    }

    void onDisconnect(BLEServer* server) {
        deviceConnected = false;
        Serial.println("BLE Client disconnected - restarting advertising");
        BLEDevice::startAdvertising();
    }
};

/**
 * Sets all LEDs to a solid color.
 * 
 * @param color RGB color value for all LEDs
 */
void setAllLEDs(CRGB color) {
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        M5.dis.drawpix(i, color);
    }
}

/**
 * Updates LED status based on BLE connection state.
 * 
 * Blue: Advertising (ready for connections)
 * Green: Connected to a client
 */
void updateLEDStatus() {
    if (deviceConnected) {
        setAllLEDs(CRGB::Green);
    } else {
        setAllLEDs(CRGB::Blue);
    }
}

/**
 * Initializes BLE functionality.
 * 
 * Sets up BLE device, server, and starts advertising.
 */
void initializeBLE() {
    Serial.println("Initializing BLE...");
    
    // Initialize BLE device
    BLEDevice::init(BLE_DEVICE_NAME);
    
    // Create BLE server
    bleServer = BLEDevice::createServer();
    bleServer->setCallbacks(new ServerCallbacks());
    
    // Start advertising
    BLEAdvertising* advertising = BLEDevice::getAdvertising();
    advertising->addServiceUUID("12345678-1234-1234-1234-123456789abc");
    advertising->setScanResponse(false);
    advertising->setMinPreferred(0x0);
    BLEDevice::startAdvertising();
    
    Serial.println("BLE advertising started");
    Serial.print("Device name: ");
    Serial.println(BLE_DEVICE_NAME);
}

/**
 * Initializes the device hardware and BLE functionality.
 * 
 * Sets up the M5 Stack ATOM Matrix hardware, initializes the LED matrix,
 * establishes serial communication, and starts BLE advertising.
 */
void setup() {
  // Initialize M5 Stack ATOM hardware
  M5.begin(true, false, true);
  
  // Initialize serial communication
  Serial.begin(SERIAL_BAUD_RATE);
  delay(100);
  
  // Print startup message
  Serial.println("=== ESP32 BLE Testing Device ===");
  Serial.println("Hardware: M5 Stack ATOM Matrix");
  Serial.println("Status: Initializing...");
  Serial.println();
  
  // Initialize LED matrix
  M5.dis.clear();
  M5.dis.setBrightness(50);
  Serial.println("LED matrix initialized");
  
  // Initialize BLE
  initializeBLE();
  
  // Set initial LED status (advertising = blue)
  updateLEDStatus();
  
  Serial.println("Device ready for BLE connections");
}

/**
 * Main program loop.
 * 
 * Continuously monitors BLE connection status and updates LED indicators.
 * Handles button presses and maintains BLE functionality.
 */
void loop() {
  // Update hardware state
  M5.update();
  
  // Update LED status based on connection state
  updateLEDStatus();
  
  // Handle button press for debugging
  if (M5.Btn.wasPressed()) {
    Serial.print("Button pressed - Connection status: ");
    Serial.println(deviceConnected ? "Connected" : "Advertising");
  }
  
  // Small delay to prevent excessive CPU usage
  delay(100);
}