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

// Bitchat UUIDs (werkt voor discovery)
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

// Commando struct + array (makkelijk uitbreiden!)
struct TriggerAction {
  const char* cmd;
  CRGB color;          // vaste kleur (Black = animatie)
  bool rainbow;
  int buzzerBeeps;
  int buzzerMs;
};

const TriggerAction triggers[] = {
  {"RED-PMSG", CRGB::Red,   false, 2, 120},
  {"BLU-PMSG", CRGB::Blue,  false, 3,  80},
  {"PMSG",     CRGB::Black, true,  4, 100}   // rainbow
  // Voeg toe: {"GRN-PMSG", CRGB::Green, false, 1, 200},
};

const int numTriggers = sizeof(triggers) / sizeof(triggers[0]);

// RxCallbacks – alleen speciale commando's triggeren LEDs/buzzer
class RxCallbacks : public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
    std::string rxValue = pCharacteristic->getValue();
    if (rxValue.empty()) return;

    Serial.printf("RX (%d bytes) from %s: ", rxValue.size(), connInfo.getAddress().toString().c_str());
    for (uint8_t b : rxValue) Serial.printf("%02X ", b);
    Serial.println();

    bool handled = false;

    for (int i = 0; i < numTriggers; i++) {
      const char* cmd = triggers[i].cmd;
      size_t cmdLen = strlen(cmd);

      if (rxValue.size() >= cmdLen) {
        for (size_t pos = 0; pos <= rxValue.size() - cmdLen; ++pos) {
          if (memcmp(&rxValue[pos], cmd, cmdLen) == 0) {
            handled = true;

            if (triggers[i].rainbow) {
              Serial.printf("→ '%s' → Rainbow!\n", cmd);
              rainbowCycle();
            } else {
              Serial.printf("→ '%s' → kleur %s\n", cmd, cmd);
              fill_solid(ws_leds, NUM_WS2812, triggers[i].color);
              FastLED.show();
            }

            // Buzzer
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
        if (handled) break;
      }
    }

    // Geen fallback meer – alleen speciale woorden doen iets
    if (!handled) {
      Serial.println("→ Normaal/encrypted bericht – geen actie");
    }

    forwardBuffer = rxValue;
    pTxChar->setValue(rxValue);
    pTxChar->notify(true);
  }
};

// ScanCallbacks – bij peer gevonden: kort blauw
class ScanCallbacks : public NimBLEScanCallbacks {
  void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override {
    if (advertisedDevice->isAdvertisingService(NimBLEUUID(SERVICE_UUID))) {
      Serial.print("Found Bitchat peer: ");
      Serial.print(advertisedDevice->toString().c_str());
      Serial.printf(" RSSI: %d\n", advertisedDevice->getRSSI());

      fill_solid(ws_leds, NUM_WS2812, CRGB::Blue);
      FastLED.show();
      delay(800);                     // kort blauw tonen
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
  Serial.println("\n=== Bitchat Relay – LEDs blauw bij start/peer, alleen speciale commando's ===");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  FastLED.addLeds<WS2812B, LED_PIN_D1, GRB>(ws_leds, NUM_WS2812);
  FastLED.setBrightness(80);

  // Opstart: LEDs blauw kort
  fill_solid(ws_leds, NUM_WS2812, CRGB::Blue);
  FastLED.show();
  delay(1200);
  fill_solid(ws_leds, NUM_WS2812, CRGB::Black);
  FastLED.show();

  Serial.println("FastLED + Buzzer init OK – blauw bij start");

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
