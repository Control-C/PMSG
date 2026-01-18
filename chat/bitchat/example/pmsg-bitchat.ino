// PMSG v4 esp32C6 Bitchat Relay Prototype met FastLED WS2812
// working code only string will not be shown in chat... working on this 
#include <NimBLEDevice.h>
#include <FastLED.h>

// WS2812 LEDs op D1 = GPIO1
#define LED_PIN_D1     1
#define NUM_WS2812     4
CRGB ws_leds[NUM_WS2812];

// Buzzer op D10 = GPIO18 (XIAO ESP32C6 pinout)
#define BUZZER_PIN     18         //  GPIO voor D10

// Bitchat custom UUIDs (werkt voor discovery)
#define SERVICE_UUID   "f47b5e2d-4a9e-4c5a-9b3f-8e1d2c3a4b5c"
#define RX_UUID        "a1b2c3d4-e5f6-4a5b-8c9d-0e1f2a3b4c5d"  // Write
#define TX_UUID        "a1b2c3d4-e5f6-4a5b-8c9d-0e1f2a3b4c5d"  // Notify

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

// RxCallbacks: kleuren cyclisch per bericht + buzzer piep + test 'r'/'b'
class RxCallbacks : public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
    std::string rxValue = pCharacteristic->getValue();
    if (rxValue.empty()) return;

    Serial.printf("RX Bitchat bericht (%d bytes) from %s: ", rxValue.size(), connInfo.getAddress().toString().c_str());
    for (uint8_t b : rxValue) Serial.printf("%02X ", b);
    Serial.println();

    Serial.println("→ Bericht ontvangen! LEDs veranderen kleur + buzzer piept");

    // Cyclisch kleur per bericht
    static uint8_t colorIndex = 0;
    CRGB colors[] = {CRGB::Red, CRGB::Blue, CRGB::Green, CRGB::Yellow, CRGB::Purple, CRGB::Orange};
    fill_solid(ws_leds, NUM_WS2812, colors[colorIndex % 6]);
    FastLED.show();
    colorIndex++;

    // Buzzer: korte piep (HIGH = aan voor active buzzer)
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);

    // Test fallback: als plaintext 'r' of 'b' (voor nRF Connect test)
    bool hasR = false, hasB = false;
    for (uint8_t b : rxValue) {
      if (b == 'r' || b == 'R') hasR = true;
      if (b == 'b' || b == 'B') hasB = true;
    }
    if (hasR) {
      fill_solid(ws_leds, NUM_WS2812, CRGB::Red);
      FastLED.show();
      Serial.println("Test 'R' gedetecteerd → force ROOD");
    }
    if (hasB) {
      fill_solid(ws_leds, NUM_WS2812, CRGB::Blue);
      FastLED.show();
      Serial.println("Test 'B' gedetecteerd → force BLAUW");
    }

    forwardBuffer = rxValue;

    pTxChar->setValue(rxValue);
    pTxChar->notify(true);

    blinkLED(2, 150);
  }
};

// ScanCallbacks
class ScanCallbacks : public NimBLEScanCallbacks {
  void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override {
    if (advertisedDevice->isAdvertisingService(NimBLEUUID(SERVICE_UUID))) {
      Serial.print("Found Bitchat peer: ");
      Serial.print(advertisedDevice->toString().c_str());
      Serial.printf(" RSSI: %d\n", advertisedDevice->getRSSI());
      blinkLED(1, 50);
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
  Serial.println("\n=== Bitchat Relay met FastLED + Buzzer op D10 ===");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  // off

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Buzzer init
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);  // Uit

  // FastLED
  FastLED.addLeds<WS2812B, LED_PIN_D1, GRB>(ws_leds, NUM_WS2812);
  FastLED.setBrightness(80);
  fill_solid(ws_leds, NUM_WS2812, CRGB::Black);
  FastLED.show();
  Serial.println("FastLED + Buzzer init OK");

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
        blinkLED(4, 60);
      } else {
        adv->start();
        Serial.println("Advertising herstart");
        blinkLED(1, 200);
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
