#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BeeS3.h>

// Pull the BEES3 class from the helper and name it bees3 so we can use it below.
BEES3 bees3;

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
                bees3.setPixelColor(green); // Green LED
                Serial.println("Green LED ON");
            }
            else if (pCharacteristic->getUUID().equals(BLEUUID("abcd1234-1234-1234-1234-1234567890ac"))) {
                bees3.setPixelColor(red); // Red LED
                Serial.println("Red LED ON");
            }
        }
        else if (value == "OFF") {
            bees3.setPixelColor(off);
            Serial.println("LED OFF");
        } 
        else {
          Serial.print("Received unexpected value: ");
          Serial.println(value.c_str());
        }
    }
};

void setup() {
    // Initalize the helper library.
    bees3.begin();

    // Brightness is 0-255. We set it to 1/2 brightness here.
    bees3.setPixelBrightness(255 / 2);

    Serial.begin(115200);

    // Create the BLE Device
    BLEDevice::init("ESP32_BLE_TESTER");

    // Create the BLE Server
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    // Create the BLE Service
    BLEService *pService = pServer->createService("abcd1234-1234-1234-1234-1234567890aa");

    // Create the open BLE Characteristic
    BLECharacteristic *pOpenCharacteristic = pService->createCharacteristic(
        "abcd1234-1234-1234-1234-1234567890ab",
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE
    );
    pOpenCharacteristic->setCallbacks(new Callbacks());
    pOpenCharacteristic->addDescriptor(new BLE2902());

    // Create the encrypted BLE Characteristic
    BLECharacteristic *pEncryptedCharacteristic = pService->createCharacteristic(
        "abcd1234-1234-1234-1234-1234567890ac",
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE
    );
    pEncryptedCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
    pEncryptedCharacteristic->setCallbacks(new Callbacks());
    pEncryptedCharacteristic->addDescriptor(new BLE2902());

    // Start the service
    pService->start();

    // Start advertising
    pServer->getAdvertising()->start();
    Serial.println("BLE advertising started");
}

void loop() {
    delay(2000); // A delay to keep the loop from running wild.
}