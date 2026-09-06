# BLE Privacy Signal

Workshop convention so a person can say **do not record me** over Bluetooth, and a camera wearable can say **I have a camera / I am recording**.

PMSG is the first proof of concept that **checks these symbols while recording**.

```
Bystander phone / badge     →  📵     please do not record me
Glasses / AI wearable       →  📷     camera on board, idle
                            →  📸     recording photo or video
                            →  🎤     microphone live
```

This is a **preference protocol**, not a kill switch and not a hidden-camera detector.

- Demo: https://pmsg.online/ble-privacy.html. *( soon WIP )
- Glasses project: https://github.com/Control-C/PMSG
- Site: https://pmsg.online
- Article: https://www.linkedin.com/pulse/ble-privacy-signal-wearables-having-recording-audio-video-teqmeq
- Org: TeQMeQ (EU)

---

## Two sides

| You are in the room | You make a recording device |
| --- | --- |
| Advertise `📵` or `📵 DNR` from your phone or a cheap tag | Put `📷` / `📸` / `🎤` in the BLE name you already advertise |
| nRF Connect works today. Hope Apple / Google add an OS toggle later | Honour `📵`: warn the wearer, do not start a new clip, keep advertising |
| Range is a room, not a city | Stamp the file so social platforms can flag it |

Bluetooth SIG: give this a 16-bit **Recording Presence Service**. Appearance may say “eyewear, camera-capable”. Appearance must **not** say “currently recording”.

---

## Copy these names

```
📵
📵 DNR

PMSG_📷
PMSG_📸
PMSG_🎤
PMSG_📸🎤
PMSG_DNR

YOURBRAND_📷
YOURBRAND-CAM
YOURBRAND-REC
YOURBRAND-MIC
YOURBRAND-DNR
```

Never show `📷` and `📸` at the same time. Append `🎤` only while the mic is writing.

Workshop UUID (until the SIG assigns 16-bit numbers):

```
0000f1a0-0000-1000-8000-00805f9b34fb    Recording Presence Service
0000f1a3-0000-1000-8000-00805f9b34fb    Do-Not-Record Request
```

One service-data byte:

```
bit 0  has_camera
bit 1  camera_active
bit 2  has_microphone
bit 3  audio_active
bit 4  honors_dnr
bit 5  dnr_requested     (this advert IS a 📵 beacon)
```

`0x20` = 📵 badge. `0x11` = idle camera that honours DNR. `0x13` = camera live.

---

## Quick start — user (nRF Connect)

1. Advertiser → name `📵 DNR`
2. Add UUID `0000F1A0-0000-1000-8000-00805F9B34FB`
3. Optional service data `20`
4. Start

A second phone or a PMSG board should see it. That is the whole bystander story until the OS ships a toggle:

`Settings → Privacy → Nearby recording → Broadcast “do not record”`

---

## Quick start — maker (ESP32 / PMSG)

```cpp
#include <NimBLEDevice.h>

static NimBLEUUID svc("0000f1a0-0000-1000-8000-00805f9b34fb");

void startPrivacyAdv(const char* name, uint8_t bits) {
  NimBLEDevice::init(name);
  NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
  adv->addServiceUUID(svc);
  NimBLEAdvertisementData data;
  data.setName(name);
  data.setServiceData(svc, std::string((char*)&bits, 1));
  adv->setAdvertisementData(data);
  adv->start();
}

// idle camera, honours 📵
// startPrivacyAdv("PMSG_📷", 0x11);

// recording
// startPrivacyAdv("PMSG_📸", 0x13);
```

When record starts, flip the name `📷` → `📸` within one second.  
If you set bit 4 and you hear `📵`, do not open a new clip. Tell the wearer why. Keep advertising.

PMSG does this first, as the open POC. Copy the marks. Keep your own prefix.

Stamp on capture:

```
pmsg:protocol        ble-privacy-signal/0.1
pmsg:dnr_present     true | false
pmsg:capture_state   idle | photo | video | audio | av
```

```html
<meta name="pmsg:dnr-present" content="true">
```

---

## Honest limits

| Can we | |
| --- | --- |
| Broadcast “do not record” from a phone, tag or PMSG | Yes |
| Warn when `PMSG_` / `📸` / a known glasses name is nearby | Yes, best effort |
| Know an arbitrary pair of glasses is recording right now | Only if it exposes the bit |
| Physically stop a non-compliant camera | No |
| Advertise BLE from a web page | No |
| Do this in iOS Safari | No — native app or OS toggle |
| Add Meta data that recording was done in a do not record  | Let Meta / snap / Tiktok ect block or don't allow to go public  |

About 8–9/10 as a preference protocol. About 3/10 as a universal detector. Do not market the first as the second.

---


