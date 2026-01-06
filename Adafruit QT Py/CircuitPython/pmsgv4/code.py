import time
import board
import neopixel

from pmsg_music_light_fx import PMSG

# ---- PMSG default pins (per your table quick you hardware !!! ) ----
PIXEL_PIN = board.D1
PIXEL_COUNT = 4
VIBE_PIN = board.D10
BUTTON_PIN = board.D9

# Optional hardware UART output (disabled by default)
# If you want it: set UART_TX to a real pin for your board (example: board.D6)
UART_TX = None
UART_RX = None

pmsg = PMSG(
    pixel_pin=PIXEL_PIN,
    pixel_count=PIXEL_COUNT,
    vibe_pin=VIBE_PIN,
    button_pin=BUTTON_PIN,
    uart_tx=UART_TX,
    uart_rx=UART_RX,
    enable_ble=True,       # will auto-disable if BLE libs/radio not present
    enable_sensors=True,
)

pmsg.boot()

while True:
    pmsg.update()
    time.sleep(0.02)
