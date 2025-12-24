# PMSG


Recommended prompt:

"Help me build PMSG using this repository.
Ask which XIAO board I am using.
Check /ai/mcp/*.json for hardware, libraries, and versions."

# PMSG Quick-Start & Grok Assistant Hub (Single-File Guide)
## Overview
This is a **one-stop Markdown file** (.md) for getting PMSG (Personal Smart Monitoring Glasses) running fast, plus an enhanced Grok assistant prompt for AI help. Save as `pmsg-grok-hub.md` in your repo root. Paste your setup details into the **[My Setup]** section below, then copy-paste the whole prompt to query Grok (me!) for custom code, wiring, or debugging. Covers hardware, software, and next-gen ideas. Last updated: Dec 24, 2025.

## Quick-Start Checklist (Get Running in <1 Hour)
### Hardware (~$20-50)
- **Core**: Seeed XIAO ESP32C3 (~$5). [Buy here](https://www.seeedstudio.com/Seeed-XIAO-ESP32C3-p-5945.html).
- **Sensors (Grove I2C)**: HDC1080 (temp/hum, ~$3), BH1750 (light, ~$2). Wire: SDA=D4, SCL=D5, VCC=3V3.
- **Extras**: OLED SSD1306 (~$4), buttons on D8/D10, LiPo battery (3.7V, ~$5).
- **Assembly**: Clip XIAO to glasses arm. Solder if using PCB (order from repo's Gerber files).
- **Tools**: USB-C cable, soldering iron (optional for proto).

### Software (Arduino IDE)
1. Install Arduino IDE: [Download](https://www.arduino.cc/en/software).
2. Add Boards: Preferences > Additional URLs: `https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json`. Install "Seeed SAMD Boards".
3. Clone Repo: `git clone https://github.com/Control-C/PMSG.git`.
4. Flash Basic: Open `/Arduino IDE/basic_sensor.ino`, select "XIAO_ESP32C3", upload. Monitor at 115200 baud.
5. Test: Serial shows "Hello PMSG!" + sensor data? Good!

### Common Fixes
- Bootloader: Double-tap reset button.
- I2C Error: Add 4.7kΩ pull-ups on SDA/SCL.
- BLE: Use nRF Connect app to scan.

## Enhanced Grok Prompt (Paste This to Me for Help)
Copy the block below **exactly** into a Grok chat. Edit **[My Setup]** with your details first—then I'll generate code, diagrams, or fixes tailored to PMSG.
