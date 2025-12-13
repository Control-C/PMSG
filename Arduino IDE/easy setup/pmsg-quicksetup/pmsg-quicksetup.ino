// Check arduino IDE 
// add this in Arduino IDE -> "settings..." -> "Additional boards manager URLs:
// https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
// Go to Tools -> "Get board info"
// Add Libarary manager this FastLED.h 
#include <FastLED.h>
// #include "MusicAndLightFX.h" // pmsg lib see or github

// Pins
#define BUTTON_PIN 9        // Boot button (GPIO9, active LOW)
#define VIB_PIN D10         // PWM vibration motor (GPIO18)
#define LED_STRIP_PIN D1    // WS2812 data pin (GPIO1)
#define NUM_LEDS 4          // 4 WS2812 LEDs
#define INTERNAL_LED LED_BUILTIN  // On-board LED (GPIO15, active LOW)

// FastLED setup
CRGB leds[NUM_LEDS];

bool oldButtonState = HIGH;  // For edge detection
bool testOn = false;  // Controls vib, LEDs, internal LED

void setup() {
  Serial.begin(115200);
  Serial.println("XIAO ESP32C6 Test: Vib D10 + 4 WS2812 D1 + Internal LED + Button D9 (No BLE)");

  // Pins
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(VIB_PIN, OUTPUT);
  pinMode(INTERNAL_LED, OUTPUT);
  analogWrite(VIB_PIN, 0);  // Vib off
  digitalWrite(INTERNAL_LED, HIGH);  // Internal LED off (active LOW)

  // FastLED
  FastLED.addLeds<WS2812, LED_STRIP_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  FastLED.clear();
  FastLED.show();

  Serial.println("Ready! Press D9 button to toggle test.");
}

void loop() {
  // Button toggle (falling edge press)
  bool buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW && oldButtonState == HIGH) {
    testOn = !testOn;
    Serial.println(testOn ? "Button: Test ON (Vib + LEDs + Internal)" : "Button: Test OFF");
    delay(50);  // Debounce
  }
  oldButtonState = buttonState;

  // Update based on testOn
  if (testOn) {
    // Vib: Full PWM buzz
    analogWrite(VIB_PIN, 255);

    // WS2812: Rainbow cycle for 4 LEDs
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV((millis() / 50 + i * 64) % 256, 255, 255);  // Rainbow
    }
    FastLED.show();

    // Internal LED: On
    digitalWrite(INTERNAL_LED, LOW);
  } else {
    // Off all
    analogWrite(VIB_PIN, 0);
    FastLED.clear();
    FastLED.show();
    digitalWrite(INTERNAL_LED, HIGH);
  }

  delay(20);  // Loop delay
}