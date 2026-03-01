# PMSG AI Assistant Setup Guide

PMSG = **P.M. Smart Glasses** — open face computer / wearable platform using Seeed Studio XIAO boards.

Repo: https://github.com/Control-C/PMSG

Goal: Set up PMSG hardware to feed data (camera, sensors, mic, IMU) to AI assistants like OpenClaw[](https://openclaw.ai), Claude, Gemini, local LLMs, etc. via BLE, WiFi, or USB-C.

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
