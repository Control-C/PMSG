# PMSG Protocol v1  
### Unified BLE + UART Control Protocol for Wearable Smart Glasses

PMSG is an **open-design wearable R&D platform** designed to work with:
- **Flipper Zero**
- **Pebble-style external controllers**
- **ESP32 & nRF52 firmware**
- UART and BLE (advertising-first, connection-optional)

This protocol is inspired by **DSMR-style design principles**:
- simple framing  
- versioning  
- predictable fields  
- extensibility  
- optional checksums  
- capability discovery  

---

## 1. Design Goals

- One **shared protocol** for BLE and UART
- BLE works with **Flipper Zero limitations** / WEB BLE chrome and Pebble Fork app for mobile todo 
- UART provides **full duplex control + sensor data** 
- Human-readable **JSON over UART**
- Compact **binary BLE beacon frames**
- Easy to implement on **ESP32 & nRF52**
- Safe for prototyping, demos, workshops

---

## 2. Transports

### 2.1 UART (recommended)
Use UART when you want:
- acknowledgements
- sensor values
- debugging
- logging

**Settings**
- Baud rate: `115200`
- Format: `8N1`
- Framing: **JSON Lines** (`\n` terminated)

---

### 2.2 BLE Advertising Beacon (Flipper-friendly) and for ProxyLens  chrome extension
Use BLE beacon mode when you want:
- instant triggers
- no pairing
- no connection
- low latency

Limitations:
- very small payload
- no ACK
- best for **actions**, not data streaming

---

## 3. UART API (JSON Lines)

### 3.1 Command (Controller → PMSG)

```json
{"p":"pmsg","v":1,"t":"cmd","id":14,"seq":12,"arg":120}
