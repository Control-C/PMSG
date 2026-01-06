import time
import math

import board
import busio
import digitalio
import pwmio
import analogio

import neopixel

# =========================
# BLE UART (optional)
# =========================
class BLEUART:
    """
    BLE Nordic UARTService telemetry output.
    Auto-disables if BLE is not available on this board/port.
    """
    def __init__(self, enable=True, name="PMSG"):
        self.ok = False
        self.ble = None
        self.uart = None
        self.adv = None
        self._last_adv = 0.0

        if not enable:
            return

        try:
            from adafruit_ble import BLERadio
            from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
            from adafruit_ble.services.nordic import UARTService
        except Exception:
            return

        try:
            self.ble = BLERadio()
            self.uart = UARTService()
            self.adv = ProvideServicesAdvertisement(self.uart)
            try:
                self.ble.name = name
            except Exception:
                pass
            self.ok = True
        except Exception:
            self.ok = False

    def connected(self):
        return bool(self.ok and self.ble and self.ble.connected)

    def update_advertising(self):
        if not self.ok or self.connected():
            return
        now = time.monotonic()
        if (now - self._last_adv) > 2.0:
            self._last_adv = now
            try:
                if not self.ble.advertising:
                    self.ble.start_advertising(self.adv)
            except Exception:
                pass

    def write_line(self, s: str):
        if not self.connected():
            return
        try:
            self.uart.write((s + "\n").encode("utf-8"))
        except Exception:
            pass


# =========================
# Hardware UART (optional)
# =========================
class HWUART:
    def __init__(self, tx=None, rx=None, baud=115200):
        self.uart = None
        if tx is None:
            return
        try:
            self.uart = busio.UART(tx, rx, baudrate=baud, timeout=0)
        except Exception:
            self.uart = None

    def write_line(self, s: str):
        if not self.uart:
            return
        try:
            self.uart.write((s + "\n").encode("utf-8"))
        except Exception:
            pass


# =========================
# Utilities
# =========================
def clamp(v, lo, hi):
    return lo if v < lo else hi if v > hi else v

def wheel(pos):
    # 0..255 -> RGB
    pos = 255 - (pos % 256)
    if pos < 85:
        return (255 - pos * 3, 0, pos * 3)
    if pos < 170:
        pos -= 85
        return (0, pos * 3, 255 - pos * 3)
    pos -= 170
    return (pos * 3, 255 - pos * 3, 0)


# =========================
# Core I2C + Sensors
# =========================
class Sensors:
    """
    Scans I2C, then conditionally enables:
      - SHT31D @ 0x44
      - VEML7700 @ 0x10
      - LTR390 @ 0x53
      - LIS2DH12 @ 0x19 (via adafruit_lis2dh)
    If a library is missing, that sensor is skipped.
    """
    def __init__(self, enable=True, i2c_freq=400000):
        self.enable = enable
        self.i2c = None
        self.found = []

        self.sht31 = None
        self.veml = None
        self.ltr = None
        self.lis = None

        if not enable:
            return

        try:
            self.i2c = busio.I2C(board.SCL, board.SDA, frequency=i2c_freq)
            while not self.i2c.try_lock():
                time.sleep(0.01)
            self.found = list(self.i2c.scan())
            self.i2c.unlock()
        except Exception:
            self.i2c = None
            self.found = []
            return

        # SHT31D (0x44)
        if 0x44 in self.found:
            try:
                import adafruit_sht31d
                self.sht31 = adafruit_sht31d.SHT31D(self.i2c, address=0x44)
            except Exception:
                self.sht31 = None

        # VEML7700 (0x10)
        if 0x10 in self.found:
            try:
                import adafruit_veml7700
                self.veml = adafruit_veml7700.VEML7700(self.i2c, address=0x10)
            except Exception:
                self.veml = None

        # LTR390 (0x53)
        if 0x53 in self.found:
            try:
                import adafruit_ltr390
                self.ltr = adafruit_ltr390.LTR390(self.i2c, address=0x53)
            except Exception:
                self.ltr = None

        # LIS2DH12 (0x19)
        if 0x19 in self.found:
            try:
                import adafruit_lis2dh
                self.lis = adafruit_lis2dh.LIS2DH_I2C(self.i2c, address=0x19)
                try:
                    self.lis.range = adafruit_lis2dh.RANGE_4_G
                except Exception:
                    pass
            except Exception:
                self.lis = None

    def snapshot(self):
        d = {}

        if self.sht31:
            try:
                d["temp_c"] = round(self.sht31.temperature, 2)
                d["rh"] = round(self.sht31.relative_humidity, 2)
            except Exception:
                pass

        if self.veml:
            try:
                d["lux"] = round(self.veml.lux, 2)
            except Exception:
                pass

        if self.ltr:
            try:
                d["uv"] = int(self.ltr.uvs)
                d["als"] = int(self.ltr.light)
            except Exception:
                pass

        if self.lis:
            try:
                x, y, z = self.lis.acceleration
                d["accel"] = (round(x, 2), round(y, 2), round(z, 2))
            except Exception:
                pass

        return d


# =========================
# Vibration Motor (D10 PWM)
# =========================
class Vibro:
    def __init__(self, pin, freq=200):
        self.pwm = pwmio.PWMOut(pin, frequency=freq, duty_cycle=0)

    def off(self):
        self.pwm.duty_cycle = 0

    def buzz(self, ms=70, strength=0.65, freq=200):
        self.pwm.frequency = freq
        self.pwm.duty_cycle = int(clamp(strength, 0.0, 1.0) * 65535)
        time.sleep(ms / 1000.0)
        self.off()

    def click(self):
        self.buzz(ms=25, strength=0.75, freq=220)


# =========================
# Button (D9 Pullup)
# =========================
class Button:
    def __init__(self, pin):
        self.io = digitalio.DigitalInOut(pin)
        self.io.direction = digitalio.Direction.INPUT
        self.io.pull = digitalio.Pull.UP
        self._prev = self.io.value
        self._t = time.monotonic()
        self._pressed = False

    def update(self):
        now = time.monotonic()
        v = self.io.value
        if v != self._prev and (now - self._t) > 0.03:
            self._t = now
            self._prev = v
            if v is False:
                self._pressed = True

    def was_pressed(self):
        self.update()
        if self._pressed:
            self._pressed = False
            return True
        return False


# =========================
# NeoPixel FX (D1, 4 LEDs)
# =========================
class FX:
    def __init__(self, pixels, brightness=0.25):
        self.pixels = pixels
        self.base_brightness = brightness
        self.pixels.brightness = brightness
        self._h = 0
        self._spark = 0
        self._t0 = time.monotonic()

    def clear(self):
        self.pixels.fill((0, 0, 0))
        self.pixels.show()

    def boot_splash(self):
        self.clear()
        for i in range(len(self.pixels)):
            self.pixels[i] = (40, 40, 40)
            self.pixels.show()
            time.sleep(0.04)
        self.clear()

    def rainbow_frame(self, speed=3):
        self._h = (self._h + speed) % 256
        n = len(self.pixels)
        for i in range(n):
            self.pixels[i] = wheel(self._h + i * (256 // max(1, n)))
        self.pixels.show()

    def sparkle_frame(self):
        self.pixels.fill((2, 2, 2))
        self._spark = (self._spark + 1) % len(self.pixels)
        self.pixels[self._spark] = (90, 90, 90)
        self.pixels.show()

    def breathe_frame(self, color=(0, 150, 255), rate=2.0):
        t = time.monotonic() - self._t0
        b = (math.sin(t * rate) + 1.0) * 0.5
        self.pixels.brightness = self.base_brightness * (0.15 + 0.85 * b)
        self.pixels.fill(color)
        self.pixels.show()
        self.pixels.brightness = self.base_brightness


# =========================
# ADC inputs (Battery A0, Test A2)
# =========================
class AnalogInputs:
    def __init__(self, battery_pin, test_pin):
        self.bat = analogio.AnalogIn(battery_pin)
        self.test = analogio.AnalogIn(test_pin)

    def _ratio(self, adc):
        return adc.value / 65535.0

    def battery_voltage_est(self, vref=3.3, divider_ratio=2.0):
        # Adjust divider_ratio to match your PCB divider
        return self._ratio(self.bat) * vref * divider_ratio

    def battery_pct_rough(self, v):
        return clamp((v - 3.2) / (4.2 - 3.2), 0.0, 1.0) * 100.0

    def test_ratio(self):
        return self._ratio(self.test)


# =========================
# PMSG MAIN CLASS
# =========================
class PMSG:
    def __init__(
        self,
        pixel_pin,
        pixel_count,
        vibe_pin,
        button_pin,
        uart_tx=None,
        uart_rx=None,
        uart_baud=115200,
        enable_ble=True,
        enable_sensors=True,
        telemetry_interval_s=1.0,
        battery_adc_pin=board.A0,
        touch_adc_pin=board.A2,
        brightness=0.25,
    ):
        self.pixels = neopixel.NeoPixel(pixel_pin, pixel_count, brightness=brightness, auto_write=False)
        self.vib = Vibro(vibe_pin)
        self.btn = Button(button_pin)
        self.fx = FX(self.pixels, brightness=brightness)

        self.ble = BLEUART(enable=enable_ble, name="PMSG")
        self.hwuart = HWUART(tx=uart_tx, rx=uart_rx, baud=uart_baud)

        self.sensors = Sensors(enable=enable_sensors)
        self.analog = AnalogInputs(battery_adc_pin, touch_adc_pin)

        self.mode = 0
        self.telemetry_interval_s = telemetry_interval_s
        self._last_telemetry = 0.0

    def _out_line(self, msg: str):
        # USB serial
        print(msg)
        # BLE UART
        self.ble.write_line(msg)
        # Hardware UART
        self.hwuart.write_line(msg)

    def boot(self):
        self.fx.boot_splash()
        self.vib.click()

        if self.sensors.enable:
            if self.sensors.found:
                self._out_line("I2C scan: " + ", ".join([hex(a) for a in self.sensors.found]))
            else:
                self._out_line("I2C scan: none")

        self._out_line("PMSG ready. Button D9 cycles modes.")
        self._out_line("Modes: 0=rainbow, 1=sparkle, 2=breathe, 3=telemetry-only")

    def update(self):
        # BLE advertising (safe no-op if BLE not supported)
        self.ble.update_advertising()

        # Button -> next mode
        if self.btn.was_pressed():
            self.mode = (self.mode + 1) % 4
            self.vib.click()
            self.fx.clear()

        # LED FX
        if self.mode == 0:
            self.fx.rainbow_frame()
        elif self.mode == 1:
            self.fx.sparkle_frame()
        elif self.mode == 2:
            self.fx.breathe_frame()
        else:
            self.fx.clear()

        # Telemetry every N seconds (USB + BLE UART + HW UART)
        now = time.monotonic()
        if (now - self._last_telemetry) >= self.telemetry_interval_s:
            self._last_telemetry = now

            vb = self.analog.battery_voltage_est(vref=3.3, divider_ratio=2.0)
            bp = self.analog.battery_pct_rough(vb)
            a2 = self.analog.test_ratio()

            snap = self.sensors.snapshot() if self.sensors.enable else {}

            payload = {
                "bat_v_est": round(vb, 3),
                "bat_pct_rough": int(bp),
                "a2_ratio": round(a2, 4),
                **snap,
            }

            # One-line dict output (easy to parse)
            self._out_line(str(payload))
