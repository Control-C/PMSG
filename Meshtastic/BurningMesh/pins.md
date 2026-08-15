# PMSG Pin Map (XIAO)

Quick reference for both ESP32-S3 and nRF52840 versions.

## Core PMSG pins

| Function                  | XIAO label | GPIO (ESP32-S3) | Notes                     |
|---------------------------|------------|-----------------|---------------------------|
| 4× WS2812C-2020-V1        | D1         | GPIO2           | Data pin for the 4 LEDs   |
| Button                    | D9         | GPIO8           | Input Pull-Up, Active LOW |
| Vibration motor (PWM)     | D10        | GPIO9           | PWM, duty 150–200         |
| I²C SDA                   | D4         | GPIO5           | Grove 3.3 V only          |
| I²C SCL                   | D5         | GPIO6           | Grove 3.3 V only          |

## Optional / extra

| Function                  | XIAO label | GPIO (ESP32-S3) | Notes                     |
|---------------------------|------------|-----------------|---------------------------|
| Extra 30× WS2812C         | D7 / RX    | GPIO44          | Free pin, good for strip  |
| Alternative button        | D2         | GPIO3           | If you prefer             |
| UART TX                   | D6         | GPIO43          | Normally free             |

## Power notes

- WS2812C LEDs → power from **5 V**, common GND with the Xiao
- Grove sensors → **3.3 V only** (never 5 V)
- Always connect the LoRa antenna before powering the board

## LoRa (Wio-SX1262)

Uses the B2B connector / SPI.  
Do not re-assign D8 / D9 / D10 if the kit is stacked the standard way.
