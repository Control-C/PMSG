# PMSG + Burning Mesh

Simple setup for **Prototype Modular Smart Glasses** (PMSG) on the Burning Mesh (Meshtastic).

Works with both versions:

- **ESP32** → Seeed XIAO ESP32S3 + Wio-SX1262 (868 / 915 MHz)
- **nRF** → Seeed XIAO nRF52840 + Wio-SX1262

Hack for playa Mesh burn 

| Function | XIAO pin | GPIO |Where to set it in APP
|------|---------------|-------------|
| 4× WS2812C LED | D1 | **2** |External Notification → Output
| Buzzer | D2 | **3** |Device → PWM Buzzer and External Notification → Output Buzzer


## Pin Map

| Function | XIAO pin | GPIO | Where to set it in the App |
| --- | --- | --- | --- |
| 4× WS2812C LED | D1 | 2 | External Notification → Output |
| Buzzer | D2 | 3 | Device → PWM Buzzer<br>External Notification → Output Buzzer |
| I²C SDA | D4 | 5 | Automatic (Grove) |
| I²C SCL | D5 | 6 | Automatic (Grove) |

## Sensors (I²C – Grove 3.3 V only)

| Sensor | Address | What it does | Stock firmware support |
| --- | --- | --- | --- |
| VEML7700-TR | 0x10 | Ambient light (lux) | Yes – automatic |
| LTR390 | 0x53 | UV + ambient light | Yes – automatic |
| LIS2DH12 | 0x19 | Accelerometer / tap | Limited* |

\* LIS2DH12 is not fully supported in stock Meshtastic for tap detection.  
VEML7700 and LTR390 work automatically when Telemetry is enabled.

## Custom firmware needed?

| Feature | Stock firmware | Custom needed |
| --- | --- | --- |
| LED blink + Buzzer on message | Yes | No |
| Button actions | Yes | No |
| VEML7700 + LTR390 readings | Yes | No |
| Full RGB colors / animations | No | Yes |
| LIS2DH12 tap → send message | No | Yes |


---

## What you need

| Item | ESP32 version | nRF version |
|------|---------------|-------------|
| Board | XIAO ESP32S3 + Wio-SX1262 | XIAO nRF52840 + Wio-SX1262 |
| Antenna | U.FL antenna (included) | U.FL antenna (included) |
| LEDs | 4× WS2812C-2020-V1 on **D1** | Same |
| Button | **D3 / NC** | Same |
| Vibration motor | **D2** (PWM) | Same |
| Sensors | Grove I²C @ 3.3 V only | Same |

---

## 1. Flash the firmware (easiest way)

### ESP32-S3 version
1. Open **https://burn.meshtastic.org** (or https://flasher.meshtastic.org) in Chrome/Edge
2. Connect XIAO ESP32S3 with a **data** USB-C cable
3. Hold **BOOT** while plugging in (or while pressing Reset)
4. Select device → **Seeed Xiao ESP32-S3**
5. Choose latest Burning Mesh / community firmware
6. Flash (use Full Erase if you want a clean start)

### nRF52840 version
1. Same flasher → **https://burn.meshtastic.org**
2. Connect XIAO nRF52840
3. Double-click **Reset** → drive `XIAO SENSE` or `NRF52BOOT` appears
4. Select device → **Seeed Xiao NRF52840 Kit**
5. Download the `.uf2` and drag it onto the drive
6. Board reboots by itself

**Always connect the LoRa antenna before powering on.**

---

## 2. First connection

1. Open the Meshtastic app (Android / iOS)
2. Pair over Bluetooth (PIN is often `123456`)
3. Set **Region** (EU868 / US915 / …)
4. Give the node a short name (example: `PMSG-01`)
5. Save and reboot

Your glasses are now on the Burning Mesh.

---

## 3. PMSG pins (app only – no code needed)

Go to **Settings → Modules → External Notification / GPIO**

Playa version

| Function              | Pin  | Setting                          |
|-----------------------|------|----------------------------------|
| 4-pixel WS2812C       | D1   | External Notification or later custom |
| Button                | nc   | Input Pull-Up, Active LOW        |
| Vibration motor       | D2  | PWM, Duty 150–200                |
| I²C sensors (Grove)   | SDA/SCL | Automatic telemetry           |

**Button actions** (Settings → Device → Button Actions)  
- Short press → Send position or canned message  
- Long press → your choice  

**Sensors**  
Plug SHT31 / VEML7700 / LTR390 into the 3.3 V Grove port.  
They show up automatically under Telemetry.

---

## 4. Folder structure (what to keep in this repo)

```
Meshtastic/
├── README.md                 ← this file
├── BurningMesh/              ← notes, camp keys, extra configs
├── pins.md                   ← exact pin table for your PCB
└── images/                   ← photos of your build
```

You do **not** need to compile anything for basic Burning Mesh + button + motor + sensors.

Only when you want full RGB animations (or 30 extra LEDs + BLE reactive effects) you need custom firmware.

---

## 5. Useful links

- Burning Mesh docs: https://docs.burningmesh.org
- Flasher: https://burn.meshtastic.org
- Official Meshtastic: https://meshtastic.org
- Seeed XIAO ESP32S3 + Wio-SX1262: Seeed wiki
- Seeed XIAO nRF52840 + Wio-SX1262: Seeed wiki

---

**That’s it.**  
Flash → pair → set region → configure the three pins in the app → done.

For custom LED animations or extra LEDs on RX (GPIO44) just open an issue or ask.
