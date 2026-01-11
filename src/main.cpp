/**
 * ESP32 BLE Testing Device - Main Firmware
 * 
 * BLE testing firmware for M5 Stack ATOM Matrix that provides standardized
 * BLE peripheral functionality for testing the Flutter Splendid BLE plugin.
 * 
 * Features:
 * - BLE advertising with configurable device name
 * - Manufacturer data in both advertisement and scan response
 * - Connection state management with LED status indicators
 * - Serial logging for debugging and monitoring
 * - Visual feedback via 5x5 RGB LED matrix
 * 
 * Manufacturer Data:
 * - Advertisement: Company ID 0xFFFF + consecutive bytes 0x00-0x0F
 * - Scan Response: Company ID 0xFFFF + consecutive bytes 0x10-0x1F
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

/// Manufacturer ID for manufacturer data (using test company ID)
const uint16_t MANUFACTURER_ID = 0xFFFF; // Reserved for testing

/// Test service UUID
const char* TEST_SERVICE_UUID = "10000000-1234-1234-1234-123456789abc";

/// Test characteristic UUIDs
const char* READ_WRITE_CHAR_UUID = "10000001-1234-1234-1234-123456789abc";
const char* READ_ONLY_CHAR_UUID = "10000002-1234-1234-1234-123456789abc";
const char* WRITE_ONLY_CHAR_UUID = "10000003-1234-1234-1234-123456789abc";
const char* NOTIFY_CHAR_UUID = "10000004-1234-1234-1234-123456789abc";
const char* LARGE_DATA_CHAR_UUID = "10000005-1234-1234-1234-123456789abc";

/// BLE server instance
BLEServer* bleServer = nullptr;

/// Test service instance
BLEService* testService = nullptr;

/// Test characteristics
BLECharacteristic* readWriteCharacteristic = nullptr;
BLECharacteristic* readOnlyCharacteristic = nullptr;
BLECharacteristic* writeOnlyCharacteristic = nullptr;
BLECharacteristic* notifyCharacteristic = nullptr;
BLECharacteristic* largeDataCharacteristic = nullptr;

/// Connection status tracking
bool deviceConnected = false;

/// Notification timing
unsigned long lastNotifyTime = 0;
const unsigned long NOTIFY_INTERVAL_MS = 3000; // Send notification every 3 seconds
uint32_t notifyCounter = 0;

/// Matrix service instance
MatrixService matrixService;

/**
 * Creates manufacturer data with consecutive numbers for easy verification.
 * 
 * Creates a manufacturer data payload containing:
 * - Manufacturer ID (2 bytes): 0xFFFF (test company ID)
 * - Data payload: consecutive numbers 0x00 through 0x0F (16 bytes)
 * 
 * @param data Buffer to store the manufacturer data
 * @param maxSize Maximum size of the buffer
 * @returns Actual size of the manufacturer data created
 */
size_t createManufacturerData(uint8_t* data, size_t maxSize) {
    if (maxSize < 18) { // Need at least 2 bytes for ID + 16 bytes for data
        return 0;
    }
    
    // Add manufacturer ID (little-endian)
    data[0] = MANUFACTURER_ID & 0xFF;
    data[1] = (MANUFACTURER_ID >> 8) & 0xFF;
    
    // Add consecutive numbers 0x00 through 0x0F
    for (int i = 0; i < 16; i++) {
        data[2 + i] = i;
    }
    
    return 18; // 2 bytes ID + 16 bytes data
}

/**
 * Creates scan response manufacturer data with different consecutive numbers.
 * 
 * Creates a scan response manufacturer data payload containing:
 * - Manufacturer ID (2 bytes): 0xFFFF (test company ID)
 * - Data payload: consecutive numbers 0x10 through 0x1F (16 bytes)
 * 
 * @param data Buffer to store the manufacturer data
 * @param maxSize Maximum size of the buffer
 * @returns Actual size of the manufacturer data created
 */
size_t createScanResponseManufacturerData(uint8_t* data, size_t maxSize) {
    if (maxSize < 18) { // Need at least 2 bytes for ID + 16 bytes for data
        return 0;
    }
    
    // Add manufacturer ID (little-endian)
    data[0] = MANUFACTURER_ID & 0xFF;
    data[1] = (MANUFACTURER_ID >> 8) & 0xFF;
    
    // Add consecutive numbers 0x10 through 0x1F for scan response
    for (int i = 0; i < 16; i++) {
        data[2 + i] = 0x10 + i;
    }
    
    return 18; // 2 bytes ID + 16 bytes data
}

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
 * Write-Only Characteristic Callbacks
 * 
 * Handles write operations to the write-only characteristic.
 * Processes commands and provides serial feedback.
 */
class WriteOnlyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* characteristic) {
        std::string value = characteristic->getValue();
        Serial.print("Write-only command received: ");
        Serial.println(value.c_str());
        
        // Process simple commands
        if (value == "ping") {
            Serial.println("Command processed: pong");
        } else if (value == "status") {
            Serial.println("Command processed: Device is running");
        } else {
            Serial.println("Command processed: Unknown command");
        }
    }
};

/**
 * Large Data Characteristic Callbacks
 * 
 * Handles write operations to the large data characteristic.
 * Logs data size for MTU and chunking tests.
 */
class LargeDataCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* characteristic) {
        std::string value = characteristic->getValue();
        Serial.print("Large data received: ");
        Serial.print(value.length());
        Serial.println(" bytes");
        
        // Log first and last few characters for verification
        if (value.length() > 20) {
            Serial.print("Start: ");
            Serial.println(value.substr(0, 10).c_str());
            Serial.print("End: ");
            Serial.println(value.substr(value.length() - 10).c_str());
        } else {
            Serial.print("Data: ");
            Serial.println(value.c_str());
        }
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
 * Sends periodic notifications to subscribed clients.
 * 
 * Sends a counter value every 3 seconds to test notification functionality.
 */
void handleNotifications() {
    if (deviceConnected && (millis() - lastNotifyTime >= NOTIFY_INTERVAL_MS)) {
        String notifyValue = "Counter: " + String(notifyCounter);
        notifyCharacteristic->setValue(notifyValue.c_str());
        notifyCharacteristic->notify();
        
        Serial.print("Notification sent: ");
        Serial.println(notifyValue);
        
        notifyCounter++;
        lastNotifyTime = millis();
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
    
    // 3. Write-only characteristic (commands)
    writeOnlyCharacteristic = testService->createCharacteristic(
        WRITE_ONLY_CHAR_UUID,
        BLECharacteristic::PROPERTY_WRITE
    );
    writeOnlyCharacteristic->setCallbacks(new WriteOnlyCallbacks());
    
    // 4. Notify characteristic (periodic updates)
    notifyCharacteristic = testService->createCharacteristic(
        NOTIFY_CHAR_UUID,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    notifyCharacteristic->addDescriptor(new BLE2902());
    
    // 5. Large data characteristic (MTU testing)
    largeDataCharacteristic = testService->createCharacteristic(
        LARGE_DATA_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    // Create 500-byte test string for MTU testing
    std::string largeData = "LARGE_DATA_TEST:";
    for (int i = 0; i < 30; i++) {
        largeData += "0123456789ABCDEF"; // 16 chars per iteration
    }
    largeData += ":END"; // Total ~500 bytes
    largeDataCharacteristic->setValue(largeData);
    largeDataCharacteristic->setCallbacks(new LargeDataCallbacks());
    
    // Start the service
    testService->start();
    
    // Start advertising
    BLEAdvertising* advertising = BLEDevice::getAdvertising();
    
    // Create advertisement data with both service UUID and manufacturer data
    BLEAdvertisementData advertisementData;
    advertisementData.setFlags(0x06); // BR_EDR_NOT_SUPPORTED | LE_GENERAL_DISC_MODE
    
    // Add service UUID to advertisement data
    advertisementData.setCompleteServices(BLEUUID(TEST_SERVICE_UUID));
    
    // Create and add manufacturer data to advertisement
    uint8_t manufacturerData[32];
    size_t manufacturerDataSize = createManufacturerData(manufacturerData, sizeof(manufacturerData));
    if (manufacturerDataSize > 0) {
        std::string manufacturerDataStr((char*)manufacturerData, manufacturerDataSize);
        advertisementData.setManufacturerData(manufacturerDataStr);
        Serial.print("Advertisement manufacturer data added: ");
        Serial.print(manufacturerDataSize);
        Serial.println(" bytes");
        
        // Log the data for verification
        Serial.print("  Data: ");
        for (size_t i = 0; i < manufacturerDataSize; i++) {
            Serial.printf("0x%02X ", manufacturerData[i]);
        }
        Serial.println();
    }
    
    // Set the complete advertisement data (includes both service UUID and manufacturer data)
    advertising->setAdvertisementData(advertisementData);
    
    // Set advertising parameters
    advertising->setScanResponse(true);
    advertising->setMinPreferred(0x06);
    advertising->setMaxPreferred(0x12);
    
    // Create and add manufacturer data to scan response
    uint8_t scanResponseData[32];
    size_t scanResponseDataSize = createScanResponseManufacturerData(scanResponseData, sizeof(scanResponseData));
    if (scanResponseDataSize > 0) {
        BLEAdvertisementData scanResponseAdv;
        std::string scanResponseDataStr((char*)scanResponseData, scanResponseDataSize);
        scanResponseAdv.setManufacturerData(scanResponseDataStr);
        
        // Set the scan response data
        advertising->setScanResponseData(scanResponseAdv);
        Serial.print("Scan response manufacturer data added: ");
        Serial.print(scanResponseDataSize);
        Serial.println(" bytes");
        
        // Log the data for verification
        Serial.print("  Data: ");
        for (size_t i = 0; i < scanResponseDataSize; i++) {
            Serial.printf("0x%02X ", scanResponseData[i]);
        }
        Serial.println();
    }
    
    // Start advertising
    BLEDevice::startAdvertising();
    
    Serial.println("BLE advertising started");
    Serial.print("Device name: ");
    Serial.println(BLE_DEVICE_NAME);
    Serial.print("Test service UUID: ");
    Serial.println(TEST_SERVICE_UUID);
    Serial.print("Manufacturer ID: 0x");
    Serial.println(MANUFACTURER_ID, HEX);
    Serial.println("Manufacturer data:");
    Serial.println("  Advertisement: 0x00-0x0F (consecutive numbers)");
    Serial.println("  Scan response: 0x10-0x1F (consecutive numbers)");
    Serial.println("Characteristics:");
    Serial.print("  Read/Write: ");
    Serial.println(READ_WRITE_CHAR_UUID);
    Serial.print("  Read-only: ");
    Serial.println(READ_ONLY_CHAR_UUID);
    Serial.print("  Write-only: ");
    Serial.println(WRITE_ONLY_CHAR_UUID);
    Serial.print("  Notify: ");
    Serial.println(NOTIFY_CHAR_UUID);
    Serial.print("  Large data: ");
    Serial.println(LARGE_DATA_CHAR_UUID);
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
  
  // Handle notifications
  handleNotifications();
  
  // Small delay to prevent excessive CPU usage
  delay(10);
}