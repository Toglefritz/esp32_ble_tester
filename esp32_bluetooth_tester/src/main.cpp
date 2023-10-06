/**
 * @file
 * @brief Firmware for the ESP32 with a Neopixel LED.
 *
 * This firmware acts as a simple tool for testing external systems that
 * utilize Bluetooth Low Energy (BLE) communication to control IoT devices.
 * It provides two main functionalities:
 *   1. Automatically advertise the BLE capabilities of the ESP32 and manage
 *      connections and disconnections of BLE clients.
 *   2. Control the Neopixel LED connected to the ESP32 dev board through BLE.
 *      It has two characteristics:
 *        - An open characteristic to turn the LED on to a green color.
 *        - An encrypted characteristic to turn the LED on to a red color.
 *
 * The Adafruit Neopixel library is used to control the Neopixel LED, and the ESP32 BLE
 * Arduino library is used for BLE communication.
 *
 * This implementation is designed to be a practical testing tool to validate
 * and debug Bluetooth communication between a mobile application and IoT devices.
 */

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Adafruit_NeoPixel.h>

/*
  SETUP: Uncomment one of the define statements below depending on the hardware configuration for your
         ESP32 dev board. If your board is not listed, add another define statement based on the
         PIN to which the onboard or external Neopixel LED is wired.
*/

// On the Bee S3, the onboard Neopixel LED is connected to GPIO48
//#define LED 48

// On the Adafruit QT Py ESP32 C3, the Neopixel LED is connected to GPIO2
#define LED 2

// TODO add more common ESP32 dev boards

// Initalize the Neopixel library used to control the LED.
Adafruit_NeoPixel led = Adafruit_NeoPixel(1, LED, NEO_GRB + NEO_KHZ800);

/**
 * @class ServerCallbacks
 * @brief A class to handle BLE server connection and disconnection events.
 *
 * This class extends the BLEServerCallbacks class and overrides its methods
 * to provide custom behavior for when a client connects and disconnects from the
 * BLE server.
 *
 * @method onConnect
 * This method is called when a client device connects to the BLE server.
 * It can be used to perform actions such as logging the connection event or
 * triggering other functionalities on the ESP32.
 *
 * @method onDisconnect
 * This method is called when a client device disconnects from the BLE server.
 * It can be used to handle disconnection events such as logging or resetting
 * specific states on the ESP32.
 */
class ServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        Serial.println("Device connected");
    }

    void onDisconnect(BLEServer* pServer) {
        Serial.println("Device disconnected");
    }
};

/**
 * @brief Sets the value of a BLE characteristic to a specified string value.
 * 
 * @param pCharacteristic A pointer to the BLECharacteristic object to be updated.
 * @param value The string value to set for the characteristic.
 */
void setCharacteristicValue(BLECharacteristic* pCharacteristic, const std::string& value) {
    if (pCharacteristic != nullptr) { // Ensure the characteristic pointer is valid.
        pCharacteristic->setValue(value); // Set the value.
        pCharacteristic->notify(); // Notify connected clients about the change.
    }
}

/**
 * @class Callbacks
 * @brief A class to handle BLE characteristic read and write events.
 *
 * This class extends the BLECharacteristicCallbacks class and overrides its methods
 * to provide custom behavior for read and write operations on the BLE characteristics.
 *
 * @method onWrite
 * This method is called when a write request is received for the BLE characteristics.
 * It handles the following cases:
 *   - When "ON" is written to the open characteristic, it turns the green LED on.
 *   - When "ON" is written to the encrypted characteristic, it turns the red LED on.
 *   - When "OFF" is written to either characteristic, it turns both LEDs off.
 *   - When any other value is written, it logs the unexpected value to the serial console.
 *
 * The method identifies the characteristic that was written to by comparing the characteristic's
 * UUID, and then it checks the written value to determine the appropriate action to take.
 */
class Callbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        if (value == "ON") {
            if (pCharacteristic->getUUID().equals(BLEUUID("abcd1234-1234-1234-1234-1234567890ab"))) {
                led.setPixelColor(0, led.Color(0, 255, 0)); // Green LED
                led.show(); 
                Serial.println("Green LED on");
                setCharacteristicValue(pCharacteristic, "Green LED on");
            }
            else if (pCharacteristic->getUUID().equals(BLEUUID("abcd1234-1234-1234-1234-1234567890ac"))) {
                led.setPixelColor(0, led.Color(255, 0, 0)); // Red LED
                led.show(); 
                Serial.println("Red LED on");
                setCharacteristicValue(pCharacteristic, "Red LED on");
            }
        }
        else if (value == "OFF") {
            led.setPixelColor(0, led.Color(0, 0, 0)); // Off
            led.show(); 
            Serial.println("LED off");
            setCharacteristicValue(pCharacteristic, "LED off");
        } 
        else {
          Serial.print("Received unexpected value: ");
          Serial.println(value.c_str());
        }
    }
};

/**
 * @class DescriptorCallbacks
 * @brief A class to handle BLE descriptor read and write events.
 *
 * This class extends the BLEDescriptorCallbacks class and overrides its onWrite method
 * to provide custom behavior for enabling or disabling notifications on the BLE characteristics.
 *
 * @method onWrite
 * This method is called when a write request is received for the BLE2902 descriptor,
 * which is the standard descriptor for enabling or disabling notifications.
 * It handles the following cases:
 *   - When a value of 0x01 is written to the descriptor, it means the client has enabled notifications.
 *     The method will print "Notifications enabled" to the serial console.
 *   - When a value of 0x00 is written to the descriptor, it means the client has disabled notifications.
 *     The method will print "Notifications disabled" to the serial console.
 *
 * The method checks the written value to determine if notifications have been enabled or disabled.
 */
class DescriptorCallbacks : public BLEDescriptorCallbacks {
    void onWrite(BLEDescriptor* pDescriptor) {
        uint8_t* data = pDescriptor->getValue();
        if (data[0] == 0x01) { // Client has enabled notifications
            Serial.println("Notifications enabled");
        } else if (data[0] == 0x00) { // Client has disabled notifications
            Serial.println("Notifications disabled");
        }
    }
};

void setup() {
    // Initialize the LED via the Neopixel library
    led.begin();

    // Set the LED brightness
    led.setBrightness(50);

    // Initialize the LED s to 'off'
    led.show(); 

    Serial.begin(115200);

    // Create the BLE Device
    BLEDevice::init("ESP32_BLE_TESTER");

    // Configure BLE Security settings
    BLESecurity *pSecurity = new BLESecurity();
    pSecurity->setCapability(ESP_IO_CAP_NONE);
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_ONLY);

    // Create the BLE Server
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    // Create the BLE Service
    BLEService *pService = pServer->createService("abcd1234-1234-1234-1234-1234567890aa");

    // Create the open BLE characteristic
    BLECharacteristic *pOpenCharacteristic = pService->createCharacteristic(
        "abcd1234-1234-1234-1234-1234567890ab",
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY
    );

    // Set initial value for the open BLE characteristic
    pOpenCharacteristic->setValue("LED off");

    // Set callbacks on the open BLE characteristic
    pOpenCharacteristic->setCallbacks(new Callbacks());
    BLEDescriptor* pOpenNotificationDescriptor = new BLE2902();
    pOpenNotificationDescriptor->setCallbacks(new DescriptorCallbacks());
    pOpenCharacteristic->addDescriptor(pOpenNotificationDescriptor);

    // Create the encrypted BLE Characteristic
    BLECharacteristic *pEncryptedCharacteristic = pService->createCharacteristic(
        "abcd1234-1234-1234-1234-1234567890ac",
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY
    );

    // Set initial value for the encrypted BLE characteristic
    pEncryptedCharacteristic->setValue("LED off");

    // Set up security on the encrypted characteristic to utilize Just Works pairing
    pEncryptedCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);

    // Set callbacks on the encrypted characteristic
    pEncryptedCharacteristic->setCallbacks(new Callbacks());
    BLEDescriptor* pEncryptedNotificationDescriptor = new BLE2902();
    pEncryptedNotificationDescriptor->setCallbacks(new DescriptorCallbacks());
    pEncryptedCharacteristic->addDescriptor(pEncryptedNotificationDescriptor);

    // Start the service
    pService->start();

    // Start advertising
    pServer->getAdvertising()->start();
    Serial.println("BLE advertising started");
}

void loop() {
    delay(2000); // A delay to keep the loop from running wild.
}