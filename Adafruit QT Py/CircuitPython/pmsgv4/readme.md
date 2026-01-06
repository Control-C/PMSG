# PMSG on Adafruit QT Py (CircuitPython) 👓⚡️  
Quick guide + demo for **PMSG (Poor Man’s Smart Glasses)** running on **Adafruit QT Py** with **CircuitPython**.

> Repo references: PMSG project by Control-C  
> https://github.com/Control-C/PMSG  
> (This README is meant to be dropped into your own folder as `README.md`.)

---

## Photos

![PMSG QT Py + shield](https://raw.githubusercontent.com/Control-C/PMSG/main/img/qtpy-pmsg.png)

![PMSG QT Py assembled](https://raw.githubusercontent.com/Control-C/PMSG/main/img/qtpypmsg.png)

---

## What you get

- ✅ NeoPixel animations on the PMSG PCB (4 LEDs)
- ✅ Haptic feedback (vibration motor) with PWM “buzz patterns”
- ✅ Single button on **D9** to cycle modes
- ✅ Optional I²C sensors (auto-detected / easy to enable)

---

## Hardware / Pin map (CircuitPython)

| Function | Pin / Address | Description | Library / Notes |
|---|---:|---|---|
| Vibration Motor | **D10 (GPIO10)** | Feedback motor (haptic buzz / PWM tunes) | CircuitPython `pwmio.PWMOut` (see `pmsg_music_light_fx.py`) |
| 4x WS2812C-2020-V1 RGB LEDs | **D1 (GPIO1)** | Programmable pixel display for animations | `neopixel.NeoPixel` |
| SHT31 (Temp + Humidity) | **I²C 0x44** | Sensirion temperature & humidity sensor | `adafruit_sht31d` (if present on PCB) |
| SGP41 (VOC + NOx / CO2 index) | **I²C** (often **0x59**) | Sensirion air quality sensor | Not on all PCBs (SGP41 support varies) |
| VEML7700 | **I²C 0x10** | Ambient light (lux) sensor | `adafruit_veml7700` |
| LTR390 | **I²C 0x53** | UV light sensor | `adafruit_ltr390` |
| LIS2DH12 | **I²C 0x19** | Accelerometer / movement | `adafruit_lis2dh` |
| Battery Level | **A0 (ADC)** | Battery voltage → % power | `analogio.AnalogIn` (board dependent divider) |
| Vibration/Touch Input Test | **A2 (ADC)** | Capacitive touch/vibration sense test | `analogio.AnalogIn` |
| Touch Input (ESP32-S3 only) | **D3** | Dedicated touch pin | `touchio.TouchIn` (if supported on your build) |
| IR Transmitter | **D8** | IR remote protocol (TV, airco, etc.) | IR libs vary; not included in this demo |
| Push Button | **D9** | Onboard pushbutton for quick testing | `digitalio` input w/ pullup |

> Many demos use **D9** as a general-purpose input for quick hardware checks.

---

## Quick start (CircuitPython)

### 1) Flash CircuitPython
1. Put your QT Py into bootloader mode.
2. Install the latest CircuitPython build for your QT Py board.
3. You should see a `CIRCUITPY` drive.

### 2) Copy files to `CIRCUITPY`
Copy these into the drive:

- `code.py`
- `lib/pmsg_music_light_fx.py`

And install the required libraries into `lib/`:
- `neopixel.mpy`

Optional sensor libs (only if you want sensors):
- `adafruit_sht31d.mpy`
- `adafruit_veml7700.mpy`
- `adafruit_ltr390.mpy`
- `adafruit_lis2dh.mpy`
- `adafruit_bus_device/` (folder)
- `adafruit_register/` (folder)

> Easiest: download **Adafruit CircuitPython Library Bundle** and copy the .mpy files you need.

---

## Demo: what it does

- Boot → starts in an LED mode
- **Press button (D9)** → cycle modes:
  1. Rainbow
  2. Sparkle
  3. “Breathing” glow
  4. Vibration test pattern (buzz-buzz…)
- Long-press (optional) → can be added to trigger “SOS buzz” etc.

---

## Files

### `code.py`
Put this at the root of `CIRCUITPY`.

```py
import time
import board
import digitalio
import neopixel

from pmsg_music_light_fx import (
    Vibro,
    FX,
    Button,
)

# ---- PMSG pins (your setup) ----
PIXEL_PIN = board.GP1    # D1 (GPIO1)
PIXEL_COUNT = 4
VIBRO_PIN = board.GP10   # D10 (GPIO10)
BUTTON_PIN = board.GP9   # D9

# ---- NeoPixels ----
pixels = neopixel.NeoPixel(PIXEL_PIN, PIXEL_COUNT, brightness=0.25, auto_write=False)

# ---- Vibration motor ----
vib = Vibro(VIBRO_PIN)

# ---- Button ----
btn = Button(BUTTON_PIN)

# ---- FX ----
fx = FX(pixels, vib)

modes = [
    fx.rainbow,
    fx.sparkle,
    fx.breathe,
    fx.vibro_test,
]

mode = 0
fx.boot_splash()

while True:
    # tap = next mode
    if btn.was_pressed():
        mode = (mode + 1) % len(modes)
        vib.click()
        fx.clear()

    # run current mode (non-blocking-ish frames)
    modes[mode]()

    time.sleep(0.02)
