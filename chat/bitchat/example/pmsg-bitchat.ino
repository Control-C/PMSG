#include <NimBLEDevice.h>

// Nordic UART Service (NUS) for prototyping
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define RX_UUID      "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"  // Write / WriteNR
#define TX_UUID      "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"  // Notify

// XIAO ESP32C6 pins
#define LED_PIN    LED_BUILTIN   // GPIO15 - active LOW
#define BUTTON_PIN 9             // D9 = GPIO20 - pull-up, press = LOW

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

// Updated callback class (new signature + connInfo)
class RxCallbacks : public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
    std::string rxValue = pCharacteristic->getValue();
    if (rxValue.empty()) return;

    Serial.printf("RX (%d bytes) from %s: ", rxValue.size(), connInfo.getAddress().toString().c_str());
    for (uint8_t b : rxValue) Serial.printf("%02X ", b);
    Serial.println();

    forwardBuffer = rxValue;   // Store for relay/forward

    blinkLED(2);

    // Echo back to the sender (for testing)
    pTxChar->setValue(rxValue);
    pTxChar->notify(true);     // true = notify all connected clients
  }
};

// New scan callback class (replaces old AdvertisedDeviceCallbacks)
class ScanCallbacks : public NimBLEScanCallbacks {
  void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override {
    if (advertisedDevice->isAdvertisingService(NimBLEUUID(SERVICE_UUID))) {
      Serial.print("Found NUS peer: ");
      Serial.print(advertisedDevice->toString().c_str());
      Serial.printf(" RSSI: %d\n", advertisedDevice->getRSSI());

      // Optional: connect if not already connected (add logic to avoid flooding)
      // For simple test/demo: just log
      blinkLED(1, 50);
    }
  }

  void onScanEnd(const NimBLEScanResults& scanResults, int reason) override {
    Serial.printf("Scan ended, reason: %d\n", reason);
    doScan = true;  // Trigger restart in loop
  }
};

void setup() {
  Serial.begin(115200);
  delay(400);
  Serial.println("\n=== NUS Dual-Role Relay Prototype (NimBLE 2.x+) ===");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  // LED off

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  NimBLEDevice::init("Relay-XIAO");
  NimBLEDevice::setPower(9);   // Max TX power (ESP32-C6 supports it)

  // ── Peripheral / Server ───────────────────────────────
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

  // Advertising
  NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setAppearance(0x00);           // Optional
  pAdvertising->enableScanResponse(true);      // ← Fixed: was setScanResponse
  pAdvertising->start();
  Serial.println("Peripheral advertising started");

  // ── Central / Scanning ────────────────────────────────
  pScan = NimBLEDevice::getScan();
  pScan->setScanCallbacks(new ScanCallbacks(), true);  // ← Fixed: setScanCallbacks + wantDuplicates
  pScan->setActiveScan(true);
  pScan->setInterval(97);
  pScan->setWindow(97);
}

void loop() {
  // Button: toggle advertising (example)
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(50);  // debounce
    if (digitalRead(BUTTON_PIN) == LOW) {
      if (NimBLEDevice::getAdvertising()->isAdvertising()) {
        NimBLEDevice::stopAdvertising();
        Serial.println("Advertising stopped");
        blinkLED(4, 60);
      } else {
        NimBLEDevice::startAdvertising();
        Serial.println("Advertising restarted");
        blinkLED(1, 200);
      }
      while (digitalRead(BUTTON_PIN) == LOW) delay(10);
    }
  }

  // Restart scan when previous ends
  if (doScan && pScan && !pScan->isScanning()) {
    doScan = false;
    pScan->start(5, false);   // Scan 5 sec, non-blocking
    Serial.println("Started scan for NUS peers...");
  }

  // Simple forward (expand this for real multi-peer relay)
  if (!forwardBuffer.empty()) {
    // In full relay: send to other connected clients or discovered peers
    // For now: already echoed in onWrite
    forwardBuffer.clear();
  }

  delay(200);
}
