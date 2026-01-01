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
#include <matrix_service.h>

/// Serial baud rate for debugging output
const uint32_t SERIAL_BAUD_RATE = 115200;

/// BLE device name for advertising
const char* BLE_DEVICE_NAME = "SplendidBLE-Tester";

/// Test service UUID
const char* TEST_SERVICE_UUID = "10000000-1234-1234-1234-123456789abc";

/// Test characteristic UUIDs
const char* READ_WRITE_CHAR_UUID = "10000001-1234-1234-1234-123456789abc";
const char* READ_ONLY_CHAR_UUID = "10000002-1234-1234-1234-123456789abc";

/// BLE server instance
BLEServer* bleServer = nullptr;

/// Test service instance
BLEService* testService = nullptr;

/// Test characteristics
BLECharacteristic* readWriteCharacteristic = nullptr;
BLECharacteristic* readOnlyCharacteristic = nullptr;

/// Connection status tracking
bool deviceConnected = false;

/// Matrix service instance
MatrixService matrixService;

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
 * Updates matrix status based on BLE connection state.
 * 
 * Connected: Solid green
 * Advertising: Start pulsing blue animation (fire and forget)
 */
void updateLEDStatus() {
    static bool lastConnectionState = false;
    
    // Only update LED state when connection status changes
    if (deviceConnected != lastConnectionState) {
        if (deviceConnected) {
            matrixService.setSolidColor(CRGB::Green);
        } else {
            matrixService.startPulsingBlue();
        }
        lastConnectionState = deviceConnected;
    }
}

/**
 * Initializes BLE functionality.
 * 
 * Sets up BLE device, server, creates test service with one characteristic,
 * and starts advertising.
 */
void initializeBLE() {
    Serial.println("Initializing BLE...");
    
    // Initialize BLE device
    BLEDevice::init(BLE_DEVICE_NAME);
    
    // Create BLE server
    bleServer = BLEDevice::createServer();
    bleServer->setCallbacks(new ServerCallbacks());
    
    // Create test service
    testService = bleServer->createService(TEST_SERVICE_UUID);
    
    // 1. Read/Write characteristic (original)
    readWriteCharacteristic = testService->createCharacteristic(
        READ_WRITE_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    readWriteCharacteristic->setValue("Hello BLE");
    
    // 2. Read-only characteristic (device info)
    readOnlyCharacteristic = testService->createCharacteristic(
        READ_ONLY_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ
    );
    readOnlyCharacteristic->setValue("ESP32-BLE-Tester v1.0");
    
    // Start the service
    testService->start();
    
    // Start advertising
    BLEAdvertising* advertising = BLEDevice::getAdvertising();
    advertising->addServiceUUID(TEST_SERVICE_UUID);
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
  
  // Initialize matrix service
  matrixService.begin();
  Serial.println("Matrix service initialized");
  
  // Initialize BLE
  initializeBLE();
  
  // Set initial LED status (advertising = pulsing blue)
  matrixService.startPulsingBlue();
  
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
  
  // Update matrix animations
  matrixService.update();
  
  // Update LED status based on connection state
  updateLEDStatus();
  
  // Small delay to prevent excessive CPU usage
  delay(10);
}