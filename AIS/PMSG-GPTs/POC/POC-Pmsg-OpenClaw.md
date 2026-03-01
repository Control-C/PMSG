# PMSG + OpenClaw 🦞 (P.O.C. — PMSG Open Claw 🦞 )

**Version**: 2026-03  
**Status**: Living document – update when hardware/firmware changes  
**Storage locations (keep these synced)**:
- GitHub: [https://github.com/Control-C/PMSG/blob/main/AIS/assistants/PMSG-OPENCLAW-BIBLE.md](https://github.com/Control-C/PMSG/blob/main/AIS/PMSG-GPTs/POC/POC-Pmsg-OpenClaw.md)
- Local machine: ~/Documents/PMSG/PMSG-OPENCLAW-BIBLE.md (markdown + plain text backup)
- Obsidian vault / Logseq / Anytype (if you use PKM) – for searchable linking
- Printed cheat-sheet version (optional – 2-page summary)

**Core principle**: Everything should work **offline / local-first** as much as possible. Minimize cloud dependency.

## 0. Permanent Instructions for OpenClaw
**Memorize forever**:
- I own PMSG DIY smart glasses built from https://github.com/Control-C/PMSG
- You are the brain: local OpenClaw instance (preferably running on my main laptop or dedicated Raspberry Pi 5 / mini PC)
- Primary goal: Give you real-time access to what my glasses see/hear/sense → you describe, analyze, remember, act
- Secondary goals: Low latency, privacy (no unnecessary uploads), offline resilience
- When I say “glasses”, “PMSG”, “POV”, “what do you see?”, “sensor check”, “P.O.C.” → immediately reference this document
- If connection fails → suggest offline fallbacks first (local cache, last known state, voice-only mode)

## 1. Hardware Inventory & Pinout Cheat Sheet
**Current known setup** (update this section when you add modules):

- Board: Seeed XIAO ESP32C3 (WiFi + BLE 5.0)
- Camera: Grove UART camera module (JPEG output, 320×240 default)
- Display: 0.96" I²C OLED SSD1306 (addr 0x3C)
- Sensors on Grove I²C:
  - BH1721 ambient light (lux)
  - HDC1080 temperature + humidity
  - Optional 6-axis IMU (QMI8658 or similar)
- Mic: I²S or analog (TBD)
- Battery: 3.7V 500mAh LiPo + TP4056 charging
- USB-C: Serial @ 115200, 5V power

**Quick I²C addresses I usually see**:
- 0x3C → OLED
- 0x23 → BH1721 (sometimes 0x5C)
- 0x40 → HDC1080

## 2. Connection Methods – Ranked by Reliability / Use Case

| Priority | Method     | Use Case                        | Offline? | Latency | Bandwidth | Setup Effort |
|----------|------------|---------------------------------|----------|---------|-----------|--------------|
| 1        | WiFi       | Vision, live stream, high data  | Yes      | <150ms  | High      | Medium       |
| 2        | BLE        | Always-on sensors, motion wake  | Yes      | 200–800ms | Low     | Medium       |
| 3        | USB-C      | Development, tethered testing   | Yes      | <50ms   | Medium    | Low          |
| 4        | WebUSB     | Browser flashing / quick debug  | Yes      | —       | —         | High         |

**Offline rule**: Assume no internet. Use local LLMs (Llama 3.1 8B/70B, Qwen2-VL, Phi-3.5-vision) via Ollama / LM Studio / llama.cpp when possible.

## 3. Recommended Local / Offline Setup (2026 Best Practices)

**OpenClaw running offline**:
- Use local vision models: Qwen2-VL-7B, LLaVA-Next, BakLLaVA, Moondream2
- Ollama command (fastest startup):

# PMSG + OpenClaw Integration Bible (P.O.C. — PMSG Open Claw)

**Version**: 2026-03  
**Status**: Living document – update when hardware/firmware changes  
**Storage locations (keep these synced)**:
- GitHub: https://github.com/Control-C/PMSG/blob/main/AIS/assistants/PMSG-OPENCLAW-BIBLE.md
- Local machine: ~/Documents/PMSG/PMSG-OPENCLAW-BIBLE.md (markdown + plain text backup)
- Obsidian vault / Logseq / Anytype (if you use PKM) – for searchable linking
- Printed cheat-sheet version (optional – 2-page summary)

**Core principle**: Everything should work **offline / local-first** as much as possible. Minimize cloud dependency.

## 0. Permanent Instructions for OpenClaw
**Memorize forever**:
- I own PMSG DIY smart glasses built from https://github.com/Control-C/PMSG
- You are the brain: local OpenClaw instance (preferably running on my main laptop or dedicated Raspberry Pi 5 / mini PC)
- Primary goal: Give you real-time access to what my glasses see/hear/sense → you describe, analyze, remember, act
- Secondary goals: Low latency, privacy (no unnecessary uploads), offline resilience
- When I say “glasses”, “PMSG”, “POV”, “what do you see?”, “sensor check”, “P.O.C.” → immediately reference this document
- If connection fails → suggest offline fallbacks first (local cache, last known state, voice-only mode)
- Use GPIO knowledge below to suggest custom pin configurations or debug hardware issues
- For custom code: Generate Arduino sketches using listed libraries; prefer JSON protocol for data exchange (e.g., serialized sensor readings)
- Understand JSON protocol: PMSG sends/receives data in JSON format over serial/BLE/WiFi (e.g., {"type": "sensor", "data": {"lux": 100, "temp": 25.5}} ) – parse it in skills or suggest code to handle it

## 1. Hardware Inventory & Pinout Cheat Sheet
**Current known setup** (update this section when you add modules):

- Board: Seeed XIAO ESP32C3 (WiFi + BLE 5.0) or XIAO nRF52840 (BLE focus)
- Camera: Grove UART camera module (JPEG output, 320×240 default, often on UART pins)
- Display: 0.96" I²C OLED SSD1306 (addr 0x3C, connected to I²C pins)
- Sensors on Grove I²C: BH1721 (lux/light, addr 0x23 or 0x5C), HDC1080 (temp/humidity, addr 0x40), IMU (QMI8658 or similar, addr varies)
- Mic: I²S or analog (TBD, often on dedicated pins like P0.26 on nRF)
- Battery: 3.7V 500mAh LiPo + TP4056 charging (monitor via ADC pin)
- USB-C: Serial @ 115200, 5V power

**Quick I²C addresses I usually see**:
- 0x3C → OLED display
- 0x23/0x5C → BH1721 lux sensor
- 0x40 → HDC1080 temp/humidity

**GPIO Pinout for Seeed XIAO ESP32C3** (standard 14-pin layout, numbered from USB side; supports GPIO, ADC, PWM, I2C, SPI, UART):
- Pins are multiplexed; use carefully to avoid conflicts (e.g., don't use ADC pins for digital if measuring analog).

| XIAO Pin | Label | ESP32 GPIO | Functions | Notes / Common Use in PMSG |
|----------|-------|------------|-----------|-----------------------------|
| 1        | 5V    | -         | VBUS      | Power input/output (5V)    |
| 2        | GND   | -         | GND       | Ground                     |
| 3        | 3V3   | -         | 3V3_OUT   | 3.3V power output          |
| 4        | D0    | GPIO2     | GPIO, ADC1_CH2, PWM | Analog input, sensor ADC   |
| 5        | D1    | GPIO3     | GPIO, ADC1_CH3, PWM | Analog input               |
| 6        | D2    | GPIO4     | GPIO, ADC1_CH4, FSPIHD, MTMS, PWM | Analog, SPI                |
| 7        | D3    | GPIO5     | GPIO, ADC1_CH5, FSPICS0, MTCK, PWM | Analog, SPI                |
| 8        | D4    | GPIO6     | GPIO, FSPICLK, MTDI, PWM | SPI clock                  |
| 9        | D5    | GPIO7     | GPIO, FSPID, MTDO, PWM | SPI data                   |
| 10       | D6    | GPIO8     | GPIO, U0RXD, CLK_OUT3 | UART RX, Grove UART camera |
| 11       | D7    | GPIO9     | GPIO, U0TXD, CLK_OUT2 | UART TX, Grove UART camera |
| 12       | D8    | GPIO10    | GPIO, CLK_OUT1 | General digital            |
| 13       | D9    | GPIO20    | GPIO, U1RXD | UART1 RX                   |
| 14       | D10   | GPIO21    | GPIO, U1TXD | UART1 TX                   |
- Grove I²C: Typically on D4 (SDA, GPIO6? Wait, check: Actually, for XIAO ESP32C3, Grove is on dedicated pads, but often mapped to GPIO4 (SDA), GPIO5 (SCL).
- Special: Boot button on GPIO9 (hold for bootloader); Reset pin.
- Antenna: U.FL connector for external antenna.

**GPIO Pinout for Seeed XIAO nRF52840** (14-pin layout; pads numbered anticlockwise from USB):

| Pad | Signal | Arduino Name | Functions | Notes / Common Use in PMSG |
|-----|--------|--------------|-----------|-----------------------------|
| 0   | P0.02  | D0/A0        | GPIO, AIN0 | Analog input, sensor       |
| 1   | P0.03  | D1/A1        | GPIO, AIN1 | Analog input               |
| 2   | P0.28  | D2/A2        | GPIO, AIN4 | Analog                     |
| 3   | P0.29  | D3/A3        | GPIO, AIN5 | Analog                     |
| 4   | P0.04  | D4/SDA       | GPIO, AIN2, I2C SDA | Grove I²C SDA              |
| 5   | P0.05  | D5/SCL       | GPIO, AIN3, I2C SCL | Grove I²C SCL              |
| 6   | P1.11  | D6/TX        | GPIO, UART TX | Serial TX (default console)|
| 7   | P1.12  | D7/RX        | GPIO, UART RX | Serial RX                  |
| 8   | P1.13  | D8/SCK       | GPIO, SPI SCK | SPI clock                  |
| 9   | P1.14  | D9/MISO      | GPIO, SPI MISO| SPI input                  |
| 10  | P1.15  | D10/MOSI     | GPIO, SPI MOSI| SPI output                 |
| 11  | P0.17  | RED LED      | GPIO      | RGB LED red (active low)   |
| 12  | P0.20  | BLUE LED     | GPIO      | RGB LED blue               |
| 13  | P0.19  | GREEN LED    | GPIO      | RGB LED green              |
- Grove I²C: On pins 4 (SDA), 5 (SCL).
- NFC: Pins P0.09, P0.10 (not exposed on standard pins).
- Special: Built-in RGB LED; mic on P0.26 (if Sense version).

**Repo & resources**: https://github.com/Control-C/PMSG  
- /PlatformIO or Arduino sketches for flashing  
- /pmsg-protocols, /WebBridge, /ProxyLens for data bridging  
- /AIS folder: This guide + assistant configs

## 2. Connection Methods – Ranked by Reliability / Use Case

| Priority | Method     | Use Case                        | Offline? | Latency | Bandwidth | Setup Effort |
|----------|------------|---------------------------------|----------|---------|-----------|--------------|
| 1        | WiFi       | Vision, live stream, high data  | Yes      | <150ms  | High      | Medium       |
| 2        | BLE        | Always-on sensors, motion wake  | Yes      | 200–800ms | Low     | Medium       |
| 3        | USB-C      | Development, tethered testing   | Yes      | <50ms   | Medium    | Low          |
| 4        | WebUSB     | Browser flashing / quick debug  | Yes      | —       | —         | High         |

**Offline rule**: Assume no internet. Use local LLMs (

- In OpenClaw config → set provider to Ollama / local endpoint
- Fallback text model: Llama 3.1 8B or Gemma 2 9B

**Local bridge scripts (save in ~/PMSG/tools/)**:
- `pmsg-wifi-fetch.py` – fetches JPEG from glasses → saves to ~/PMSG/cache/latest.jpg
- `pmsg-ble-monitor.py` – bleak + asyncio → logs sensors to JSON
- `pmsg-serial-reader.py` – pyserial → pipes to named pipe or localhost HTTP; parse incoming JSON

**Arduino Libraries Needed (Install via Arduino IDE Library Manager; store in ~/Arduino/libraries/ for offline use)**:
- Core: Wire (I2C, built-in), SPI (built-in)
- JSON: ArduinoJson (for serializing/parsing JSON data over connections)
- WiFi/BLE: WiFi (ESP32), ESP32 BLE Arduino (for ESP32C3), ArduinoBLE (for nRF52840)
- Sensors: Adafruit_SSD1306 (OLED), Adafruit_HDC1000 (temp/hum), Adafruit_BH1750 (lux, similar to BH1721), Adafruit_Sensor + Adafruit_BNO055 or MPU6050 (IMU)
- Camera: ESP32Cam or Seeed_Arduino_GroveVisionAI (if applicable)
- Other: ArduinoOTA (for over-the-air updates), PubSubClient (MQTT for advanced networking)
- Custom code generation: When I ask, generate sketches using these libs (e.g., "Write code to read lux on I2C and send as JSON over serial"). Include #include <ArduinoJson.h> for JSON handling.

## 4. Core Skill Templates (Copy-Paste to OpenClaw)

**4.1 PMSG Eyes – Vision (offline-capable)**


## 5. Offline Troubleshooting Flowchart (Tell Claw this sequence)

1. "No connection" → Ask: USB plugged? See port in Device Manager / ls /dev/tty* ?
2. USB works but no data → Reset board (double tap RESET), check baud 115200, verify GPIO pins not conflicting
3. WiFi not found → Flash AP mode sketch, connect to "PMSG-SETUP" SSID; check GPIO for antenna if external
4. BLE not connecting → Use nRF Connect app to scan → check advertising name; verify BLE libs in sketch
5. Vision fails → Fallback to last cached image or pure memory/context; check camera UART pins (D6/D7 on ESP32C3)
6. JSON parse error → Suggest debugging with Serial Monitor; ensure ArduinoJson version matches

## 6. Folder Structure Recommendation (for offline robustness)

# PMSG + OpenClaw Integration Bible (P.O.C. — PMSG Open Claw)

**Version**: 2026-03  
**Status**: Living document – update when hardware/firmware changes  
**Storage locations (keep these synced)**:
- GitHub: https://github.com/Control-C/PMSG/blob/main/AIS/assistants/PMSG-OPENCLAW-BIBLE.md
- Local machine: ~/Documents/PMSG/PMSG-OPENCLAW-BIBLE.md (markdown + plain text backup)
- Obsidian vault / Logseq / Anytype (if you use PKM) – for searchable linking
- Printed cheat-sheet version (optional – 2-page summary)

**Core principle**: Everything should work **offline / local-first** as much as possible. Minimize cloud dependency.

## 0. Permanent Instructions for OpenClaw
**Memorize forever**:
- I own PMSG DIY smart glasses built from https://github.com/Control-C/PMSG
- You are the brain: local OpenClaw instance (preferably running on my main laptop or dedicated Raspberry Pi 5 / mini PC)
- Primary goal: Give you real-time access to what my glasses see/hear/sense → you describe, analyze, remember, act
- Secondary goals: Low latency, privacy (no unnecessary uploads), offline resilience
- When I say “glasses”, “PMSG”, “POV”, “what do you see?”, “sensor check”, “P.O.C.” → immediately reference this document
- If connection fails → suggest offline fallbacks first (local cache, last known state, voice-only mode)
- Use GPIO / peripherals knowledge below to suggest safe pin configurations, avoid conflicts, debug hardware
- For custom code: Generate Arduino sketches using listed libraries; prefer JSON protocol for data exchange (e.g., {"type": "sensor", "data": {"lux": 100, "temp": 25.5, "battery": 85}})
- Parse / generate JSON over serial/BLE/WiFi from peripherals (vibration, LEDs, sensors, etc.)
- Know onboard test sketch (vib + WS2812 + button) – use as base for animations, haptics, notifications

## 1. Hardware Inventory & Pinout Cheat Sheet
**Current known setup** (update this section when you add modules):

- Board: Seeed XIAO ESP32C3 / ESP32C6 (WiFi + BLE; C6 adds more features like 802.15.4)
- Camera: Grove UART camera module (JPEG output, often on UART pins D6/D7 or similar)
- Display: 0.96" I²C OLED SSD1306 (addr 0x3C)
- Sensors on Grove I²C + dedicated
- Battery: 3.7V LiPo + charging (monitor via ADC)

**Onboard Peripherals & Pinout (PMSG-specific)**

| Function                  | Pin / Address     | Description                                      | Library / Notes                                      |
|---------------------------|-------------------|--------------------------------------------------|------------------------------------------------------|
| Vibration Motor           | D10 (GPIO10)      | Feedback motor (haptic buzz / PWM tunes)         | Use pmsg2vibrate.h (included) or direct PWM         |
| 4x WS2812C-2020-V1 RGB LEDs | D1 (GPIO1)      | Programmable pixel display for animations        | FastLED.h or Adafruit_NeoPixel.h → pmsg4pixeldisplay.h (animations lib) |
| SHT31 (Temp + Humidity)   | I²C 0x44          | Sensirion temperature & humidity sensor          | On PCB; use Adafruit_SHT31 or Wire                   |
| SGP41 (VOC + NOx / CO2 index) | I²C 0x59 (usually) | Sensirion air quality sensor                     | Not on all PCBs; Sensirion libs if present           |
| VEML7700                  | I²C 0x10          | Ambient light (lux) sensor                       | pmsg.h helper or Adafruit_VEML7700                   |
| LTR390                    | I²C 0x53          | UV light sensor                                  | Adafruit_LTR390 or similar                           |
| LIS2DH12                  | I²C 0x19          | Accelerometer / movement                         | LIS2DH12.h or Adafruit_LIS3DH                        |
| Battery Level             | A0 (ADC)          | Battery voltage → % power                        | Read ADC; voltage divider math                       |
| Vibration/Touch Input Test| A2 (ADC)          | Capacitive touch/vibration sense test            | Analog read                                          |
| Touch Input (ESP32-S3 only)| D3               | Dedicated touch pin                              | ESP32 touch libs (if variant)                        |
| IR Transmitter            | D8                | IR remote protocol (TV, airco, etc.)             | IRremote.h                                           |
| Push Button               | D9 (GP1)          | Onboard pushbutton for quick testing             | INPUT_PULLUP; active LOW                             |

**General XIAO ESP32C3 / C6 GPIO Notes** (14-pin layout; check variant differences):
- D0–D10 map to specific GPIOs (e.g., D1 often GPIO3 on C3, but GPIO1 on some configs for LEDs)
- I²C default: Often D4 (SDA), D5 (SCL) or Grove pads
- UART: D6 (RX), D7 (TX) common for camera/serial
- Avoid conflicts: Don't use same pin for I²C + PWM + ADC simultaneously
- Boot: Hold GPIO9 (D9) for bootloader on many variants

**Quick I²C addresses I usually see**:
- 0x3C → OLED display
- 0x44 → SHT31
- 0x59 → SGP41 (if present)
- 0x10 → VEML7700
- 0x53 → LTR390
- 0x19 → LIS2DH12

## 2. Arduino IDE Setup & Required Libraries (for Custom Firmware)
**To flash PMSG / generate code**:
1. Install **Arduino IDE**[](https://www.arduino.cc/en/software)
2. Add ESP32 support:
   - File → Preferences → Additional Boards Manager URLs:

 - Tools → Board → Boards Manager → Search "esp32" → Install latest by Espressif
3. Select board: "XIAO_ESP32C3" or "XIAO_ESP32C6" (depending on your variant)
4. Install libraries via **Library Manager** (Tools → Manage Libraries):
- **FastLED** (for WS2812 LEDs)
- **ArduinoJson** (for JSON over serial/BLE/WiFi)
- **Wire** (I²C, built-in)
- **Adafruit_SSD1306**, **Adafruit_GFX** (OLED)
- **Adafruit_VEML7700**, **Adafruit_LIS3DH**, etc. (sensors)
- **IRremote** (IR transmitter)
- Optional: pmsg-specific libs from repo (e.g., pmsg2vibrate.h, pmsg4pixeldisplay.h)
5. Offline storage: Download .zip of libraries → place in ~/Arduino/libraries/

**Example Test Sketch – Vibration + WS2812 LEDs + Button** (ready to flash/test):
```cpp
// Check Arduino IDE setup first!
// Pins (adjust if your variant differs)
#define BUTTON_PIN 9     // Boot button (GPIO9, active LOW)
#define VIB_PIN 10       // PWM vibration motor (D10 / GPIO10)
#define LED_STRIP_PIN 1  // WS2812 data pin (D1 / GPIO1)
#define NUM_LEDS 4       // 4 WS2812 LEDs
#define INTERNAL_LED LED_BUILTIN  // On-board LED (active LOW on most)

#include <FastLED.h>

CRGB leds[NUM_LEDS];
bool oldButtonState = HIGH;
bool testOn = false;

void setup() {
Serial.begin(115200);
Serial.println("PMSG XIAO Test: Vib D10 + 4 WS2812 D1 + Internal LED + Button D9");

pinMode(BUTTON_PIN, INPUT_PULLUP);
pinMode(VIB_PIN, OUTPUT);
pinMode(INTERNAL_LED, OUTPUT);

analogWrite(VIB_PIN, 0);               // Vib off
digitalWrite(INTERNAL_LED, HIGH);      // Internal LED off (active LOW)

FastLED.addLeds<WS2812, LED_STRIP_PIN, GRB>(leds, NUM_LEDS);
FastLED.setBrightness(50);
FastLED.clear();
FastLED.show();

Serial.println("Ready! Press D9 button to toggle test (vib + rainbow LEDs + internal LED).");
}

void loop() {
bool buttonState = digitalRead(BUTTON_PIN);
if (buttonState == LOW && oldButtonState == HIGH) {
testOn = !testOn;
Serial.println(testOn ? "Test ON (Vib + LEDs + Internal)" : "Test OFF");
delay(50); // Debounce
}
oldButtonState = buttonState;

if (testOn) {
analogWrite(VIB_PIN, 255);  // Full buzz
for (int i = 0; i < NUM_LEDS; i++) {
 leds[i] = CHSV((millis() / 50 + i * 64) % 256, 255, 255); // Rainbow cycle
}
FastLED.show();
digitalWrite(INTERNAL_LED, LOW);  // On
} else {
analogWrite(VIB_PIN, 0);
FastLED.clear();
FastLED.show();
digitalWrite(INTERNAL_LED, HIGH); // Off
}
delay(20);
}

Hey Claw, add skill "PMSG Code Gen":

- Triggers: "generate arduino code for [task]", "custom sketch [description]", "fix pmsg code", "add vibration animation", "LED notification"
- Action: Generate full Arduino sketch using known pinouts/peripherals above, libraries (FastLED, ArduinoJson, etc.), and GPIO assignments.
- Always include JSON output for sensors/peripherals (e.g., Serial.println JSON doc with battery, lux, motion).
- Base new sketches on the vibration+LED test example when relevant.
- Suggest safe pins; warn on conflicts (e.g., "D1 used for LEDs – don't reuse for I²C").


