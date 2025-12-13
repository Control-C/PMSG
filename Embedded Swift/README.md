By seeed studio see https://wiki.seeedstudio.com/xiao-esp32-swift/
 
# Makefile for PMSG Smart Glasses using Embedded Swift on Seeed Studio XIAO ESP32C6
# This Makefile assumes:
# - ESP-IDF v5.2.1 is installed and sourced (source ~/esp-idf/export.sh or use 'get_idf' alias)
# - Swift 6.0 toolchain is installed and in PATH
# - The project structure follows ESP-IDF with Swift integration in main/ (e.g., Swift sources in main/, CMakeLists.txt configured for swiftc)
# - Ported PMSG logic from Arduino to Embedded Swift (e.g., display handling via U8g2 or similar, WiFi for connectivity)
# - Run 'make setup' first if needed, then 'make all' to build

# Project name
PROJECT_NAME := pmsg-smart-glasses

# IDF_PATH should be set in environment
ifndef IDF_PATH
$(error Please set IDF_PATH to your ESP-IDF installation directory)
endif

# Include ESP-IDF make rules
include $(IDF_PATH)/make/project.mk

# Custom targets for convenience
.PHONY: setup all clean flash monitor set-target

setup:
	@echo "Setting up project for XIAO ESP32C6 with Embedded Swift..."
	idf.py set-target esp32c6
	@echo "Ensure Swift sources are in main/ and CMakeLists.txt includes: target_link_libraries(${COMPONENT_LIB} \"-lswiftCore -lswift_Concurrency\")"
	@echo "For PMSG features: Integrate display (e.g., SSD1306 OLED via U8g2 Swift wrapper), sensors, WiFi BLE."

all: set-target
	idf.py build

clean:
	idf.py fullclean

flash: set-target
	idf.py -p /dev/ttyACM0 flash  # Adjust port as needed (e.g., /dev/cu.usbmodem* on macOS)

monitor: set-target
	idf.py -p /dev/ttyACM0 monitor

set-target:
	idf.py set-target esp32c6

# GitHub CI/CD example target (assumes actions runner has IDF and Swift setup)
ci-build:
	idf.py set-target esp32c6
	idf.py build
	@echo "Build successful for PMSG Smart Glasses on ESP32C6"

# Flash with monitor for development
run: flash monitor