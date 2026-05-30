#include <BleGamepad.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>

BleGamepad bleGamepad("PMSGAMEPAD", "PMSG", 100);

Adafruit_LIS3DH lis = Adafruit_LIS3DH();

#define LED_PIN     D1
#define NUM_LEDS    4
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

const int VIBRATION_PIN = D10;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!lis.begin(0x19)) {
    Serial.println("LIS2DH12 not found!");
    while (1);
  }
  lis.setRange(LIS3DH_RANGE_2_G);

  pinMode(VIBRATION_PIN, OUTPUT);
  digitalWrite(VIBRATION_PIN, LOW);

  strip.begin();
  strip.show();

  bleGamepad.begin();
  
  Serial.println("✅ PMSG GAMEPAD Ready! for Windows / linux - will not work Apple Mac");

  analogWrite(VIBRATION_PIN, 220);
  delay(55);
  analogWrite(VIBRATION_PIN, 0);
  delay(55);
  analogWrite(VIBRATION_PIN, 220);
  delay(55);
  analogWrite(VIBRATION_PIN, 0);
}

void loop() {
  bool connected = bleGamepad.isConnected();

  int lux = readLux();
  int brightness = map(lux, 0, 3000, 5, 35);
  brightness = constrain(brightness, 5, 45);

  if (connected) {
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, brightness * 0.45));
    }
    strip.show();
  } else {
    static unsigned long lastBlink = 0;
    static bool ledState = false;
    if (millis() - lastBlink > 550) {
      ledState = !ledState;
      lastBlink = millis();
    }
    uint8_t b = ledState ? brightness : brightness / 3;
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, b));
    }
    strip.show();
  }

  if (connected) {
    sensors_event_t event;
    lis.getEvent(&event);

    int16_t leftX = map(event.acceleration.x * 100, -200, 200, -32767, 32767);
    int16_t leftY = map(event.acceleration.y * 100, -200, 200, -32767, 32767);
    int16_t rightX = map(event.acceleration.z * 100, -200, 200, -32767, 32767);

    bleGamepad.setLeftThumb(leftX, leftY);
    bleGamepad.setRightThumb(rightX, 0);

    Serial.print("X: ");
    Serial.print(event.acceleration.x, 2);
    Serial.print("  Y: ");
    Serial.print(event.acceleration.y, 2);
    Serial.print("  Z: ");
    Serial.println(event.acceleration.z, 2);

    if (abs(event.acceleration.x) > 12 || abs(event.acceleration.y) > 12 || abs(event.acceleration.z) > 12) {
      analogWrite(VIBRATION_PIN, 220);
      delay(50);
      analogWrite(VIBRATION_PIN, 0);
    }
  }

  delay(20);
}

int readLux() {
  Wire.beginTransmission(0x23);
  Wire.write(0x10);
  Wire.endTransmission();
  delay(120);
  Wire.requestFrom(0x23, 2);
  if (Wire.available() == 2) {
    uint16_t val = (Wire.read() << 8) | Wire.read();
    return val / 1.2;
  }
  return 600;
}
