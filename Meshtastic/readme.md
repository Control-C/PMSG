# 🛰️ PMSGv4 Meshtastic LoRa Nodes * Check your version ESP or NRF 

- **Meshtastic Network Introduction (Seeed Studio)**  
  https://wiki.seeedstudio.com/meshtastic_introduction/  [oai_citation:2‡Seeed Studio](https://wiki.seeedstudio.com/meshtastic_introduction/?utm_source=chatgpt.com)
  
- **XIAO ESP32-S3 OR Wio-SX1262 Kit for Meshtastic (Seeed Studio)**  
  https://wiki.seeedstudio.com/xiao_esp32s3_%26_wio_SX1262_kit_for_meshtastic_main_page/  [oai_citation:3‡Seeed Studio](https://wiki.seeedstudio.com/xiao_esp32s3_%26_wio_SX1262_kit_for_meshtastic_main_page/?utm_source=chatgpt.com)


This project lets you run a **Meshtastic LoRa node** with:

✅ ESP32-C / nRF LoRa or Seeed Studio XIAO   
✅ Optional XIAO nRF sensor host (I²C 3v3 / uart 3v3  Grove breakout)  
✅ Portable environmental telemetry  
✅ Vibration motor & push button I/O  
✅ Optional uplink to **The Things Network (TTN)** via wifi api / MQTT *( not good for you batt ) esp version 

📌 Based on Meshtastic firmware and community examples.  
https://meshtastic.org/docs/configuration/module/telemetry/
---

## 📦 Hardware & Wiring

### Supported Boards

| Board | LoRa | MCU |
|-------|------|-----|
| **ESP32-C LoRa** | ✔️ | ✔️ BLE / wifi sensor host |
| **Seeed XIAO nRF52840** | ✔️ | ✔️ BLE sensor host |

### Sensor Expansion — Grove I²C (3V3)

You can plug Grove sensors into a **3V3 I²C breakout**:

| Sensor | I²C Addr | Function |
|--------|----------|----------|
| SHT31 | `0x44`, `0x45` | Temp & Humidity |
| VEML7700 | `0x10` | Ambient Light |
| LTR390 | `0x53` | UV Light |

All sensors share SDA/SCL on the 3V3 bus.

#### Grove Wiring
⚠️ **Use only 3.3V power** — do not use 5V on the grove bus.

---

## 🧠 I/O Pins (Example)

| Function | Pin |
|----------|-----|
| **4 pixel ws2812b RGB led** | `D1` PIN 1 = output|
| **Vibration Motor (PWM)** | `D10` PIN 10 = output|
| **Push Button** | `D9` Pin9 = input |
| **I²C (Grove & Sensors)** | `SDA`, `SCL` |

Feel free to modify these in code or config.

---

## 🚀 Getting Started

### 1. Install Meshtastic Firmware

You can either:

✔ Flash using **Meshtastic Web Flasher**  
✔ Build & flash using **Arduino IDE or PlatformIO**

Meshtastic firmware supports devices such as **ESP32-C LoRa** and Seeed boards.  [oai_citation:1‡GitHub](https://github.com/meshtastic/Meshtastic-arduino/blob/master/README.md?utm_source=chatgpt.com)

**Arduino IDE Setup**
1. Install the ESP32 / nRF52840 board support
2. Add Meshtastic source to Arduino
3. Select correct board and flash

---

TODO WIP 

# 🛰️ PMSG + Meshtastic — Quick Setup Guide  
**Button, Vibration Motor & I²C Sensors (App Configuration Only)**

This guide explains how to configure your **PMSG / XIAO / ESP32-C** device in the **Meshtastic app** so that:

- 🌈 **D1** works as a **4 pixel display** ( ws2812b )
- 🔘 **D9** works as a **push button**
- 🔊 **D10** drives a **vibration motor**
- 📊 **I²C sensors** appear in the app (telemetry)

No firmware changes. No coding. Only app settings.

---

# 🎛️ Part 1 — Button & Vibration Motor Setup

## 📍 Pin Mapping (XIAO / PMSG default)

| Function | Pin |
|----------|-----|
| Four pixel display | **D1** |
| Push Button | **D9** |
| Vibration Motor (PWM) | **D10** |

---

## 1️⃣ Connect to your node

1. Power on your device  
2. Open the **Meshtastic app** (iOS / Android / Desktop)  
3. Connect via **BLE** or **USB**  
4. Wait until your node shows **Online**

---

## 2️⃣ Configure pins in the app

1. Go to **Settings**  
2. Open **Modules**  
3. Open **GPIO / IO**  
   *(sometimes called “External Notifications” or “I/O”)*

---

## 3️⃣ Set the vibration motor (D10)

In **GPIO / IO** settings:

- **Motor Enabled** → `ON`  
- **Motor PWM Pin** → `10`  
- **Motor Mode** → `PWM`  
- **Motor Power / Duty** → `150–200` (safe start)

Result:  
Meshtastic will use **D10** to drive the vibration motor for alerts and events.

---

## 4️⃣ Set the button (D9)

In the same menu:

- **Button Pin** → `9`  
- **Button Mode** → `Input Pull-Up`  
- **Active Level** → `LOW`

Wiring reminder:

---

## 5️⃣ Assign button actions

Go to:  
**Settings → Device → Button Actions**

Choose what the button does:

- Short press → Send position  
- Long press → Send preset message  
- Double press → Toggle power mode  

---

## 6️⃣ Save & reboot

Tap **Save** → **Reboot** the node (or power-cycle).

---

## ✅ Button & Motor Checklist

| Item | Value |
|------|------|
| Motor pin | **D10** | 🔊
| Button pin | **D9** | 🔘
| Button mode | **Input Pull-Up** |
| Button active | **LOW** |
| Motor mode | **PWM** |

---

# 📊 PART 2 — I²C Sensors Setup (Telemetry)

This makes your sensors visible in the Meshtastic app.

Supported sensors in this setup:

| Sensor | Function | I²C Address | Telemetry |
|--------|----------|-------------|-----------|
| **SHT31** | Temp & Humidity | `0x44` / `0x45` | 🌡 Temperature & 💧 Humidity  
| **VEML7700** | Ambient Light | `0x10` | 💡 Light level
| **LTR390** | UV Light | `0x53` | ☀️ UV index  

---

TODO custom firmware to add RGB PMSG4pixelDisplay and logica movement / tap / touch 
