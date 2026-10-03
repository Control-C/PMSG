# Muse — agent notes

PMSG stands for Prototype Modular Smart Glasses. https://pmsg.online

Recommended prompt:

"Help me put Muse on this PMSG. Ask which XIAO is seated. Read AIS/mcp/muse_bleps.json and Muse/board/pmsg-hardware.json before you write pins. Keep BLE-PS (📵 📷 📸 🎤) in the advert."

## Rules

- Host for Muse: XIAO ESP32C6 first. ESP-IDF v6.0.1 only.
- Silk is not GPIO. On C6: D1 = GPIO1, D9 = GPIO20, D10 = GPIO18. On S3: D1 = GPIO2, D9 = GPIO8, D10 = GPIO9.
- 3.3 V. No `delay()` in the gadget loop. Vibration is PWM.
- 4 pixels are the status UI. Cap brightness at 16.
- BLE-PS is a preference protocol, not a kill switch. Names: `PMSG_📷` `PMSG_📸` `PMSG_🎤` `PMSG_📸🎤`. Bystander: `📵` or `📵 DNR`. Byte `0x20` means do not record. Never show 📷 and 📸 together.
- Do not flash Muse and Meshtastic on the same module.
- SDK token ships in the firmware. Do not commit it.

## Links

- Start: `Muse/README.md` in this repo
- SDK: https://github.com/Control-C/Pmsg-muse-gadget-sdk
- Token: https://gadgets.muse.ai/settings/sdk-tokens
- BLE-PS: `BLE-bluetooth-privacy-signal/README.md`
- Sketch: `BLE-bluetooth-privacy-signal/examples/bleps_pmsg.ino`
