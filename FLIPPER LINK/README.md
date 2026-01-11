# PMSG FLIPPE LINK 🐬👓 — Flipper Zero controller for PMSG (BLE * UART USBC)

This repo contains Flipper Zero external apps to control **PMSG smart glasses**.

## 1) BLE (today): `pmsg_ble_beacon`
Flipper **broadcasts** tiny BLE advertisement packets ("extra beacon").
PMSG scans BLE advertisements and executes the command (BUZZ, LED modes, etc.).

Why beacon mode?
- It’s fast and simple (no pairing / no GATT complexity)
- Works great for “remote trigger” actions
- Flipper extra beacon payload is limited to 31 bytes.  [oai_citation:2‡developer.flipper.net](https://developer.flipper.net/flipperzero/doxygen/extra__beacon_8h_source.html)

### Command packet format (BLE ADV)
We send standard ADV data with:
- Flags
- Manufacturer Specific Data (Company ID `0x1337`)
- ASCII payload: `PMSG:<command>`

Example payload string:
- `PMSG:BUZZ`
- `PMSG:LED:ON`
- `PMSG:RAINBOW`

**NOTE:** Keep commands short-ish. ADV is tiny.

### PMSG side (what you implement)
- BLE scan
- parse manufacturer data (type 0xFF)
- verify company id `0x1337`
- read ASCII string `PMSG:...`
- run the matching action

## 2) UART (next): `pmsg_uart_remote`
Second app will send the same `PMSG:<command>\n` strings over UART on Flipper GPIO.
Wiring reference: UART TX/RX + GND (pins 13, 14, 11), 3.3V logic.  [oai_citation:3‡Flipper Forum](https://forum.flipper.net/t/wiring-diagram-of-the-serial-port-for-connection-to-flipper-zero/7132?utm_source=chatgpt.com)

---

## Building / Installing (uFBT)
Use uFBT (micro Flipper Build Tool) to build external apps.
Typical flow:
- Put `apps/pmsg_ble_beacon` into your external apps workspace
- `ufbt` build + launch

---

## Supported commands (v0)
### Actuators
- `PMSG:BUZZ`
- `PMSG:vibration-tune1`
- `PMSG:vibration-tune2`
- `PMSG:vibration-tune3`
- `PMSG:vibration-sonar`
- `PMSG:LED:ON`
- `PMSG:RAINBOW`
- `PMSG:leftanimation`
- `PMSG:rightanimation`
- `PMSG:allblue`
- `PMSG:allblue-flash`
- `PMSG:allgreen-flash`
- `PMSG:allgreen`

### Sensors (request)
- `PMSG:LUX`
- `PMSG:UV`
- `PMSG:movement`
- `PMSG:temp`
- `PMSG:humidity`
- `PMSG:tap`
- `PMSG:touch`

For sensors over beacon: you usually broadcast *requests*, then PMSG replies via:
- another beacon in the other direction, OR
- UART, OR
- a real BLE connection (GATT) (not the beacon approach)

---

## Safety / disclaimer
PMSG is a prototype platform for R&D and creative experiments.
