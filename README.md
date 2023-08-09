# ESP32 Bluetooth Tester

## Overview
This project provides firmware for the ESP32 equipped with a Neopixel LED. It serves as a simple tool to test external systems that utilize Bluetooth Low Energy (BLE) communication to control IoT devices. The sketch offers a BLE interface to control the Neopixel LED by using two different characteristics.

## Features
- Automatic BLE advertisement on startup
- Control of a Neopixel LED connected to the dev board through BLE characteristics
- Open characteristic for green LED control
- Encrypted characteristic for red LED control

## Libraries Used
- ESP32 BLE Arduino Library
- Adafruit Neopixel Library

## Getting Started
### Prerequisites
- PlatformIO installed in Visual Studio Code
- Development board with an ESP32 chip and either an onboard Neopixel LED or an external Neopixel LED connected to a GPIO pin
- USB cable to connect the development board to your computer

### Upload Instructions
1. Open Project in VS Code: Open Visual Studio Code, and then open the folder containing the project.
2. Configure PlatformIO: Make sure the PlatformIO extension is installed in VS Code. You may need to set the correct board and COM port settings in platformio.ini:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
upload_port = YOUR_COM_PORT
```

3. Connect ESP32 Board: Connect the development board to your computer using the USB cable.
4. Build the Sketch: Click the "Build" button (a checkmark icon) at the bottom of the VS Code window.
5. Upload the Sketch: Click the "Upload" button (a right arrow icon) at the bottom of the VS Code window. This will compile the sketch and upload it to the connected ESP32 device.
6. Monitor Serial Output: Click the "Serial Monitor" button (a plug icon) at the bottom of the VS Code window to open the serial monitor and view the log output from the ESP32.

## Support & Contribution
For questions, support, or contributions, please reach out through the project's repository or open an issue.

## License
This project is open-source and available under the MIT License. See the LICENSE file for details.