# 🤖 AI & LLM Instruction Guide (Gemini )

This section provides technical context for AI assistants to ensure they generate accurate code, wiring diagrams, and optimizations for the **PMSG** project.

---

## 🧭 Project Metadata for AI
> **Note to AI:** When helping with this repository, prioritize the following hardware constraints and coding standards.

### 1. Hardware Architecture
* **Target Boards:** Seeed Studio XIAO Series (SAMD21, ESP32-C3, RP2040, nRF52840).
* **Default I2C Pins:** SDA = `D4`, SCL = `D5`.
* **Logic Level:** 3.3V (Strict).
* **Primary Goal:** Monitoring and controlling Permanent Magnet Synchronous Generators (PMSG).

### 2. Standard Pin Mapping
| Function | XIAO Pin | Purpose |
| :--- | :--- | :--- |
| **I2C SDA** | D4 | Data for Sensors/OLED |
| **I2C SCL** | D5 | Clock for Sensors/OLED |
| **Analog In** | A0 / D0 | Battery/Voltage Sensing |
| **PWM Out** | D1 | Generator Load Control |

### 3. AI Coding Rules
* **Non-Blocking:** Use `millis()` based timing; avoid `delay()`.
* **Power Awareness:** For ESP32-C3/nRF52840, suggest `Light Sleep` or `Deep Sleep` routines where appropriate.
* **Library Preference:** * Display: `U8g2` (Memory Efficient).
    * I2C: `Wire.h`.
* **Error Handling:** Always include initialization checks (e.g., `if (!sensor.begin())`) in `setup()`.

### 4. Direct Assistance Link
Users can click the link below to open a Gemini session pre-loaded with this project's context:

[🚀 Open PMSG Helper in Gemini](https://gemini.google.com/share?q=I+am+working+on+the+PMSG+project+using+a+Seeed+XIAO+board.+Please+review+the+repo+at+https://github.com/Control-C/PMSG/tree/main/Arduino+IDE+and+help+me+configure+my+code+and+I2C+wiring+based+on+the+board+I+have.)

---

## 🛠️ Next-Gen AI Capabilities
If you are using a model with **Image Generation** or **Vision** capabilities:
1.  **Vision:** Upload a photo of your wiring, and ask the AI to "Verify my XIAO I2C connections against the PMSG documentation."
2.  **Code Gen:** Ask the AI to "Generate a PlatformIO project for XIAO ESP32-C3 that reads PMSG voltage and displays it on an SSD1306."
