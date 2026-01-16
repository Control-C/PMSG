#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <FastLED.h>
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_VEML6070.h>

// ─────────────────────────────────────────────
// Hardware definitions for XIAO ESP32C6
// ─────────────────────────────────────────────
#define NUM_LEDS          4
#define LED_STRIP_PIN     1       // D1 = GPIO1 → WS2812 data
#define VIB_PIN           18      // D10 = GPIO18 → vibration/buzzer PWM
#define BUTTON_PIN        20      // D9 = GPIO20 → button (active LOW)
#define ONBOARD_LED_PIN   15      // Yellow USER LED (active LOW)

// I²C pins (default on XIAO ESP32C6)
#define SDA_PIN           22      // D4 = GPIO22 → SDA
#define SCL_PIN           23      // D5 = GPIO23 → SCL

CRGB leds[NUM_LEDS];

BH1750 lightMeter;                  // Lux sensor
Adafruit_VEML6070 uv = Adafruit_VEML6070();  // UV sensor

// BLE UUIDs (PMSG style)
#define SERVICE_UUID        "504d5347-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic *pCharacteristic;

bool oldButtonState = HIGH;

// Callback when client writes to the characteristic
class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();
    if (rxValue.length() > 0) {
      String cmd = String(rxValue.c_str());
      cmd.trim();

      Serial.print("Received: "); Serial.println(cmd);

      if (cmd == "/buzz") {
        analogWrite(VIB_PIN, 255);
        delay(400);
        analogWrite(VIB_PIN, 0);
        pCharacteristic->setValue("Buzz OK");
        pCharacteristic->notify();
      }
      else if (cmd == "/Flash") {
        for (int times = 0; times < 4; times++) {
          fill_solid(leds, NUM_LEDS, CRGB::White);
          FastLED.show();
          delay(120);
          FastLED.clear();
          FastLED.show();
          delay(120);
        }
        pCharacteristic->setValue("Flash OK");
        pCharacteristic->notify();
      }
      else if (cmd == "/green") {
        fill_solid(leds, NUM_LEDS, CRGB::Green);
        FastLED.show();
        pCharacteristic->setValue("Green OK");
        pCharacteristic->notify();
      }
      else if (cmd == "/red") {
        fill_solid(leds, NUM_LEDS, CRGB::Red);
        FastLED.show();
        pCharacteristic->setValue("Red OK");
        pCharacteristic->notify();
      }
      else if (cmd == "/lux") {
        float lux = lightMeter.readLightLevel();
        if (lux < 0) lux = 0;  // BH1750 returns -1 on error sometimes
        String reply = "Lux: " + String(lux, 1);
        pCharacteristic->setValue(reply.c_str());
        pCharacteristic->notify();
        Serial.println(reply);
      }
      else if (cmd == "/uv") {
        uint16_t uvRaw = uv.readUV();
        float uvIndex = uvRaw / 100.0;  // VEML6070 returns raw, divide by 100 for index approx
        String reply = "UV Index: " + String(uvIndex, 1);
        pCharacteristic->setValue(reply.c_str());
        pCharacteristic->notify();
        Serial.println(reply);
      }
      else {
        pCharacteristic->setValue("Unknown cmd");
        pCharacteristic->notify();
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("PMSG BitChat – XIAO ESP32C6 (BLE + LEDs + Vib + Button + BH1750 + VEML6070)");

  // ─── Pins ─────────────────────────────────────
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(ONBOARD_LED_PIN, OUTPUT);
  digitalWrite(ONBOARD_LED_PIN, HIGH);   // Off (active LOW)

  pinMode(VIB_PIN, OUTPUT);
  analogWrite(VIB_PIN, 0);               // Off

  // FastLED
  FastLED.addLeds<WS2812, LED_STRIP_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(60);
  FastLED.clear();
  FastLED.show();

  // ─── I²C Sensors ──────────────────────────────
  Wire.begin(SDA_PIN, SCL_PIN);          // Explicit pins for safety

  // BH1750 (lux)
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("BH1750 initialized OK");
  } else {
    Serial.println("BH1750 ERROR - check wiring / I2C");
  }

  // VEML6070 (UV)
  if (uv.begin()) {
    uv.setIntegrationTime(VEML6070_1_T);   // 1T = ~40ms integration, good balance
    Serial.println("VEML6070 initialized OK");
  } else {
    Serial.println("VEML6070 ERROR - check wiring / I2C");
  }

  // ─── BLE setup ────────────────────────────────
  BLEDevice::init("PMSG BitChat Glasses");
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE |
    BLECharacteristic::PROPERTY_NOTIFY
  );

  pCharacteristic->addDescriptor(new BLE2902());
  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("PMSG ready w/ sensors");

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();

  Serial.println("BLE advertising... Use nRF Connect → write /lux or /uv");
}

void loop() {
  // Button press → short buzz + onboard LED flash
  bool buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW && oldButtonState == HIGH) {
    Serial.println("Button → buzz + LED flash");
    analogWrite(VIB_PIN, 180);
    digitalWrite(ONBOARD_LED_PIN, LOW);   // On
    delay(150);
    analogWrite(VIB_PIN, 0);
    digitalWrite(ONBOARD_LED_PIN, HIGH);  // Off

    // Notify if someone is connected
    if (pCharacteristic->getSubscribedCount() > 0) {
      pCharacteristic->setValue("Button pressed");
      pCharacteristic->notify();
    }
    delay(60);  // debounce
  }
  oldButtonState = buttonState;

  delay(30);
}
