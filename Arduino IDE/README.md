# PMSG - Prototype Modular Smart Glasses for Arduino IDE


## Project Overview
**PMSG** (Prototype Modular Smart Glasses ) is an open-source DIY project to transform ordinary glasses or goggles into customizable smart glasses.

Replace the temples (arms) with custom PCBs featuring hinge cutouts. Mount a tiny microcontroller and add modular sensors/connectors. Affordable, hackable, and ideal for wearables experiments like notifications, head tracking, monitoring, or basic AR.



## Hardware Recommendations
- Custom PCB (order from repo files via JLCPCB/Seeed Fusion)
- Glasses frame (old pair with standard hinges)
- Microcontroller:
  - **Primary: Seeed Studio XIAO ESP32C6** (WiFi 6, BLE, Zigbee/Thread support for advanced mesh/IoT)
  - LoRa variants: XIAO with nRF (e.g., RA-02 LoRa module) or ESP32-LoRa boards
  - Alternatives: XIAO RP2040, Adafruit QT Py, or other XIAO-compatible boards
- Small LiPo battery + charging module
- Optional sensors: IMU, mic, camera, OLED (via Grove connectors)

![XIAO ESP32C6](https://files.seeedstudio.com/wiki/SeeedStudio-XIAO-ESP32C6/img/xiaoc6.jpg)  
*Seeed Studio XIAO ESP32C6 - Recommended for WiFi/BLE/Zigbee*

## Arduino Control for PMSG V4 and Higher
This section covers the default peripherals and quick-start features on **PMSG Prototype Modular Smart Glasses V4 and higher**. These are designed to get you up and running fast with vibration feedback, RGB animations, sensors, and input testing.

### Onboard Peripherals & Pinout
| Function                          | Pin / Address                  | Description                                      | Library / Notes                          |
|-----------------------------------|--------------------------------|--------------------------------------------------|------------------------------------------|
| Vibration Motor                   | D10 (GPIO10)                   | Feedback motor (haptic buzz / PWM tunes)         | Use `pmsg2vibrate.h` (included) or direct PWM |
| 4x WS2812C-2020-V1 RGB LEDs       | D1 (GPIO1)                     | Programmable pixel display for animations        | `FastLED.h` or `Adafruit_NeoPixel.h` → `pmsg4pixeldisplay.h` (animations lib) |
| SHT31 (Temp + Humidity)           | I²C 0x44                       | Sensirion temperature & humidity sensor          | On PCB                                   |
| SGP41 (VOC + NOx / CO2 index)     | I²C 0x3? (usually 0x59)        | Sensirion air quality sensor                     | Not on all PCBs                          |
| VEML7700                          | I²C 0x10                       | Ambient light (lux) sensor                       | `pmsg.h` helper                          |
| LTR390                            | I²C 0x53                       | UV light sensor                                  |                                          |
| LIS2DH12                          | I²C 0x19                       | Accelerometer / movement                         | `LIS2DH12.h`                             |
| Battery Level                     | A0 (ADC)                       | Battery voltage → % power                        | Read ADC                                 |
| Vibration/Touch Input Test        | A2 (ADC)                       | Capacitive touch/vibration sense test            |                                          |
| Touch Input (ESP32-S3 only)       | D3                             | Dedicated touch pin                              |                                          |
| IR Transmitter                    | D8                             | IR remote protocol (TV, airco, etc.)             | `IRremote.h`                             |
| Push Button                       | D9 (GP1)                       | Onboard pushbutton for quick testing             |                                          |

Many demos use **D9** as a general-purpose input for quick hardware checks.

### Included Helper Libraries
- `pmsg2vibrate.h` – Easy control of the vibration motor (buzz, patterns, tones via PWM)
- `pmsg4pixeldisplay.h` – Fun animations for the 4 WS2812 LEDs (rainbow, alerts, etc.)
- `pmsgi2csensors.h` – Helpers for cool sensors like SHT, LUX, UV, Movement and more 

### Quick Setup
1. add USB C to PMSG board
2. Mount your chosen MCU (e.g., XIAO ESP32C6 ).
3. Attach to glasses 
4. Connect battery.
5. Program via Arduino IDE.

### Setup Arduino IDE for XIAO ESP32C6
- Add ESP32 board package (by Espressif).
- Select board: "XIAO_ESP32C6".
- Use USB-C for upload.

## Quick Test Examples (Arduino IDE)
These simple sketches verify your V4+ setup post-assembly.

( make sure you IDE has right boards of Xiao installed see how bellow  )
## Add this in Arduino IDE -> "settings..." -> " -> Additional boards manager URLs:
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json


### 1. Basic Blink & Serial (Hardware Check)
```cpp
// Quick Test 1: Blink LED + Serial
#define LED_PIN LED_BUILTIN

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("PMSG V4+ with ESP32C6 Booted!");
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  Serial.println("LED ON");
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  Serial.println("LED OFF");
  delay(1000);
}