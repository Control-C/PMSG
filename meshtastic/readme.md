# 🛰️ PMSGv4 Meshtastic LoRa Node  
**Arduino / PlatformIO Example for ESP32-C LoRa & Seeed XIAO LoRa / nRF + Sensor Expansion**

This project lets you run a **Meshtastic LoRa node** with:

✅ Seeed Studio XIAO (LoRa or nRF) or ESP32-C LoRa 
✅ Optional I2C / UART sensor host (I²C 3v3 - Uart 3v3 / Grove breakout)  
✅ Portable environmental telemetry  
✅ Vibration motor & push button I/O  
✅ Optional uplink to **The Things Network (TTN)**

📌 Based on Meshtastic firmware and community examples.  [oai_citation:0‡GitHub](https://github.com/meshtastic/Meshtastic-arduino/blob/master/README.md?utm_source=chatgpt.com)

---

## 📦 Hardware & Wiring

### Supported Boards

| Board | LoRa | MCU |
|-------|------|-----|
| **ESP32-C LoRa** | ✔️ | ESP32 |
| **Seeed XIAO LoRa** | ✔️ | nRF |
| **Seeed XIAO nRF52840** | ❌ LoRa | ✔️ BLE / sensor host |

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
| **Vibration Motor (PWM)** | `D10` |
| **Push Button** | `D9` |
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
1. Install the ESP32 / RP2040 / nRF52840 board support
2. Add Meshtastic source to Arduino
3. Select correct board and flash

---

## 📡 The Things Network (TTN)

You can configure your node to send uplink telemetry to TTN:

1. Create a TTN application
2. Register your device  
3. Copy the DevEUI / AppEUI / AppKey  
4. Add to your Meshtastic config

Example snippet:

```yaml
lora:
  region: EU868
  app_eui: XXXXXXXX
  dev_eui: XXXXXXXX
  app_key: XXXXXXXX
