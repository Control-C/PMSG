# PMSG Hardware Schematic & Pinout

> **Official ready-to-use PCBA**  
> Order directly from [pmsg.online](https://pmsg.online) or Seeed Studio.

PMSG turns ordinary glasses or goggles into modular smart glasses by replacing one (or both) temples with a custom PCBA based on the Seeed Studio XIAO platform.

## Board Overview

- Microcontroller: Seeed Studio XIAO series (ESP32-C3 / C6 / compatible variants)
- Expansion: Grove ports (I²C, UART, GPIO) 
Oled,Camera,IR LED / Flash Light ect see https://www.seeedstudio.com/grove
- Feedback: Vibration motor + addressable RGB LEDs
- Sensors: Onboard light + temperature/humidity (exact sensors depend on revision)
- Power: USB-C (primary) + battery monitoring support
- Connectivity: Wi-Fi + Bluetooth (external antenna recommended on some variants)


## Pinout & Connections

| Function                  | XIAO Pin / GPIO     | Notes                              | Typical Use / Library              |
|---------------------------|---------------------|------------------------------------|------------------------------------|
| Vibration Motor           | D10 (GPIO10)        | PWM capable                        | Haptic feedback                    |
| RGB LEDs (WS2812)         | D1 (GPIO1)          | 4× addressable                     | FastLED / Adafruit_NeoPixel        |
| Temperature + Humidity    | I²C                 | Onboard sensor                     | Wire + sensor library              |
| Ambient Light             | I²C                 | Onboard sensor                     | —                                  |
| Button 1                  | D8                  | Active high / pull-up              | User input                         |
| Button 2                  | Check schematic     | May share pin on some revisions    | —                                  |
| Grove I²C                 | SDA / SCL           | OLED, extra sensors, etc.          | Standard Grove                     |
| Grove UART                | TX / RX             | Camera or serial modules           | HardwareSerial                     |
| Grove GPIO                | D2 / D3             | General purpose                    | Digital I/O                        |
| Battery voltage sense     | A0                  | Voltage divider                    | analogRead()                       |
| USB-C / Debug Serial      | USB                 | 115200 baud recommended            | Serial                             |

> Always verify the exact pinout against the revision of the board you received. Minor differences can exist between XIAO variants (C3 / C6 / S3 / nRF).

## Power

- Primary power: USB-C
- 3.3 V regulated rail from the XIAO
- Battery monitoring available on A0
- Stand-alone battery operation supported (battery holder / charging depends on revision)

## Expansion & Accessories

- Grove connectors for quick addition of:
  - OLED displays
  - Cameras (UART)
  - Extra environmental sensors
  - Other 3.3 V modules
- Designed for modular experiments (BLE notifications, air-quality monitoring, haptics, simple AR cues, mesh networking, etc.)

## Ordering

Ready-made PCBA (no soldering or fabrication required):

- Official store: [pmsg.online](https://pmsg.online)
- Seeed Studio Fusion (search for PMSG or contact via the site)

Hinges, glasses frames, and additional Grove modules are sourced separately.

## Related Documentation

- Main project README
- Firmware examples (Arduino IDE, PlatformIO, Embedded Swift)
- AI-guided setup prompts
- LoRa / mesh / advanced variants (see AIS and related folders)

---

Questions or want to contribute improvements to the documentation? Open an issue or pull request.
