// The single-board computer for on you face (PMSG) by Paul Mooij
// Made by ChatGPT 4o
// Use AI-Setupguide to udpate this code or let Agent help you out

// simple test via USB to check onboard sensors and buttons
// *( A0 not connected, also 2,4Ghz ant. for wifi - BLE
// check pmsg vesion and tech via AI-Setupguide

// TODO
// 1.1 You can add OLED display to make GoogleGlass style interface via USB
// 1.2 Add Wifi / BLE
// 2. test the A0 for batt use

#include <Arduino.h>
#include <Wire.h>
#include <BH1721FVC.h>  // Light sensor library
#include <ClosedCube_HDC1080.h>  // Humidity & Temperature sensor library
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

// Sensor and pin definitions
BH1721FVC luxSensor;
ClosedCube_HDC1080 hdc1080;
// pmsg version 3 only 
const int button1Pin = D8;   // Button 1
const int button2Pin = D10;  // Button 2 only v2 and v3 !!!  v4 andh higher this is vibration motor 
const int batteryPin = A0;   // Battery voltage monitor

// Variables to store sensor data
float lux = 0.0;
float temperature = 0.0;
float humidity = 0.0;
int button1State = 0;
int button2State = 0;

// OLED display setup (assuming it uses I2C on Wire)
U8G2_SSD1306_128X64_ALT0_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// String to hold incoming serial data
String incomingData = "";

// Function to read battery voltage level
float readBatteryLevel() {
  int sensorValue = analogRead(batteryPin);
  float voltage = sensorValue * (3.3 / 1023.0); // Assuming 3.3V reference
  return voltage;
}

void setup() {
    Serial.begin(115200);  // Start serial communication
    Wire.begin();          // Initialize I2C
    
    // Initialize sensors
    luxSensor.begin();
    hdc1080.begin(0x40);

    // Initialize buttons as input
    pinMode(button1Pin, INPUT);
    pinMode(button2Pin, INPUT);

    // Initialize OLED display
    u8g2.begin();

    delay(500);
    Serial.println("Startup");

    // Initial OLED Display
    u8g2.clearBuffer();                   // Clear the internal memory
    u8g2.setFont(u8g2_font_ncenB08_tr);   // Choose a font
    u8g2.drawStr(0,10,"Starting up...");  // Initial message
    u8g2.sendBuffer();                    // Transfer to display
}

void loop() {
    // Read light level from BH1721FVC sensor
    lux = luxSensor.readLightLevel();
    
    // Read temperature and humidity from HDC1080
    temperature = hdc1080.readTemperature();
    humidity = hdc1080.readHumidity();
    
    // Read button states
    button1State = digitalRead(button1Pin);
    button2State = digitalRead(button2Pin);
    
    // Read battery voltage
    float batteryVoltage = readBatteryLevel();
    
    // Check if serial data is available
    if (Serial.available() > 0) {
        char receivedChar = Serial.read();
        // If newline or carriage return, process the data
        if (receivedChar == '\n' || receivedChar == '\r') {
            // Display received string on OLED
            u8g2.clearBuffer();
            u8g2.setFont(u8g2_font_ncenB08_tr);
            u8g2.drawStr(0,10, incomingData.c_str());  // Display the received string
            u8g2.sendBuffer();

            // Clear the string after displaying
            incomingData = "";
        } else {
            // Append the character to the string
            incomingData += receivedChar;
        }
    }

    // Send sensor data via serial
    Serial.print("Lux: "); Serial.print(lux); Serial.print(" lx, ");
    Serial.print("Temp: "); Serial.print(temperature); Serial.print(" C, ");
    Serial.print("Humidity: "); Serial.print(humidity); Serial.print(" %, ");
    Serial.print("Battery: "); Serial.print(batteryVoltage); Serial.print(" V, ");
    
    if (button1State == HIGH) {
        Serial.print("Button1: pressed, ");
    }
    
    if (button2State == HIGH) {
        Serial.print("Button2: pressed, ");
    }
    
    Serial.println();

    delay(1000);  // Delay for readability, adjust as needed
}
