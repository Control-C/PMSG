# PMSG AI Assistant Setup Guide

PMSG = **P.M. Smart Glasses** — open face computer / wearable platform using Seeed Studio XIAO boards.

Repo: https://github.com/Control-C/PMSG

Goal: Set up PMSG hardware to feed data (camera, sensors, mic, IMU) to AI assistants like OpenClaw[](https://openclaw.ai), Claude, Gemini, local LLMs, etc. via BLE, WiFi, or USB-C.

# ESP-CLAW 
Or if a ESP is used go for https://esp-claw.com/en/ 

## Hardware Assumptions (PMSG variants)
- Seeed XIAO nRF52840 — BLE focus
- Seeed XIAO ESP32S3 — WiFi + camera support
- Camera module (OV2640/OV5640 etc.)
- Display (OLED/SHARP Memory), mic, IMU, battery

## Step 1: Flash Base Firmware
Use the AI-Guided Setup prompts in the main README or Arduino/PlatformIO from /firmware folder.

Example: Flash BLE camera broadcaster or WiFi stream server.

## Step 2: Choose Data Connection Method

### BLE (Low Power, Always-On)
- Best for sensor data + low-fps images.
- GATT service with notify characteristics for camera JPEG, IMU, battery.
- Client (phone/PC): Use nRF Connect, Python bleak, or custom app.
- Bridge to AI: Script reads BLE → sends base64 image/text to OpenClaw skill or LLM API.

### WiFi (High Bandwidth)
- ESP32-S3 variant shines here.
- Run HTTP server / WebSocket / RTSP for camera stream.
- Example code: ESP32-CAM style MJPEG server.
- AI side: OpenCV/Python fetches frames → multimodal prompt ("Describe this scene").

### USB-C (Tethered / Dev Mode)
- XIAO USB appears as Serial (CDC) or Composite.
- Send data over serial: `Serial.println("imu:0.1,0.2,9.8");` or base64 images.
- PC: Python `pyserial` reads → forwards to assistant.
- Bonus: Use for initial config / OTA updates.

## Step 3: Integrate with AI Assistant (e.g. OpenClaw)
1. Install OpenClaw (see above).
2. Create custom skill:
   - Poll BLE/WiFi/USB.
   - On trigger (voice "Hey, what do you see?"), grab frame/sensor data.
   - Use OpenClaw tool calling → send to vision model.
3. Alternative: Use local proxy script that exposes REST/WebSocket endpoint → OpenClaw calls it.

## Tips & Troubleshooting
- Power: Use small LiPo + charging via USB-C.
- Latency: BLE ~100-500ms, WiFi <100ms for low-res.
- Privacy: Keep everything local — OpenClaw runs on-device.
- Community: Check PMSG issues, OpenClaw Discord.

Start simple: Get BLE advertising working → read battery level in nRF Connect → expand to camera.


# PMSG AI Assistant Setup Guide

PMSG = **P.M. Smart Glasses** — open face computer / wearable platform using Seeed Studio XIAO boards.

Repo: https://github.com/Control-C/PMSG

Goal: Set up PMSG hardware to feed data (camera, sensors, mic, IMU) to AI assistants like OpenClaw[](https://openclaw.ai), Claude, Gemini, local LLMs, etc. via BLE, WiFi, or USB-C.

## Hardware Assumptions (PMSG variants)
- Seeed XIAO ESP32C3 (primary; WiFi + BLE capable) or XIAO nRF52840 (BLE focus)
- Camera module (e.g., via Grove UART or I²C)
- Display (OLED/SHARP Memory via Grove I²C), mic, IMU, battery
- Grove I²C port for sensors (e.g., BH1721 lux, HDC1080 temp/humidity)
- USB-C for serial (115200 baud), power, and OTG (host mode tested on Android)

## Step 0: Connect & Detect the PMSG on Your Computer (Troubleshooting Guide)
Start simple—most issues are connection-related. Work through these steps in order. Report back what you see at each stage (e.g., "no port appeared", "serial monitor shows garbage", "I2C scan found 0x3C").

### 1. USB-C Direct Connection (Simplest & Most Reliable)
- Plug PMSG into computer via USB-C data cable (not charge-only).
- Check if it appears as a serial device:
  - **Windows**: Device Manager → Ports (COM & LPT) → Look for "USB Serial Device" or "Silicon Labs CP210x" / "CH340" / "USB JTAG/serial" (COMx port).
  - **macOS**: Open Terminal → `ls /dev/cu.*` or `ls /dev/tty.*` → Look for `/dev/cu.usbserial-*` or `/dev/ttyACM0`.
  - **Linux**: `ls /dev/tty*` → Look for `/dev/ttyUSB*` or `/dev/ttyACM*`; may need `sudo usermod -a -G dialout $USER` and reboot.
- If detected → Open serial monitor:
  - Arduino IDE: Tools → Serial Monitor → Select port + 115200 baud.
  - Or use `screen /dev/cu.usbserial-xxx 115200` (macOS) or `minicom -D /dev/ttyUSB0 -b 115200` (Linux).
  - Reset board (double-press reset button if needed) → Look for boot logs, sensor data, or "ready" messages.
- If **no port appears**:
  - Try different USB-C cable/port (data-capable required).
  - Hold BOOT button while plugging in (for ESP32 variants) to force bootloader mode.
  - Try on another computer/OS.

### 2. Fallback: Browser-Based (WebUSB / WebSerial) if USB Fails
PMSG does not have official WebUSB/WebSerial flashing (no .html/.js in repo for browser upload), but XIAO boards sometimes work with generic tools.
- Try https://espressif.github.io/esptool-js/ (for ESP32C3) or https://dfu.tizen.org/webdfu (generic DFU).
- Or Arduino Web Editor (cloud.arduino.cc) → Add board support → Try upload via browser.
- If it detects the device → Great! Use for flashing simple test sketches.
- If not → Skip to wired IDE methods below (most reliable).

### 3. Network/WiFi Discovery (If Board Already Has WiFi Firmware)
- If previous firmware enabled WiFi AP or station mode:
  - Scan WiFi networks on phone/laptop → Look for SSID like "PMSG-XXXX" or "ESP32-AP".
  - Connect → Open 192.168.4.1 (common captive portal) or check serial for IP.
- Use tools to scan:
  - nmap: `nmap -sn 192.168.1.0/24` (your local subnet).
  - Angry IP Scanner or Fing app.
- If found → Access via HTTP/WebSocket for data (e.g., /stream for camera if implemented).

### 4. Download & Install IDE for Flashing / Sensor Testing
To flash new firmware or read sensors, install one of these:
- **Arduino IDE** (easiest for beginners):
  - Download: https://www.arduino.cc/en/software
  - Add ESP32 support: File → Preferences → Additional Boards Manager URLs: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
  - Tools → Board → Boards Manager → Search "esp32" → Install latest.
  - Select board: "XIAO_ESP32C3" (or nRF52840 variant if using that).
- **PlatformIO** (recommended for PMSG repo):
  - Install VS Code + PlatformIO extension.
  - Clone repo: `git clone https://github.com/Control-C/PMSG`
  - Open `/PlatformIO` folder in VS Code → PIO Home → Build/Upload.
- Test flash: Load Blink example → Change LED pin if needed (check repo pinout) → Upload.

### 5. Run Basic Scans & Tests

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

- **Serial scan**: After connection, send `AT` or just watch for output. Reset board to see logs.
- **I²C Scan** (great for finding connected sensors/displays):
  - Flash this simple Arduino sketch (from examples or write new):
    ```cpp
    #include <Wire.h>
    void setup() {
      Serial.begin(115200);
      Wire.begin();  // Grove I²C pins
      Serial.println("I2C Scanner");
      for (byte addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
          Serial.print("Found device at 0x");
          if (addr < 16) Serial.print("0");
          Serial.println(addr, HEX);
        }
      }
    }
    void loop() {}
