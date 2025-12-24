# PMSG


Recommended prompt:

"Help me build PMSG using this repository.
Ask which XIAO board I am using.
Check /ai/mcp/*.json for hardware, libraries, and versions."

# PMSG Quick-Start & Grok Assistant Hub (Single-File Guide)
## Overview
This is a **one-stop Markdown file** (.md) for getting PMSG (Personal Smart Monitoring Glasses) running fast, plus an enhanced Grok assistant prompt for AI help. Save as `pmsg-grok-hub.md` in your repo root. Paste your setup details into the **[My Setup]** section below, then copy-paste the whole prompt to query Grok (me!) for custom code, wiring, or debugging. Covers hardware, software, and next-gen ideas. Last updated: Dec 24, 2025.

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
