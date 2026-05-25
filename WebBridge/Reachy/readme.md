WIP ( still making it work ) 

source :https://github.com/pollen-robotics/reachy_mini

# PMSG Smart Glasses - Reachy Mini Controller 🕶️🤖

This project integrates the **PMSG (Prototype Modular Smart Glasses)** open-source hardware platform—powered by a **Seeed Studio XIAO ESP32-C6**—with the [Pollen Robotics Reachy Mini](https://github.com/pollen-robotics/reachy_mini) robot framework. 

The glasses track your head movements using an onboard I2C IMU (X, Y, Z axes) and stream real-time data via **BLE Gamepad emulation** or a **Wi-Fi JSON API**. It features environmental edge-sensing (UV/Lux), haptic feedback (Vibration), and a 4-pixel addressable RGB LED status bar.

---

## 🤖 AI-Agent Developer Framework (AIS / AGENTS.md)

This project strictly adheres to agentic-driven engineering. If you are using an AI developer tool (e.g., Claude Code, Cursor, Copilot), paste this initial prompt to automatically bootstrap software development:

> *"I am building an application linking PMSG Smart Glasses to a Reachy Mini robot. Read the local `README.md` for specific hardware configurations, then cross-reference with the official robot SDK patterns at https://github.com and the glasses configuration templates at https://github.com."*

---

## 🚀 Features

*   **Dual Mode Control**: Control Reachy Mini wirelessly via native BLE HID Gamepad or a Wi-Fi HTTP REST API.
*   **3-Axis Head Tracking**: Reads raw I2C sensor data to map head Pitch, Roll, and Yaw seamlessly to robot movements.
*   **Environmental Sensing**: Exposes real-time Ambient Light (Lux) and Ultra-Violet (UV) index values via API and Serial.
*   **Local Feedback**: Includes a 4-pixel WS2812B NeoPixel array for status displays and a vibration motor for haptic alerts.
*   **Developer Friendly**: Full telemetry and diagnostic controls mirrored over USB Serial at 115200 baud.

---

## 🛠️ Hardware Mapping (XIAO ESP32-C6)


| Component | Protocol / Pin | Description |
| :--- | :--- | :--- |
| **IMU / UV / Lux Sensors** | I2C (`SDA: LP_IO02 / SCL: LP_IO03`) | Core telemetry bus |
| **NeoPixel LED Array** | Digital Output (`D2 / GPIO21`) | 4-Pixel Status indicator |
| **Vibration Motor** | Digital Output (`D3 / GPIO22`) | Haptic feedback driver |
| **USB-C** | Native CDC Serial | Debugging and wired telemetry |

---

## 💻 Firmware Installation

### 1. Prerequisites
Open your Arduino IDE and install the following libraries via the Library Manager:
*   `ESP32-BLE-Gamepad` (by lewurm)
*   `Adafruit MPU6050` (or your specific I2C IMU library)
*   `Adafruit LTR390` (for UV and Lux)
*   `Adafruit NeoPixel`

### 2. ESP32-C6 Source Code
Flash the following code to your XIAO ESP32-C6. *Make sure to update your Wi-Fi credentials before uploading.*

```cpp
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_LTR390.h>
#include <Adafruit_NeoPixel.h>
#include <BleGamepad.h>

// --- Configurations ---
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

#define NUM_PIXELS    4
#define PIN_NEOPIXEL  D2
#define PIN_VIBE      D3

// --- Object Initializations ---
BleGamepad bleGamepad("PMSG Reachy Controller", "DIY", 100);
WebServer server(80);
Adafruit_MPU6050 mpu;
Adafruit_LTR390 ltr = Adafruit_LTR390();
Adafruit_NeoPixel pixels(NUM_PIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

// --- Global Telemetry Data ---
float roll = 0, pitch = 0, yaw = 0;
float lux = 0;
uint32_t uv = 0;

void handleRootAPI() {
  String json = "{";
  json += "\"head\":{\"roll\":" + String(roll) + ",\"pitch\":" + String(pitch) + ",\"yaw\":" + String(yaw) + "},";
  json += "\"environment\":{\"lux\":" + String(lux) + ",\"uv_index\":" + String(uv) + "}";
  json += "}";
  server.send(200, "application/json", json);
}

void handleVibeAPI() {
  if (server.hasArg("duration")) {
    int ms = server.arg("duration").toInt();
    digitalWrite(PIN_VIBE, HIGH);
    delay(ms);
    digitalWrite(PIN_VIBE, LOW);
    server.send(200, "text/plain", "Vibrated for " + String(ms) + "ms");
  } else {
    server.send(400, "text/plain", "Missing 'duration' parameter");
  }
}

void handleLedAPI() {
  if (server.hasArg("r") && server.hasArg("g") && server.hasArg("b")) {
    int r = server.arg("r").toInt();
    int g = server.arg("g").toInt();
    int b = server.arg("b").toInt();
    for(int i=0; i<NUM_PIXELS; i++) pixels.setPixelColor(i, pixels.Color(r, g, b));
    pixels.show();
    server.send(200, "text/plain", "LEDs Updated");
  } else {
    server.send(400, "text/plain", "Missing r, g, or b parameters");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_VIBE, OUTPUT);
  pixels.begin();
  pixels.clear();
  
  // Initialize I2C (Uses standard ESP32-C6 Wire pins)
  Wire.begin();
  
  if (!mpu.begin()) Serial.println("⚠️ MPU6050 missing!");
  if (!ltr.begin()) Serial.println("⚠️ LTR390 missing!");

  // Connect Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nConnected! IP Address: " + WiFi.localIP().toString());

  // Setup API Endpoints
  server.on("/api/telemetry", HTTP_GET, handleRootAPI);
  server.on("/api/vibrate", HTTP_POST, handleVibeAPI);
  server.on("/api/led", HTTP_POST, handleLedAPI);
  server.initFromMethod(HTTP_OPTIONS, [&](){ server.send(200); });
  server.begin();

  // Start BLE Gamepad
  bleGamepad.begin();
}

void loop() {
  server.handleClient();
  
  // Read IMU
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  
  // Calculate raw movements (X/Y/Z translation)
  pitch = atan2(-a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 57.295;
  roll  = atan2(a.acceleration.y, a.acceleration.z) * 57.295;
  yaw   += g.gyro.z * 0.05; // Quick Integration approximation for Z axis

  // Read Environment Sensors
  if (ltr.newDataAvailable()) {
    lux = ltr.readLUX();
    uv = ltr.readUVS();
  }

  // Handle BLE Output Mapping (Targets Right Analog Stick for Reachy Dashboard)
  if (bleGamepad.isConnected()) {
    int16_t joyX = map(constrain(roll, -45, 45), -45, 45, -32767, 32767);
    int16_t joyY = map(constrain(pitch, -45, 45), -45, 45, -32767, 32767);
    bleGamepad.setRightThumb(joyX, joyY);
  }

  // Serial Debug Output
  Serial.printf("[TELEMETRY] Pitch: %.2f | Roll: %.2f | Yaw: %.2f | Lux: %.1f | UV: %d | BLE_CONN: %d\n", 
                pitch, roll, yaw, lux, uv, bleGamepad.isConnected());
  delay(50);
}
```

---

## 🍏 Integration on macOS

### Option A: Direct Dashboard Integration (BLE Gamepad)
1. Go to **System Settings > Bluetooth** on your Mac.
2. Select and connect to **"PMSG Reachy Controller"**.
3. Open your **Reachy Mini Simulation Dashboard**. 
4. The default gamepad indicator will turn green. The right stick emulation maps directly to the robot's physical head servos.

### Option B: Python Control Loop Integration (Wi-Fi API)
This asynchronous Python script handles teleoperation loops matching the framework specifications of Reachy Mini's `AGENTS.md`:

```python
import requests
import time
from reachy_mini import ReachyMini
from reachy_mini.utils import create_head_pose

GLASSES_IP = "http://<YOUR_GLASSES_IP>" # Check Serial monitor for address
UPDATE_RATE_HZ = 20.0

print("🚀 Initializing PMSG to Reachy Mini Teleoperation Loop...")

with ReachyMini() as mini:
    while True:
        loop_start = time.time()
        try:
            # 1. Fetch telemetry from glasses
            response = requests.get(f"{GLASSES_IP}/api/telemetry", timeout=0.05).json()
            head_data = response["head"]
            env_data = response["environment"]

            # 2. Extract spatial coordinates
            target_pitch = head_data["pitch"]
            target_roll = head_data["roll"]

            # Trigger haptic buzz and set LEDs to warning Red if light levels get too bright!
            if env_data["lux"] > 5000.0:
                requests.post(f"{GLASSES_IP}/api/vibrate?duration=100")
                requests.post(f"{GLASSES_IP}/api/led?r=255&g=0&b=0")
            else:
                # Set LEDs to green operational state
                requests.post(f"{GLASSES_IP}/api/led?r=0&g=255&b=0")

            # 3. Direct robot command
            mini.goto_target(
                head=create_head_pose(roll=target_roll, pitch=target_pitch, degrees=True),
                duration=1.0 / UPDATE_RATE_HZ
            )
        except Exception as e:
            print("Sync error or packet dropped:", e)
        
        # Enforce constant control loop frequency
        elapsed = time.time() - loop_start
        sleep_time = max(0, (1.0 / UPDATE_RATE_HZ) - elapsed)
        time.sleep(sleep_time)
```
