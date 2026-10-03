// PMSG BLE-PS example — 📵 📷 📸 🎤
// Prototype Modular Smart Glasses. Preference protocol, not a kill switch.
// Spec: https://github.com/Control-C/PMSG/tree/main/BLE-bluetooth-privacy-signal
//
// Board: XIAO ESP32C6 (or S3) in the PMSG arm.
// D1  = 4x WS2812 data
// D9  = button (short = cycle state, long = clear 📵 latch)
// D10 = vibration motor
//
// Needs NimBLE-Arduino. Pixel timing is bit-banged so FastLED is optional.

#include <NimBLEDevice.h>

static const NimBLEUUID kSvc("0000f1a0-0000-1000-8000-00805f9b34fb");

static const int PIN_LED = D1;
static const int PIN_BTN = D9;
static const int PIN_VIB = D10;
static const int N_PIX = 4;

// bit 0 camera, 1 camera_active, 2 has_mic, 3 audio_active, 4 honors_dnr, 5 dnr_requested
static const uint8_t BITS_IDLE = 0x11;   // 📷
static const uint8_t BITS_REC  = 0x13;   // 📸
static const uint8_t BITS_MIC  = 0x1C;   // 🎤
static const uint8_t BITS_AV   = 0x1F;   // 📸🎤
static const uint8_t BITS_DNR  = 0x20;   // 📵

enum State { ST_IDLE, ST_REC, ST_AV, ST_MIC };
static State g_state = ST_IDLE;
static bool g_dnr = false;
static bool g_dnr_override = false;

static const char* stateName(State s) {
  switch (s) {
    case ST_REC: return "PMSG_📸";
    case ST_AV:  return "PMSG_📸🎤";
    case ST_MIC: return "PMSG_🎤";
    default:     return "PMSG_📷";
  }
}

static uint8_t stateBits(State s) {
  switch (s) {
    case ST_REC: return BITS_REC;
    case ST_AV:  return BITS_AV;
    case ST_MIC: return BITS_MIC;
    default:     return BITS_IDLE;
  }
}

static void buzz(int ms) {
  digitalWrite(PIN_VIB, HIGH);
  delay(ms);
  digitalWrite(PIN_VIB, LOW);
}

// WS2812, GRB, one byte. Short enough for a demo. Not for a tight sensor loop.
static void pixByte(uint8_t v) {
  for (int i = 7; i >= 0; --i) {
    if (v & (1 << i)) {
      digitalWrite(PIN_LED, HIGH);
      delayMicroseconds(1);
      digitalWrite(PIN_LED, LOW);
    } else {
      digitalWrite(PIN_LED, HIGH);
      delayMicroseconds(0);
      digitalWrite(PIN_LED, LOW);
      delayMicroseconds(1);
    }
  }
}

static void show(uint8_t r, uint8_t g, uint8_t b) {
  noInterrupts();
  for (int i = 0; i < N_PIX; ++i) {
    pixByte(g);
    pixByte(r);
    pixByte(b);
  }
  interrupts();
  delayMicroseconds(80);
}

static void paint() {
  if (g_dnr && !g_dnr_override) {
    show(40, 24, 0);  // amber — 📵 heard
    return;
  }
  switch (g_state) {
    case ST_REC: show(40, 0, 0); break;
    case ST_AV:  show(40, 0, 24); break;
    case ST_MIC: show(24, 0, 40); break;
    default:     show(8, 8, 8); break;  // 📷 idle
  }
}

static void startAdv() {
  uint8_t bits = stateBits(g_state);
  NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
  adv->stop();
  NimBLEAdvertisementData data;
  data.setName(stateName(g_state));
  data.setServiceData(kSvc, std::string((char*)&bits, 1));
  adv->setAdvertisementData(data);
  adv->addServiceUUID(kSvc);
  adv->start();
  Serial.printf("adv %s bits 0x%02X dnr=%d\n", stateName(g_state), bits, g_dnr);
}

class ScanCb : public NimBLEScanCallbacks {
  void onResult(const NimBLEAdvertisedDevice* d) override {
    std::string name = d->getName();
    bool hit = name.find("📵") != std::string::npos || name.find("DNR") != std::string::npos;
    if (d->haveServiceData()) {
      std::string sd = d->getServiceData(kSvc);
      if (!sd.empty() && (sd[0] & 0x20)) hit = true;
    }
    if (!hit || g_dnr) return;
    g_dnr = true;
    if (g_state != ST_IDLE) {
      g_state = ST_IDLE;  // do not keep a new clip open
      startAdv();
    }
    Serial.println("📵 heard — no new clip");
    buzz(60);
    delay(40);
    buzz(60);
    paint();
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(PIN_BTN, INPUT_PULLUP);
  pinMode(PIN_VIB, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_VIB, LOW);
  paint();

  NimBLEDevice::init("PMSG_📷");
  NimBLEDevice::getScan()->setScanCallbacks(new ScanCb(), true);
  NimBLEDevice::getScan()->setInterval(120);
  NimBLEDevice::getScan()->setWindow(40);
  NimBLEDevice::getScan()->setActiveScan(true);
  NimBLEDevice::getScan()->start(0, false, true);
  startAdv();
  buzz(40);
}

void loop() {
  static uint32_t downAt = 0;
  static bool held = false;
  bool down = digitalRead(PIN_BTN) == LOW;

  if (down && !downAt) downAt = millis();
  if (down && downAt && !held && millis() - downAt > 1200) {
    held = true;
    g_dnr_override = true;
    Serial.println("D9 long — wearer override, stamp still dnr_present");
    buzz(120);
    paint();
  }
  if (!down && downAt) {
    if (!held && millis() - downAt > 40) {
      if (g_dnr && !g_dnr_override && g_state == ST_IDLE) {
        Serial.println("refused — 📵 present");
        buzz(30);
      } else {
        g_state = (State)((g_state + 1) % 4);
        if (g_dnr && !g_dnr_override) g_state = ST_IDLE;
        startAdv();
        paint();
        buzz(25);
      }
    }
    downAt = 0;
    held = false;
  }

  // Pulse the bar while 📸 so the arm shows recording without a screen.
  if (g_state == ST_REC || g_state == ST_AV) {
    show((millis() / 400) % 2 ? 48 : 8, 0, g_state == ST_AV ? 20 : 0);
  }
  delay(20);
}
