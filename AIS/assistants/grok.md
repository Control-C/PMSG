# PMSG


Recommended prompt:

"Help me build PMSG using this repository.
Ask which XIAO board I am using.
Check /ai/mcp/*.json for hardware, libraries, and versions."

# 🤖 AI & LLM Instruction Guide (Grok)

This section provides technical context for AI assistants to ensure they generate accurate code, wiring diagrams, and optimizations for the **PMSG** project.

---

## 🧭 Project Metadata for AI
> **Note to AI:** When helping with this repository, prioritize the following hardware constraints and coding standards. Use your tools (e.g., code_execution for simulation, web_search for updates) to verify details.

### 1. Hardware Architecture
* **Target Boards:** Seeed Studio XIAO Series (SAMD21, ESP32-C3, RP2040, nRF52840).
* **Default I2C Pins:** SDA = `D4`, SCL = `D5`.
* **Logic Level:** 3.3V (Strict).
* **Primary Goal:** Building affordable, modular smart glasses (Poor Men's Smart Glasses) with sensors, BLE/WiFi, and IoT for environmental monitoring.

### 2. Standard Pin Mapping
| Function | XIAO Pin | Purpose |
|----------|----------|---------|
| **I2C SDA** | D4 | Data for Sensors/OLED |
| **I2C SCL** | D5 | Clock for Sensors/OLED |
| **Analog In** | A0 / D0 | Battery/Voltage Sensing |
| **PWM Out** | D1 | Vibration Motor/LED Control |
| **Buttons** | D8 (Up), D10 (Down) | User Input |
| **BLE/WiFi** | Built-in | Connectivity (ESP32-C3) |

### 3. AI Coding Rules
* **Non-Blocking:** Use `millis()` based timing; avoid `delay()` for responsiveness in wearables.
* **Power Awareness:** For ESP32-C3/nRF52840, suggest `esp_light_sleep_start()` or `esp_deep_sleep_start()` for battery life >24h.
* **Library Preference:** 
  * Display: `Adafruit_SSD1306` or `U8g2` (Memory Efficient).
  * Sensors: `Adafruit_HDC1008`, `BH1750`.
  * BLE: `BLEDevice.h` (ESP32 Arduino Core).
* **Error Handling:** Always include initialization checks (e.g., `if (!sensor.begin()) { Serial.println("Error!"); }`) in `setup()`. Add Serial debug at 115200 baud.
* **Simulation:** Use code_execution tool to test sketches before flashing.

### 4. Direct Assistance Link
Users can click the link below to open a Grok session pre-loaded with this project's context:

[🚀 Open PMSG Helper in Grok](https://grok.com/share?q=I%20am%20working%20on%20the%20PMSG%20project%20using%20a%20Seeed%20XIAO%20board.%20Please%20review%20the%20repo%20at%20https://github.com/Control-C/PMSG%20and%20help%20me%20configure%20my%20code%20and%20I2C%20wiring%20based%20on%20the%20board%20I%20have.%20Use%20your%20tools%20for%20accurate%20responses.)

---

## 🛠️ Next-Gen AI Capabilities
If you are using Grok with **tool access**:
1. **Code Execution:** Ask Grok to "Simulate this Arduino sketch for XIAO ESP32-C3 reading HDC1080 sensor data using code_execution."
2. **Web/X Search:** "Search for latest Seeed XIAO ESP32-C3 pinout updates" or "Find community fixes for BLE on PMSG glasses."
3. **Vision/Image Analysis:** Upload a wiring photo and ask "Analyze this image for I2C connection errors on my PMSG setup using view_image."
4. **Code Gen:** Ask Grok to "Generate a full Arduino sketch for PMSG with BLE notification of temp/humidity, including deep sleep optimization."

For advanced users: Integrate with xAI API for real-time data processing (details at https://x.ai/api).
