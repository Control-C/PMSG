import time
import math
import board
import digitalio
import pwmio

# ---------------------------
# VIBRATION (D10)
# ---------------------------
class Vibro:
    """
    Simple vibro driver using PWMOut.
    Works for haptic buzz patterns and quick feedback clicks.
    """
    def __init__(self, pin, freq=200):
        self.pwm = pwmio.PWMOut(pin, frequency=freq, duty_cycle=0)
        self._last = time.monotonic()

    def off(self):
        self.pwm.duty_cycle = 0

    def on(self, strength=0.6):
        # strength: 0..1
        strength = 0.0 if strength < 0 else (1.0 if strength > 1 else strength)
        self.pwm.duty_cycle = int(65535 * strength)

    def buzz(self, strength=0.7, duration=0.08):
        self.on(strength)
        time.sleep(duration)
        self.off()

    def click(self):
        # short tactile click
        self.buzz(strength=0.75, duration=0.03)

    def double_click(self):
        self.click()
        time.sleep(0.05)
        self.click()

# ---------------------------
# BUTTON (D9)
# ---------------------------
class Button:
    """
    Simple debounced button helper.
    Assumes button to GND with internal pull-up.
    """
    def __init__(self, pin):
        self.io = digitalio.DigitalInOut(pin)
        self.io.direction = digitalio.Direction.INPUT
        self.io.pull = digitalio.Pull.UP

        self._prev = self.io.value
        self._last_change = time.monotonic()
        self._pressed_event = False

    def update(self):
        now = time.monotonic()
        v = self.io.value

        # debounce ~30ms
        if v != self._prev and (now - self._last_change) > 0.03:
            self._last_change = now
            self._prev = v

            # pressed = goes LOW
            if v is False:
                self._pressed_event = True

    def was_pressed(self):
        self.update()
        if self._pressed_event:
            self._pressed_event = False
            return True
        return False

# ---------------------------
# LED FX (D1 / 4 pixels)
# ---------------------------
class FX:
    def __init__(self, pixels, vibro=None):
        self.pixels = pixels
        self.vibro = vibro
        self.t0 = time.monotonic()
        self._spark_i = 0
        self._rainbow_h = 0

    def clear(self):
        self.pixels.fill((0, 0, 0))
        self.pixels.show()

    def boot_splash(self):
        # quick “hello” sequence
        self.pixels.fill((0, 0, 0))
        self.pixels.show()
        for i in range(len(self.pixels)):
            self.pixels[i] = (30, 30, 30)
            self.pixels.show()
            time.sleep(0.05)
        self.clear()
        if self.vibro:
            self.vibro.double_click()

    # --- helpers ---
    @staticmethod
    def _wheel(pos):
        # pos: 0..255
        if pos < 0 or pos > 255:
            return (0, 0, 0)
        if pos < 85:
            return (255 - pos * 3, pos * 3, 0)
        if pos < 170:
            pos -= 85
            return (0, 255 - pos * 3, pos * 3)
        pos -= 170
        return (pos * 3, 0, 255 - pos * 3)

    # --- modes ---
    def rainbow(self):
        # smooth hue shift across 4 pixels
        self._rainbow_h = (self._rainbow_h + 2) % 256
        for i in range(len(self.pixels)):
            c = self._wheel((self._rainbow_h + i * 64) % 256)
            self.pixels[i] = c
        self.pixels.show()

    def sparkle(self):
        # dark base + single sparkle chasing
        self.pixels.fill((2, 2, 2))
        self._spark_i = (self._spark_i + 1) % len(self.pixels)
        self.pixels[self._spark_i] = (80, 80, 80)
        self.pixels.show()

    def breathe(self):
        # sine-wave brightness breathing
        t = time.monotonic() - self.t0
        b = (math.sin(t * 2.0) + 1.0) * 0.5  # 0..1
        val = int(5 + b * 70)
        self.pixels.fill((val, val, val))
        self.pixels.show()

    def vibro_test(self):
        # LED pulses + vibration pattern (slow)
        t = time.monotonic() - self.t0
        phase = int(t) % 4

        if phase == 0:
            self.pixels.fill((40, 0, 0))
        elif phase == 1:
            self.pixels.fill((0, 40, 0))
        elif phase == 2:
            self.pixels.fill((0, 0, 40))
        else:
            self.pixels.fill((30, 30, 30))
        self.pixels.show()

        # buzz once per second on phase change
        # (simple edge detect)
        now = time.monotonic()
        if int(now) != int(self.t0 + t - 0.02):
            if self.vibro:
                self.vibro.buzz(strength=0.6, duration=0.06)
