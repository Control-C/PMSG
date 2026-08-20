/*
  PMSG – BLE Scanner + Dance/Mic + Lux/UV + Vibration
  Board package: Seeed nRF52 mbed-enabled Boards
  Board: Seeed XIAO nRF52840 Sense

  30× WS2813  → D7
  4× status   → D1
  Button      → D8
  Vibration   → D10
  VEML7700 + LTR390 via I2C
*/

#include <ArduinoBLE.h>
#include <Adafruit_NeoPixel.h>
#include <LSM6DS3.h>
#include <PDM.h>
#include <Wire.h>
#include <Adafruit_VEML7700.h>
#include <Adafruit_LTR390.h>

// ========== PINS ==========
#define STRIP_PIN     7     // D7 = 30× WS2813
#define STATUS_PIN    1     // D1 = 4 status pixels
#define BUTTON_PIN    8     // D8
#define VIB_PIN       10    // D10

#define NUM_STRIP     30
#define NUM_STATUS    4

// ========== OBJECTS ==========
Adafruit_NeoPixel strip(NUM_STRIP, STRIP_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel status(NUM_STATUS, STATUS_PIN, NEO_GRB + NEO_KHZ800);

LSM6DS3 imu(I2C_MODE, 0x6A);
Adafruit_VEML7700 veml;
Adafruit_LTR390 ltr;

// ========== GLOBALS ==========
uint8_t currentMode = 0;          // 0 = BLE Scanner, 1 = Dance+Mic
unsigned long lastButton = 0;
bool lastButtonState = HIGH;

uint16_t bleDeviceCount = 0;
unsigned long lastScan = 0;
const unsigned long SCAN_INTERVAL = 3000;

#define MIC_BUFFER 256
short sampleBuffer[MIC_BUFFER];
volatile int samplesRead = 0;
float micLevel = 0;

float motionLevel = 0;
float lastAx = 0, lastAy = 0, lastAz = 0;

float lux = 50;
float uvi = 0;

unsigned long lastAnim = 0;
uint8_t hue = 0;
uint8_t baseBrightness = 80;

unsigned long vibEnd = 0;

// ========== SETUP ==========
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("PMSG mbed version – ArduinoBLE + Dance + Lux/UV + Vib");

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(VIB_PIN, OUTPUT);
  digitalWrite(VIB_PIN, LOW);

  strip.begin();
  strip.setBrightness(baseBrightness);
  strip.show();

  status.begin();
  status.setBrightness(50);
  status.show();

  // IMU
  if (imu.begin() != 0) Serial.println("IMU fail");
  else Serial.println("IMU OK");

  // Lux
  if (!veml.begin()) Serial.println("VEML7700 fail");
  else {
    Serial.println("VEML7700 OK");
    veml.setGain(VEML7700_GAIN_1);
    veml.setIntegrationTime(VEML7700_IT_100MS);
  }

  // UV
  if (!ltr.begin()) Serial.println("LTR390 fail");
  else {
    Serial.println("LTR390 OK");
    ltr.setMode(LTR390_MODE_UVS);
    ltr.setGain(LTR390_GAIN_3);
    ltr.setResolution(LTR390_RESOLUTION_16BIT);
  }

  // Mic
  PDM.onReceive(onPDMdata);
  if (!PDM.begin(1, 16000)) Serial.println("PDM fail");

  // BLE
  if (!BLE.begin()) {
    Serial.println("BLE begin failed!");
    while (1);
  }
  Serial.println("BLE OK – starting scanner");
  BLE.scan();   // start continuous scan

  updateStatusPixels();
  vibrate(80);
  Serial.println("Ready – press D8 to switch mode");
}

// ========== LOOP ==========
void loop() {
  handleButton();
  updateSensors();
  handleVibration();
  updateBLECount();

  if (currentMode == 0) {
    animateBLEScanner();
  } else {
    animateDance();
  }

  delay(8);
}

// ========== BUTTON ==========
void handleButton() {
  bool state = digitalRead(BUTTON_PIN);
  if (state == LOW && lastButtonState == HIGH && millis() - lastButton > 350) {
    currentMode = (currentMode + 1) % 2;
    lastButton = millis();
    Serial.print("Mode → ");
    Serial.println(currentMode == 0 ? "BLE Scanner" : "Dance + Mic");
    updateStatusPixels();
    strip.clear();
    strip.show();
    vibrate(120);
  }
  lastButtonState = state;
}

void updateStatusPixels() {
  status.clear();
  uint32_t c = (currentMode == 0) ? status.Color(0, 40, 180) : status.Color(180, 0, 120);
  for (int i = 0; i < NUM_STATUS; i++) status.setPixelColor(i, c);
  status.show();
}

// ========== BLE COUNT (ArduinoBLE) ==========
void updateBLECount() {
  static uint16_t countInWindow = 0;
  static unsigned long windowStart = 0;

  // Tel advertenties in een venster van 2.5 seconden
  BLEDevice peripheral = BLE.available();
  if (peripheral) {
    countInWindow++;
    // Optioneel: Serial.println(peripheral.address());
  }

  if (millis() - windowStart > 2500) {
    bleDeviceCount = countInWindow;
    countInWindow = 0;
    windowStart = millis();

    if (currentMode == 0) {
      Serial.print("BLE devices (approx): ");
      Serial.print(bleDeviceCount);
      Serial.print("  | Lux: ");
      Serial.print(lux, 0);
      Serial.print("  | UVI: ");
      Serial.println(uvi, 1);
    }
  }
}

// ========== SENSORS ==========
void updateSensors() {
  // Motion
  float ax = imu.readFloatAccelX();
  float ay = imu.readFloatAccelY();
  float az = imu.readFloatAccelZ();
  float dx = abs(ax - lastAx);
  float dy = abs(ay - lastAy);
  float dz = abs(az - lastAz);
  motionLevel = constrain((dx + dy + dz) * 8.0f, 0.0f, 1.0f);
  lastAx = ax; lastAy = ay; lastAz = az;

  // Mic
  if (samplesRead > 0) {
    long sum = 0;
    for (int i = 0; i < samplesRead; i++) sum += abs(sampleBuffer[i]);
    micLevel = constrain((float)sum / (samplesRead * 8000.0f), 0.0f, 1.0f);
    samplesRead = 0;
  }

  // Lux
  static unsigned long lastLux = 0;
  if (millis() - lastLux > 400) {
    lastLux = millis();
    lux = veml.readLux();
    baseBrightness = constrain(map((long)lux, 0, 800, 140, 35), 25, 160);
    strip.setBrightness(baseBrightness);
  }

  // UV
  if (ltr.newDataAvailable()) {
    uvi = ltr.readUVS() / 2300.0f;
  }
}

// ========== VIBRATION ==========
void vibrate(uint16_t ms) {
  digitalWrite(VIB_PIN, HIGH);
  vibEnd = millis() + ms;
}

void handleVibration() {
  if (vibEnd && millis() > vibEnd) {
    digitalWrite(VIB_PIN, LOW);
    vibEnd = 0;
  }

  static unsigned long lastExtra = 0;
  if (millis() - lastExtra > 3000) {
    if (bleDeviceCount >= 12 || motionLevel > 0.7f) {
      vibrate(60);
      lastExtra = millis();
    }
  }
}

// ========== ANIMATIES ==========
void animateBLEScanner() {
  uint8_t activeLeds = constrain(map(bleDeviceCount, 0, 18, 3, NUM_STRIP), 3, NUM_STRIP);
  uint8_t speed = constrain(map(bleDeviceCount, 0, 18, 4, 18), 4, 18);
  uint8_t sat = constrain(200 + (int)(uvi * 20), 180, 255);

  if (millis() - lastAnim > (42 - speed)) {
    lastAnim = millis();
    hue += 2;

    for (int i = 0; i < NUM_STRIP; i++) {
      if (i < activeLeds) {
        uint8_t v = 35 + i * 6;
        strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(hue + i * 550, sat, v)));
      } else {
        strip.setPixelColor(i, 0);
      }
    }
    strip.show();
  }
}

void animateDance() {
  float intensity = max(motionLevel, micLevel * 0.9f);
  intensity = constrain(intensity + uvi * 0.08f, 0.0f, 1.0f);

  uint8_t bright = 25 + (uint8_t)(intensity * 180);
  uint8_t speed = 3 + (uint8_t)(intensity * 22);

  if (millis() - lastAnim > (48 - speed)) {
    lastAnim = millis();
    hue += 3 + (uint8_t)(intensity * 9);

    for (int i = 0; i < NUM_STRIP; i++) {
      uint16_t pixelHue = hue + (i * 65536L / NUM_STRIP);
      uint8_t v = bright;
      if (micLevel > 0.38f && (i % 3 == 0)) v = min(255, v + 90);
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue, 255, v)));
    }
    strip.show();
  }
}

// ========== PDM ==========
void onPDMdata() {
  int bytesAvailable = PDM.available();
  PDM.read(sampleBuffer, bytesAvailable);
  samplesRead = bytesAvailable / 2;
}
