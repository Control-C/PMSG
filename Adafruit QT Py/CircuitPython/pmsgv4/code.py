import time
import board

from pmsg_music_light_fx import PMSG

# ---- PMSG default pins (your hardware table) ----
PIXEL_PIN = board.D1
PIXEL_COUNT = 4
VIBE_PIN = board.D10
BUTTON_PIN = board.D9

# Optional hardware UART output
# Example (ONLY if your board exposes these pins): UART_TX = board.TX, UART_RX = board.RX
UART_TX = None
UART_RX = None

pmsg = PMSG(
    pixel_pin=PIXEL_PIN,
    pixel_count=PIXEL_COUNT,
    vibe_pin=VIBE_PIN,
    button_pin=BUTTON_PIN,
    uart_tx=UART_TX,
    uart_rx=UART_RX,
    uart_baud=115200,
    enable_ble=True,       # auto-disables if BLE not supported
    enable_sensors=True,   # scans and reads sensors if libs exist
    telemetry_interval_s=1.0,
)

pmsg.boot()

while True:
    pmsg.update()
    time.sleep(0.02)
