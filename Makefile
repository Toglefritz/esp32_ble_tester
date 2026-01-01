# ESP32 BLE Tester Firmware Makefile
# Provides convenient commands for PlatformIO operations

# Default target
.DEFAULT_GOAL := help

# PlatformIO executable (can be overridden)
PIO ?= pio

# Check if PlatformIO is installed
check-pio:
	@which $(PIO) > /dev/null || (echo "Error: PlatformIO not found. Install with: pip install platformio" && exit 1)

# Build the firmware
build: check-pio
	@echo "Building ESP32 BLE Tester firmware..."
	$(PIO) run

# Clean build artifacts
clean: check-pio
	@echo "Cleaning build artifacts..."
	$(PIO) run --target clean

# Upload firmware to connected ESP32
upload: check-pio
	@echo "Uploading firmware to ESP32..."
	$(PIO) run --target upload

# Start serial monitor
monitor: check-pio
	@echo "Starting serial monitor (Ctrl+C to exit)..."
	$(PIO) device monitor

# Flash firmware and start monitor (most common workflow)
flash: check-pio
	@echo "Flashing firmware and starting monitor..."
	$(PIO) run --target upload --target monitor

# List connected devices
devices: check-pio
	@echo "Connected devices:"
	$(PIO) device list

# Show project information
info: check-pio
	@echo "Project information:"
	$(PIO) project config

# Update PlatformIO libraries
update: check-pio
	@echo "Updating libraries..."
	$(PIO) lib update

# Install project dependencies
install: check-pio
	@echo "Installing dependencies..."
	$(PIO) lib install

# Show help
help:
	@echo "ESP32 BLE Tester Firmware - Available commands:"
	@echo ""
	@echo "  make build    - Build the firmware"
	@echo "  make clean    - Clean build artifacts"
	@echo "  make upload   - Upload firmware to ESP32"
	@echo "  make monitor  - Start serial monitor"
	@echo "  make flash    - Upload firmware and start monitor (recommended)"
	@echo "  make devices  - List connected devices"
	@echo "  make info     - Show project information"
	@echo "  make update   - Update libraries"
	@echo "  make install  - Install dependencies"
	@echo "  make help     - Show this help message"
	@echo ""
	@echo "Prerequisites:"
	@echo "  - PlatformIO installed (pip install platformio)"
	@echo "  - ESP32 connected via USB"
	@echo ""
	@echo "Most common usage:"
	@echo "  make flash    # Build, upload, and monitor in one command"

# Declare phony targets
.PHONY: check-pio build clean upload monitor flash devices info update install help