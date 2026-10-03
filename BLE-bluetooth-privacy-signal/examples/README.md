# BLE-PS example — 📵 📷 📸 🎤

PMSG stands for Prototype Modular Smart Glasses. BLE-PS (work name BLEPS) is the open preference protocol: a bystander can say do not record, and a wearable can say it has a camera, is recording, or has a live mic.

This is not a kill switch and not a hidden-camera detector. Spec: [BLE-bluetooth-privacy-signal](https://github.com/Control-C/PMSG/tree/main/BLE-bluetooth-privacy-signal). Demo page: [pmsg.online/ble-privacy.html](https://pmsg.online/ble-privacy.html).

## Marks

| Who | Name | Service-data byte | Meaning |
| --- | --- | --- | --- |
| Bystander phone or tag | `📵` or `📵 DNR` | `0x20` | Please do not record me |
| Glasses, idle | `PMSG_📷` | `0x11` | Camera on board, not recording, honours 📵 |
| Glasses, recording | `PMSG_📸` | `0x13` | Photo or video live |
| Glasses, mic only | `PMSG_🎤` | `0x1C` | Mic writing, honours 📵 |
| Glasses, picture and mic | `PMSG_📸🎤` | `0x1F` | Camera and mic live |

Never advertise `📷` and `📸` at the same time. Append `🎤` only while the mic is writing.

Workshop UUIDs until the Bluetooth SIG assigns 16-bit numbers:

```
0000f1a0-0000-1000-8000-00805f9b34fb    Recording Presence Service
0000f1a3-0000-1000-8000-00805f9b34fb    Do-Not-Record Request
```

Byte bits: 0 has_camera, 1 camera_active, 2 has_microphone, 3 audio_active, 4 honors_dnr, 5 dnr_requested.

## What the sketch does

`bleps_pmsg.ino` runs on a XIAO in the PMSG arm (ESP32-C6 or S3).

1. Advertises `PMSG_📷` / `0x11` at boot.
2. D9 short press cycles 📷 → 📸 → 📸🎤 → 🎤 → 📷, and flips the BLE name within one second.
3. Scans for `📵`, `📵 DNR`, or service data with bit 5 set.
4. On 📵: four pixels go amber, D10 buzzes twice, a new 📸 or 🎤 state is refused. The advert stays up.
5. D9 long press clears the latch so the wearer can record anyway. The stamp still says DNR was present.

Pixel bar (D1, keep brightness low):

| State | Pixels |
| --- | --- |
| 📷 idle | dim white |
| 📸 recording | red pulse |
| 🎤 mic | purple |
| 📸🎤 | red, purple, red, purple |
| 📵 heard | amber, all four |

## Flash

Arduino IDE, board package for the seated XIAO. Library: NimBLE-Arduino. Helper headers `pmsg4pixeldisplay.h` and `pmsg2vibrate.h` live in the [PMSG Arduino IDE tree](https://github.com/Control-C/PMSG/tree/main/Arduino%20IDE). The sketch also runs without them: it drives GPIO D1 and D10 directly.

Phone check: nRF Connect, advertiser name `📵 DNR`, UUID `0000F1A0-0000-1000-8000-00805F9B34FB`, service data `20`.

## Muse

A Muse gadget on Wi-Fi does not replace this advert. If the glasses run the Muse firmware and a camera or mic path is added later, keep a second advert with these names. Muse status colours stay on the pixel bar only while BLE-PS is idle. 📵 wins: amber bar, buzz, do not open a new clip.
