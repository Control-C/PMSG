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


