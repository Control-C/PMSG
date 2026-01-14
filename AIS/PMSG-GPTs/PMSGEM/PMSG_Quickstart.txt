# PMSG – Quickstart Guide

PMSG is an **open-design wearable smart-glasses prototype** built for makers, labs, workshops, and demos.  
It connects hardware (ESP32 / nRF boards, sensors, LEDs) with ChatGPT-assisted setup and experimentation.

⚠️ **Prototype & R&D use only**. Not a certified medical or safety device.

---

## Supported Boards (common)
- Seeed XIAO ESP32S3
- Seeed XIAO nRF52840 Sense
- Other ESP32 / nRF boards (with manual pin mapping)

---

## What You Need
- PMSG PCB or breadboard setup
- USB-C cable
- Computer (macOS / Windows / Linux)
- Arduino IDE or PlatformIO
- Optional: phone for BLE testing

---

## Basic Setup (5 minutes)
1. Connect the board via USB
2. Open Arduino IDE or PlatformIO
3. Select the correct board & port
4. Flash a minimal firmware (LED or Serial test)
5. Verify output (LED blink or Serial message)

---

## First “It Works” Test
- Power on the board
- Confirm:
  - LED lights up or blinks
  - Serial Monitor prints a message
  - (Optional) BLE device appears

If this works → your PMSG hardware is alive 🎉

---

## Safety Notes
- Do not use while driving or operating machinery
- No medical, surveillance, or weaponized use
- Intended for education, creativity, and experimentation