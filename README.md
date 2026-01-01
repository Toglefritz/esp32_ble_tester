# ESP32 BLE Testing Device for Flutter Splendid BLE

A comprehensive BLE testing platform built for the M5 Stack ATOM Matrix development kit, designed to test and demonstrate the capabilities of the Flutter Splendid BLE plugin.

## Overview

This ESP32-based device provides multiple BLE services that implement different pairing approaches and characteristic types to thoroughly test BLE functionality in Flutter applications. The onboard LED matrix provides visual feedback for device status and operations.

## Hardware Requirements

- **M5 Stack ATOM Matrix** - ESP32-based development kit with 5x5 RGB LED matrix
- USB-C cable for programming and power
- Compatible with ESP32-PICO-D4 microcontroller

## Development Environment

This project uses PlatformIO for development and build management.

### Prerequisites
- PlatformIO Core or PlatformIO IDE
- ESP32 development framework

### Getting Started

1. Clone this repository
2. Open the project in PlatformIO
3. Build and upload to your M5 Stack ATOM Matrix
4. Monitor serial output for device status

```bash
# Build the project
pio run

# Upload to device
pio run --target upload

# Monitor serial output
pio device monitor
```

## Project Structure

```
├── src/
│   └── main.cpp          # Main firmware entry point
├── include/              # Header files
├── lib/                  # Local libraries
├── platformio.ini        # PlatformIO configuration
└── README.md             
```

## Testing with Flutter Splendid BLE

This device is specifically designed to test the Flutter Splendid BLE plugin capabilities:

- Service discovery
- Characteristic operations
- Different pairing methods
- Data transfer patterns
- Connection management

## License

This project is open source and available under the MIT License.