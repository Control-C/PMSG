// PMSG v4 esp32C6 Bitchat Relay Prototype met FastLED WS2812
// working code only string will not be shown in chat... working on this 
#include <NimBLEDevice.h>
#include <FastLED.h>

// WS2812 LEDs op D1 = GPIO1
#define LED_PIN_D1     1
#define NUM_WS2812     4
CRGB ws_leds[NUM_WS2812];

// Buzzer op D10 = GPIO18 (XIAO ESP32C6)
#define BUZZER_PIN     18

// Bitchat UUIDs
#define SERVICE_UUID   "f47b5e2d-4a9e-4c5a-9b3f-8e1d2c3a4b5c"
#define RX_UUID        "a1b2c3d4-e5f6-4a5b-8c9d-0e1f2a3b4c5d"
#define TX_UUID        "a1b2c3d4-e5f6-4a5b-8c9d-0e1f2a3b4c5d"

// Built-in LED & button
#define LED_PIN    LED_BUILTIN   // GPIO15 active LOW
#define BUTTON_PIN 9             // D9 = GPIO20

NimBLEServer*        pServer   = nullptr;
NimBLECharacteristic* pRxChar   = nullptr;
NimBLECharacteristic* pTxChar   = nullptr;

NimBLEScan*          pScan     = nullptr;
bool                 doScan    = true;
std::string          forwardBuffer;

// Blink helper
void blinkLED(int times = 1, int ms = 80) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, LOW);
    delay(ms);
    digitalWrite(LED_PIN, HIGH);
    delay(ms);
  }
}

// Rainbow animatie
void rainbowCycle(uint8_t wait = 10, uint8_t cycles = 3) {
  for (uint8_t cycle = 0; cycle < cycles; cycle++) {
    for (uint16_t j = 0; j < 256; j++) {
      for (int i = 0; i < NUM_WS2812; i++) {
        ws_leds[i] = CHSV(((i * 256 / NUM_WS2812) + j) & 255, 255, 255);
      }
      FastLED.show();
      delay(wait);
    }
  }
  fill_solid(ws_leds, NUM_WS2812, CRGB::Black);
  FastLED.show();
}

// RxCallbacks
class RxCallbacks : public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
    std::string rxValue = pCharacteristic->getValue();
    if (rxValue.empty()) return;

    Serial.printf("RX (%d bytes) from %s: ", rxValue.size(), connInfo.getAddress().toString().c_str());
    for (uint8_t b : rxValue) Serial.printf("%02X ", b);
    Serial.println();

    // Maak lowercase versie voor case-insensitive check
    std::string rxLower = rxValue;
    for (char& c : rxLower) c = tolower(c);

    bool handled = false;

    // === Speciale commando's voor LEDs / animaties ===
    struct TriggerAction {
      const char* cmdLower;
      CRGB color;
      bool rainbow;
      int buzzerBeeps;
      int buzzerMs;
    };

    const TriggerAction triggers[] = {
      {"red-pmsg",  CRGB::Red,   false, 2, 120},
      {"blu-pmsg",  CRGB::Blue,  false, 3,  80},
      {"pmsg",      CRGB::Black, true,  4, 100},
      {"grn-pmsg",  CRGB::Green, false, 1, 200}
    };
    const int numTriggers = sizeof(triggers) / sizeof(triggers[0]);

    for (int i = 0; i < numTriggers; i++) {
      if (rxLower.find(triggers[i].cmdLower) != std::string::npos) {
        handled = true;

        if (triggers[i].rainbow) {
          Serial.printf("→ '%s' → Rainbow!\n", triggers[i].cmdLower);
          rainbowCycle();
        } else {
          Serial.printf("→ '%s' → kleur %s\n", triggers[i].cmdLower, triggers[i].cmdLower);
          fill_solid(ws_leds, NUM_WS2812, triggers[i].color);
          FastLED.show();
        }

        for (int b = 0; b < triggers[i].buzzerBeeps; b++) {
          digitalWrite(BUZZER_PIN, HIGH);
          delay(triggers[i].buzzerMs);
          digitalWrite(BUZZER_PIN, LOW);
          delay(80);
        }

        blinkLED(2, 150);
        break;
      }
    }

    // === Sensor data reply commando's ===
    if (!handled) {
      String replyText = "hello";

      if (rxLower.find("uvmsg-data uv") != std::string::npos ||
          rxLower.find("uvmsg-data from uv") != std::string::npos) {
        // Dummy UV sensor waarde (vervang later door echte uitlezing)
        float uvIndex = 4.2 + (random(0, 30) / 10.0);  // varieert een beetje
        replyText = "[XIAO-UV] UV index: " + String(uvIndex, 1) + " | Raw: " + String(random(500, 1200));
      }
      else if (rxLower.find("tmsg-data temp") != std::string::npos ||
               rxLower.find("tmsg-data from temp") != std::string::npos) {
        float temp = 21.8 + (random(-15, 25) / 10.0);  // varieert
        replyText = "[XIAO-TEMP] Temperatuur: " + String(temp, 1) + " °C | Humidity: " + String(random(40, 75)) + "%";
      }
      else if (rxLower.find("d-data") != std::string::npos) {
        // Algemene sensor info
        float uv = 3.9;
        float temp = 22.5;
        replyText = "[XIAO-SENSOR] UV: " + String(uv, 1) + " | Temp: " + String(temp, 1) + " °C | Uptime: " + String(millis()/1000) + "s";
      }

      if (!replyText.isEmpty()) {
        handled = true;
        Serial.println("Sensor request! Reply: " + replyText);

        // Stuur reply via notify
        std::string replyBytes(replyText.c_str(), replyText.length());
        pTxChar->setValue(replyBytes);
        pTxChar->notify(true);

        // Feedback: cyan LEDs + buzzer
        fill_solid(ws_leds, NUM_WS2812, CRGB::Cyan);
        FastLED.show();
        delay(800);
        fill_solid(ws_leds, NUM_WS2812, CRGB::Black);
        FastLED.show();

        for (int i = 0; i < 2; i++) {
          digitalWrite(BUZZER_PIN, HIGH);
          delay(150);
          digitalWrite(BUZZER_PIN, LOW);
          delay(100);
        }
      }
    }

    // Log als er niets speciaals gebeurde
    if (!handled) {
      Serial.println("→ Normaal/encrypted bericht – geen actie");
    }

    forwardBuffer = rxValue;
    pTxChar->setValue(rxValue);
    pTxChar->notify(true);
  }
};

// ScanCallbacks – kort blauw bij peer gevonden
class ScanCallbacks : public NimBLEScanCallbacks {
  void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override {
    if (advertisedDevice->isAdvertisingService(NimBLEUUID(SERVICE_UUID))) {
      Serial.print("Found Bitchat peer: ");
      Serial.print(advertisedDevice->toString().c_str());
      Serial.printf(" RSSI: %d\n", advertisedDevice->getRSSI());

      fill_solid(ws_leds, NUM_WS2812, CRGB::Blue);
      FastLED.show();
      delay(800);
      fill_solid(ws_leds, NUM_WS2812, CRGB::Black);
      FastLED.show();
    }
  }

  void onScanEnd(const NimBLEScanResults& scanResults, int reason) override {
    Serial.printf("Scan ended, reason: %d\n", reason);
    doScan = true;
  }
};

void setup() {
  Serial.begin(115200);
  delay(400);
  Serial.println("\n=== Bitchat Relay met sensor reply op PMSG-data ===");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  FastLED.addLeds<WS2812B, LED_PIN_D1, GRB>(ws_leds, NUM_WS2812);
  FastLED.setBrightness(80);

  // Opstart: blauw kort
  fill_solid(ws_leds, NUM_WS2812, CRGB::Blue);
  FastLED.show();
  delay(1200);
  fill_solid(ws_leds, NUM_WS2812, CRGB::Black);
  FastLED.show();

  Serial.println("Init OK – blauw bij start");

  NimBLEDevice::init("Bitchat-XIAO-Relay");
  NimBLEDevice::setPower(9);

  pServer = NimBLEDevice::createServer();

  NimBLEService* pService = pServer->createService(SERVICE_UUID);

  pRxChar = pService->createCharacteristic(
    RX_UUID,
    NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
  );
  pRxChar->setCallbacks(new RxCallbacks());

  pTxChar = pService->createCharacteristic(
    TX_UUID,
    NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ
  );

  pService->start();

  NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setAppearance(0x00);
  pAdvertising->enableScanResponse(true);
  pAdvertising->start();
  Serial.println("Advertising gestart...");

  pScan = NimBLEDevice::getScan();
  pScan->setScanCallbacks(new ScanCallbacks(), true);
  pScan->setActiveScan(true);
  pScan->setInterval(97);
  pScan->setWindow(97);
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(50);
    if (digitalRead(BUTTON_PIN) == LOW) {
      auto* adv = NimBLEDevice::getAdvertising();
      if (adv->isAdvertising()) {
        adv->stop();
        Serial.println("Advertising gestopt");
      } else {
        adv->start();
        Serial.println("Advertising herstart");
      }
      while (digitalRead(BUTTON_PIN) == LOW) delay(10);
    }
  }

  if (doScan && pScan && !pScan->isScanning()) {
    doScan = false;
    pScan->start(5, false);
    Serial.println("Scan voor Bitchat peers gestart...");
  }

  if (!forwardBuffer.empty()) {
    forwardBuffer.clear();
  }

  delay(200);
}
