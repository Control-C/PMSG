# AIS update — Muse and BLE-PS

Paste this under the tool list in `AIS/README.md`. PMSG stands for Prototype Modular Smart Glasses.

## Muse

Phone agent on the glasses arm. XIAO ESP32C6, 4-pixel bar as the status lamp, D9 to pair, D10 to buzz, lux / UV / movement on I²C.

Start: [Muse/README.md](../Muse/README.md)
Agent file: [assistants/muse.md](assistants/muse.md)
Context: [mcp/muse_bleps.json](mcp/muse_bleps.json)
SDK: https://github.com/Control-C/Pmsg-muse-gadget-sdk

Ask the model which XIAO is seated. Silk D10 is not GPIO10 on C6 or S3.

## BLE-PS

Preference protocol. Bystander says 📵. Glasses say 📷 idle, 📸 recording, 🎤 mic.

Folder: [BLE-bluetooth-privacy-signal](../BLE-bluetooth-privacy-signal)
Example sketch: [examples/bleps_pmsg.ino](../BLE-bluetooth-privacy-signal/examples/bleps_pmsg.ino)

Muse on Wi-Fi does not replace the BLE advert. 📵 means do not open a new clip. It does not switch the camera off by force.
