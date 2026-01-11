# PMSG ↔ Flipper Zero  
## USB-C → UART (Power + Control)

!!!! WIP Still working on this... 

This document explains how to connect **PMSG smart glasses** to **Flipper Zero** using **UART**, while optionally powering PMSG from Flipper — similar to accessory-style setups (e.g. VR headsets).

This setup is intended for **R&D, prototyping, workshops, and demos**.

---

## 🎯 Goal

- **Control:** Flipper Zero sends `PMSG:*` commands to PMSG  
- **Data:** UART (TX / RX)  
- **Power:**  
  - 3.3V (low-power mode)  
  - 5V (preferred, full-power mode)  
- **Connector:** USB-C (used as a physical connector, not USB protocol)

---

## ⚠️ Important note about USB-C

- ❌ Standard USB-C cable → will NOT work
- ✅ UART works only if **UART signals are physically wired**
- ✅ Power works only if **VBUS is explicitly routed**

In this project, **USB-C is used as a carrier for UART + power**, not as USB data. 

Will work on that USB-C LINK Kabel simular to TI-83 back in day OG of link cables ;) 

---

## ✅ Recommended & proven setup (works today)

### UART via Flipper GPIO (reference design)

Flipper Zero exposes **UART on GPIO pins** (3.3V logic).

### Wiring

| Flipper Zero GPIO | PMSG |
|------------------|------|
| TX               | UART RX |
| RX               | UART TX |
| GND              | GND |
| 3V3 (optional)   | VCC (low-power only) |

- **Baud rate:** `115200`
- **Format:** `8N1`
- **Protocol:** ASCII, newline terminated (`\n`)
- **Voltage level:** `3.3V`

This is what the **PMSG UART Remote** Flipper app uses.

---

## 🔋 Powering PMSG from Flipper

### Option A — 3.3V from Flipper (simple, limited)

- Flipper provides **3.3V** on GPIO
- Works for:
  - MCU
  - BLE
  - Sensors
- Not sufficient for:
  - High-brightness LEDs
  - Vibration motors at full power

Use for **light demos** or **logic-only testing**.

---

### Option B — 5V power (recommended)

For the full PMSG experience (LEDs, vibration, radio), **5V is recommended**.

#### 1️⃣ External 5V (safest)
- Battery or power bank
- UART still comes from Flipper GPIO
- Zero risk to Flipper hardware

#### 2️⃣ USB-C “special cable” (advanced / experimental)

A **custom cable or breakout** that carries:
- **5V (VBUS)** → PMSG power input
- **UART TX/RX** → PMSG UART pins
- **GND**

⚠️ This is **not a standard USB cable**  
It is a **USB-C-shaped UART + power cable**.

---

## 🔌 USB-C → USB-C expectations (reality check)

- Flipper Zero is **not a USB host**
- ESP32 / nRF USB ports expect a **USB host**
- Therefore:
  - ❌ Plug-and-play USB-C ↔ USB-C does NOT work
  - ✅ Custom wiring **can work** if UART + power are exposed

Think of USB-C here as:
> A compact, reversible connector  
> carrying **power + UART**, not USB data

---

## 🧠 Mental model

- **UART** = simple, reliable, hackable  
- **USB** = complex (host/device, drivers, descriptors)

For PMSG we intentionally choose **UART**, even if the connector is USB-C.

This is similar to:
- accessory modes
- dev kits
- wearable & XR prototyping hardware

---

## 🧪 Prototype disclaimer

PMSG is an **open-design experimental platform**.

- Intended for:
  - R&D
  - creative coding
  - workshops
  - demos
- Not intended as:
  - consumer-grade USB device
  - certified USB product (without redesign)

---

## ✅ Summary

- ✔ UART is the communication layer  
- ✔ Flipper GPIO is the reference interface  
- ✔ 3.3V works for low power  
- ✔ 5V is recommended for full power  
- ⚠ USB-C cable must be **custom**, not standard  

---

If needed next:
- USB-C pinout for a **PMSG UART cable**
- Wiring diagram (simple schematic)
- ESP32 / nRF UART firmware example
- Auto-detect power mode (3.3V vs 5V)

👓⚡
