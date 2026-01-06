import time
import board
import busio
import digitalio
import pwmio
import analogio
import neopixel

# =========================
# PMSG PIN MAP *( Check PMSG version on you board and type MCU ) 
# =========================
# Vibration motor: D10 (PWM)
# NeoPixels (4x WS2812C-2020-V1): D1
# Push Button: D9
# Battery level ADC: A0
# Vibration/touch input test ADC: A2
# Touch input (ESP32-S3 only): D3 (optional)
# IR TX: D8 (optional)

# if this demo works have look at special PMSG LIB for Vibration motor and pmsg4pixeldisplay ect 

PIXEL_PIN = board.D1
PIXEL_COUNT = 4
BUTTON_PIN = board.D9
VIBE_PIN = board.D10
BATTERY_ADC_PIN = board.A0
TOUCH_ADC_PIN = board.A2

BRIGHTNESS = 0.25  # safe-ish wearable brightness
I2C_SCAN_ON_BOOT = True

# =========================
# Helpers
# =========================
def clamp(v, lo, hi):
    return lo if v < lo else hi if v > hi else v

def wheel(pos):
    # simple rainbow helper (0-255)
    pos = 255 - (pos % 256)
    if pos < 85:
        return (255 - pos * 3, 0, pos * 3)
    if pos < 170:
        pos -= 85
        return (0, pos * 3, 255 - pos * 3)
    pos -= 170
    return (pos * 3, 255 - pos * 3, 0)

def set_pixels(pixels, color):
    for i in range(PIXEL_COUNT):
        pixels[i] = color
    pixels.show()

def pulse_pixels(pixels, base_color=(0, 150, 255), steps=25, delay=0.01):
    # quick breathe effect
    for s in range(steps):
        scale = s / (steps - 1)
        pixels.brightness = BRIGHTNESS * scale
        set_pixels(pixels, base_color)
        time.sleep(delay)
    for s in range(steps - 1, -1, -1):
        scale = s / (steps - 1)
        pixels.brightness = BRIGHTNESS * scale
        set_pixels(pixels, base_color)
        time.sleep(delay)
    pixels.brightness = BRIGHTNESS

def buzz(vibe_pwm, ms=80, duty=0.6, freq=180):
    # simple haptic buzz via PWM
    vibe_pwm.frequency = freq
    vibe_pwm.duty_cycle = int(clamp(duty, 0.0, 1.0) * 65535)
    time.sleep(ms / 1000.0)
    vibe_pwm.duty_cycle = 0

def buzz_tune(vibe_pwm):
    # tiny "tune" - change frequencies
    for f in (160, 220, 180, 260):
        buzz(vibe_pwm, ms=60, duty=0.55, freq=f)
        time.sleep(0.03)

def read_adc_percent(adc):
    # raw 0..65535
    return adc.value / 65535.0

def battery_voltage_estimate(adc_ratio, vref=3.3, divider_ratio=2.0):
    # If you have a 2:1 divider to ADC (common): Vbatt ≈ adc_ratio * vref * 2
    # Adjust divider_ratio to match your PCB.
    return adc_ratio * vref * divider_ratio

def battery_percent_from_voltage(v):
    # rough LiPo mapping (very approximate)
    # 3.2V ~ 0%, 4.2V ~ 100%
    return clamp((v - 3.2) / (4.2 - 3.2), 0.0, 1.0)

# =========================
# Setup: Pixels, Button, PWM, ADC
# =========================
pixels = neopixel.NeoPixel(
    PIXEL_PIN,
    PIXEL_COUNT,
    brightness=BRIGHTNESS,
    auto_write=False
)
set_pixels(pixels, (0, 0, 0))

button = digitalio.DigitalInOut(BUTTON_PIN)
button.direction = digitalio.Direction.INPUT
button.pull = digitalio.Pull.UP

vibe_pwm = pwmio.PWMOut(VIBE_PIN, frequency=180, duty_cycle=0)

bat_adc = analogio.AnalogIn(BATTERY_ADC_PIN)
touch_adc = analogio.AnalogIn(TOUCH_ADC_PIN)

# =========================
# Setup: I2C + Sensors (optional)
# =========================
i2c = busio.I2C(board.SCL, board.SDA, frequency=400000)

# wait for I2C to be ready
while not i2c.try_lock():
    time.sleep(0.01)

found = []
try:
    found = list(i2c.scan())
finally:
    i2c.unlock()

if I2C_SCAN_ON_BOOT:
    print("I2C devices found:", [hex(a) for a in found])

# Try importing sensor libs (skip if not present)
sht31 = None
veml = None
ltr = None
sgp41 = None

try:
    import adafruit_sht31d
    sht31 = adafruit_sht31d.SHT31D(i2c, address=0x44)
except Exception as e:
    print("SHT31 not available:", repr(e))

try:
    import adafruit_veml7700
    veml = adafruit_veml7700.VEML7700(i2c, address=0x10)
except Exception as e:
    print("VEML7700 not available:", repr(e))

try:
    import adafruit_ltr390
    ltr = adafruit_ltr390.LTR390(i2c, address=0x53)
except Exception as e:
    print("LTR390 not available:", repr(e))

# SGP41 note:
# - I2C address is 0x59 (fixed).  [oai_citation:2‡Sensirion AG](https://sensirion.com/resource/datasheet/sgp41?utm_source=chatgpt.com)
# - CircuitPython library may not be in the official bundle on all setups.
# If you do have a working SGP41 CP library, put it in /lib and adjust import below.
try:
    import adafruit_sgp41  # only works if you have this driver in CIRCUITPY/lib
    sgp41 = adafruit_sgp41.SGP41(i2c, address=0x59)
except Exception as e:
    print("SGP41 skipped (no CP driver found):", repr(e))

# =========================
# Main loop
# =========================
state = False
last_button = True
rainbow_offset = 0
last_print = 0.0

# boot feedback
pulse_pixels(pixels, (0, 150, 255))
buzz_tune(vibe_pwm)

print("PMSG demo running. Button D9 toggles mode.")
print("Mode OFF: LEDs off, no vibe. Mode ON: rainbow + periodic sensor print.")

while True:
    now = time.monotonic()

    # -------- Button edge detect --------
    cur = button.value
    if last_button and (not cur):  # falling edge (pressed)
        state = not state
        if state:
            buzz(vibe_pwm, ms=80, duty=0.6, freq=220)
            pulse_pixels(pixels, (0, 200, 120))
        else:
            buzz(vibe_pwm, ms=60, duty=0.5, freq=160)
            set_pixels(pixels, (0, 0, 0))
        time.sleep(0.15)  # debounce
    last_button = cur

    # -------- LED behavior --------
    if state:
        # rainbow animation
        for i in range(PIXEL_COUNT):
            pixels[i] = wheel(rainbow_offset + i * 32)
        pixels.show()
        rainbow_offset = (rainbow_offset + 3) % 256

    # -------- Periodic telemetry --------
    if state and (now - last_print) > 1.0:
        last_print = now

        bat_ratio = read_adc_percent(bat_adc)
        touch_ratio = read_adc_percent(touch_adc)

        # adjust divider_ratio if your PCB differs
        vbatt = battery_voltage_estimate(bat_ratio, vref=3.3, divider_ratio=2.0)
        bperc = battery_percent_from_voltage(vbatt) * 100.0

        print("\n--- PMSG Telemetry ---")
        print("Battery ADC ratio:", round(bat_ratio, 4))
        print("Battery est. V:", round(vbatt, 3), "V  (est.)")
        print("Battery est. %:", int(bperc), "%  (very rough)")
        print("Touch/Vibe test ADC ratio (A2):", round(touch_ratio, 4))

        if sht31:
            print("SHT31:  T =", round(sht31.temperature, 2), "C | RH =", round(sht31.relative_humidity, 2), "%")
        if veml:
            print("VEML7700: Lux =", round(veml.lux, 2))
        if ltr:
            # LTR390 provides UV and ALS channels
            print("LTR390: UV =", ltr.uvs, "| ALS =", ltr.light)
        if sgp41:
            # depends on driver API; leave as placeholder print
            print("SGP41: driver present (API depends on library).")

        # little haptic tick each print
        buzz(vibe_pwm, ms=20, duty=0.35, freq=200)

    time.sleep(0.02)
