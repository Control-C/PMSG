# PMSG — Quick Start (NeoPixels + Vibration + Button)

PMSG (P.M. Smart Glasses) is a compact wearable electronics platform designed for
small development boards such as **Adafruit QT Py** and **Seeed Studio XIAO**.

This README shows a **minimal, correct PMSG GPIO demo** based on the existing
PMSG GPIO examples.

---

## PMSG + QT Py Hardware


![PMSG QT Py Assembled](https://raw.githubusercontent.com/Control-C/PMSG/main/img/qtpypmsg.png)

---

## Supported Boards

- Adafruit QT Py (SAMD21 / RP2040 / ESP32-S2 / ESP32-S3)
- Seeed Studio XIAO (ESP32 / ESP32-C3 / ESP32-S3 / nRF52840)

> ⚠️ Always select the **exact board model** when installing CircuitPython.  
> Pin naming and capabilities differ per board.
![PMSG QT Py Front](https://raw.githubusercontent.com/Control-C/PMSG/main/img/qtpy-pmsg.png)
---

## Pin Configuration (PMSG Default)

| Function           | Pin |
|--------------------|-----|
| NeoPixel Data      | D1 |
| NeoPixel Count     | 4 |
| Vibration Motor    | D10 |
| Button / Switch    | D9 |
| Power              | 3V3 = batt/i2c/uart or USB-C 5v | 
| Ground             | GND |

This pinout matches the **PMSG GPIO reference examples**.

---

## Requirements

### Software
- CircuitPython (matching your board)
- CircuitPython Library Bundle

### Libraries
Copy into `CIRCUITPY/lib/`:
- `neopixel.mpy`

---

## Installation

1. Flash CircuitPython to your board
2. Reconnect the board — it appears as `CIRCUITPY`
3. Copy `neopixel.mpy` into `CIRCUITPY/lib/`
4. Create or replace `code.py` with the example below

---

## `code.py` — Minimal PMSG GPIO Demo

- Button toggles LEDs
- Vibration motor follows LED state
- Safe defaults for wearable use

```python
import time
import board
import digitalio
import neopixel

# -------- CONFIG --------
PIXEL_PIN = board.D1
PIXEL_COUNT = 4
BUTTON_PIN = board.D9
VIBE_PIN = board.D10
BRIGHTNESS = 0.3
# ------------------------

pixels = neopixel.NeoPixel(
    PIXEL_PIN,
    PIXEL_COUNT,
    brightness=BRIGHTNESS,
    auto_write=False
)

button = digitalio.DigitalInOut(BUTTON_PIN)
button.direction = digitalio.Direction.INPUT
button.pull = digitalio.Pull.UP

vibe = digitalio.DigitalInOut(VIBE_PIN)
vibe.direction = digitalio.Direction.OUTPUT
vibe.value = False

state = False

def set_pixels(color):
    for i in range(PIXEL_COUNT):
        pixels[i] = color
    pixels.show()

set_pixels((0, 0, 0))

while True:
    if not button.value:  # button pressed
        state = not state

        if state:
            set_pixels((0, 150, 255))  # soft cyan
            vibe.value = True
        else:
            set_pixels((0, 0, 0))
            vibe.value = False

        time.sleep(0.3)  # debounce

    time.sleep(0.01)
