# Muse on PMSG

PMSG stands for Prototype Modular Smart Glasses. This folder is how to put a Muse gadget on the glasses arm.

Site: https://pmsg.online
SDK fork: https://github.com/Control-C/Pmsg-muse-gadget-sdk
Upstream: https://github.com/facebookincubator/muse-gadget-sdk
Board pack (pins, overlay, BLE-PS sketch): that fork, plus `board/` here.

Muse is the phone-side agent. PMSG is the face computer: 4-pixel bar, vibration, button, lux, UV, movement. BLE-PS stays on the air even when Muse is connected. A Wi-Fi session does not replace 📵 📷 📸 🎤.

## What you need

- PMSG V4 or higher, XIAO seated. Start with **XIAO ESP32C6**.
- USB-C data cable.
- Muse app (iOS or Android), Developer mode on.
- SDK token: https://gadgets.muse.ai/settings/sdk-tokens
- Computer with ESP-IDF **v6.0.1** (macOS or Linux). Other IDF versions are not supported by the SDK.

nRF52840 is a PMSG host for LoRa. It is not an ESP32. Do not flash this Muse build on it.

## Pins the firmware must keep

| Part | Silk | XIAO ESP32C6 GPIO |
| --- | --- | --- |
| 4× WS2812C-2020 | D1 | GPIO1 |
| Button | D9 | GPIO20 |
| Vibration motor | D10 | GPIO18 (PWM) |
| I²C SDA / SCL | D4 / D5 | GPIO22 / GPIO23 |
| VEML7700 lux | I²C | `0x10` |
| LTR390 UV | I²C | `0x53` |
| LIS2DH12 movement | I²C | `0x19` |

3.3 V only. Full map: `board/pmsg-hardware.json`.

## Start

```sh
git clone https://github.com/Control-C/Pmsg-muse-gadget-sdk
cd Pmsg-muse-gadget-sdk/esp32
cp /path/to/PMSG/Muse/board/sdkconfig.pmsg-xiao-esp32c6 devices/

. ~/esp/esp-idf-v6/export.sh
idf.py -B build-pmsg-c6 -DIDF_TARGET=esp32c6 \
  -DSDKCONFIG=build-pmsg-c6/sdkconfig \
  -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;devices/sdkconfig.pmsg-xiao-esp32c6" \
  menuconfig
```

Set **ESP32 Device SDK > Muse Gadgets SDK token**. Then:

```sh
idf.py -B build-pmsg-c6 -DIDF_TARGET=esp32c6 \
  -DSDKCONFIG=build-pmsg-c6/sdkconfig \
  -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;devices/sdkconfig.pmsg-xiao-esp32c6" \
  build
idf.py -B build-pmsg-c6 -p /dev/cu.usbmodem1101 flash monitor
```

Hold BOOT, tap RESET, release BOOT if the port does not sync.

In the Muse app: Settings > Devices > Developer mode > Add Device. Name looks like `MuseGadget-XXXXXX`. When the 4 pixels breathe blue, press D9. Hold D9 about 5 seconds to reset setup.

C6 has no PSRAM. Home-network tunnel stays off. Muse can still pair and read sensors.

## Status on the bar

Orange breathing = ready. Blue breathing = press D9. Green = connected, one short buzz on D10. Red blink = check the log. Keep brightness at 8–16. The pixels sit next to the eye.

## Add BLE-PS

Preference protocol, not a kill switch. Spec and sketch:

`BLE-bluetooth-privacy-signal/`

Glasses advertise `PMSG_📷` idle, `PMSG_📸` recording, `PMSG_🎤` mic. A bystander advertises `📵` or `📵 DNR`. On a heard 📵: amber bar, double buzz, do not open a new clip. Workshop UUID `0000f1a0-0000-1000-8000-00805f9b34fb`.

Flash `BLE-bluetooth-privacy-signal/examples/bleps_pmsg.ino` first if you only want the privacy demo. Do not run that sketch and the Muse firmware on the same module at the same time. When both are in one build, Muse owns Wi-Fi and the pixel status lamp; BLE-PS owns the second advert. 📵 wins over a new recording.

## Ask an agent

Point it at `AIS/assistants/muse.md` and `AIS/mcp/muse_bleps.json`. Ask which XIAO is seated before it writes GPIO numbers.
