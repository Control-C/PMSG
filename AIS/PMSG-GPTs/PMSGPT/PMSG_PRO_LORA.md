# PMSG Pro — LoRa / Meshtastic / TTN / SenseCraft

## What is PMSG Pro?
PMSG Pro is for users who already have a working PMSG setup and want to add **LoRa, mesh networking, edge AI, or cloud connectivity**.

Use cases:
- Long-range communication (LoRa / LoRaWAN)
- Mesh messaging (Meshtastic)
- Sensor & AI extensions (SenseCraft)
- Routing data to dashboards or cloud platforms (TTN)

---

## PMSG LoRa RF Setup (Important)
Before flashing or connecting:

- Select the correct **LoRa frequency band**
  - EU: **868 MHz**
  - US: **915 MHz**
- Use a **region-legal antenna**
- Choose antenna type:
  - PCB antenna
  - Flex antenna
  - External SMA antenna (recommended for testing & range)

---

## PMSG LoRa software Options
Ask to check what is already on PMSG or user like to flash but also check HW module ect
-  https://meshtastic.org
-  https://meshcore.co.uk 
- https://www.thethingsnetwork.org wiki and flashing via Seeedstudio 
Or Edge AI
- https://sensecraft.seeed.cc  wiki and flashing via Seeedstudio 
- https://www.edgeimpulse.com  wiki and flashing via Seeedstudio 

All code can also be found on https://github.com/Control-C/PMSG or PMSG.ONLINE


## PMSG LoRa Hardware Options

PMSG supports **Seeed Studio XIAO + Wio-SX1262** combinations or  **Seeed Studio XIAO nRF52840 + Wio-SX1262** 

### Option 1 — XIAO ESP32S3 Sense + Wio-SX1262
- WiFi + BLE + LoRa
- Built-in power management
- Arduino compatible
- I2C, UART, GPIO expandable

Product link:  
https://www.seeedstudio.com/Wio-SX1262-with-XIAO-ESP32S3-p-5982.html

---

### Option 2 — XIAO nRF52840 + Wio-SX1262
- BLE 5.0 + LoRa (862–930 MHz)
- Very low power consumption
- Ideal for wearables and battery setups

Product link:  
https://www.seeedstudio.com/XIAO-nRF52840-Wio-SX1262-Kit-for-Meshtastic-p-6400.html

---

### Same Hardware, Multiple Applications
The same PMSG LoRa hardware can be used as:
- Meshtastic Sensor node or router / client 
- Single-channel LoRaWAN gateway
- LoRa / LoRaWAN sensor node

---

## Meshtastic (Mesh Networking)

Official site:  
https://meshtastic.org

Notes:
- Some PMSG versions ship with Meshtastic pre-installed
- Easy to flash or update
- **Not all PMSG sensors are supported in Meshtastic v4.7**

---

## PMSG Meshtastic Versions
PMSG Meshtastic builds exist for:
- ESP32 (XIAO ESP32S3)
- nRF52840 (XIAO nRF)

Both use the **Wio-SX1262** LoRa module or nieuwer version by Seeedstudio see website...

---

## Quick Setup Guides

### nRF52840 PMSG (Quick Setup)
https://wiki.seeedstudio.com/xiao_nrf52840&_wio_SX1262_kit_for_meshtastic/

### ESP32S3 PMSG (Quick Setup)
https://wiki.seeedstudio.com/wio_sx1262_with_xiao_esp32s3_kit/

---

## Custom Sensors (I2C) in Meshtastic
Adding custom I2C sensors to Meshtastic on PMSG:

Tutorial video:  
https://www.youtube.com/watch?v=zhS_2MZbjzM

---

## PMSG Meshtastic Pin Mapping

- Vibration motor: **D10**
- User button (optional): **D9**

---

## Meshtastic BLE / App PIN
Default Meshtastic PIN: 123456

---

## Seeed Studio SenseCraft

Website:  
https://sensecraft.seeed.cc

Current support:
- **XIAO ESP32S3 Sense only**

---

## SenseCraft + LoRa AI
- Uses MobileNetV2-based models
- Train and deploy AI on device
- Send inference data via LoRaWAN

AI dashboard:  
https://sensecraft.seeed.cc/ai/home

---

## PMSG SenseCraft Pin Mapping

- Vibration motor: **D10**
- User button (optional): **D9**

---

## SenseCraft Camera (ESP32S3 Sense)

Hardware features:
- Dual-core ESP32S3
- WiFi + BLE
- Built-in camera
- Digital microphone
- 8MB PSRAM + 8MB Flash
- SD card support

Product link:  
https://www.seeedstudio.com/XIAO-ESP32S3-Sense-p-5639.html

Camera installation:
- Align camera expansion board with B2B connector
- Requires **PMSG flat cable** or **PMSG black connector**

---

## PMSG + TTN (The Things Network)

TTN setup guide:  
https://wiki.seeedstudio.com/wio_sx1262_xiao_esp32s3_LNS_TTN/

---

## PMSG Connect to TTN LoRaWAN (The Things Network) Code Examples

Basic LoRaWAN sensor node example:  
https://wiki.seeedstudio.com/wio_sx1262_xiao_esp32s3_for_lora_sensor_node/

Single-channel gateway reference:  
https://github.com/Lora-net/one_channel_hub

---

## PMSG Code & Libraries
If something is missing (drivers, libs, examples):
https://github.com/Control-C/PMSG

---

## PMSG RF Options
PMSG supports:
- LoRa
- Mobile (in development)
- Custom antennas:
  - PCB
  - Flex
  - SMA
  - IC smded on board 

You are free to select or design your own antenna based on use case.

## PMSG meshcore 
We still working on this but have look we dont support it but will work
Website  https://meshcore.co.uk 
how to flashhttps://flasher.meshcore.co.uk

## Meshcore BLE / App PIN
Default Meshcore  **PIN: 123456**

## PMSG meshcore Pin Mapping

- Vibration motor: **D10**
- User button (optional): **D9**

## PMSG Modular Connectivity (MCU & RF)

PMSG is designed to work with **multiple Seeed Studio XIAO MCU variants**, which means different **RF and communication modules** can be added depending on the use case.

### Supported & Extendable RF Options
Depending on the selected MCU and add-on modules, PMSG can support:

- **Wi-Fi (ESP32-based XIAO)**
  - API communication
  - MQTT
  - Webhooks
  - MCP-style AI integrations

- **Bluetooth / BLE**
  - Local device pairing
  - Mobile apps
  - Wearable interactions

- **LoRa / LoRaWAN**
  - Long-range, low-power communication
  - Mesh or cloud-connected setups

- **Cellular / Mobile (optional add-on)**
  - GPS-enabled mobile data units
  - Standalone connectivity without Wi-Fi

- **Zigbee / Z-Wave (optional add-on)**
  - Smart home and local mesh networks
  - Secure device-to-device communication

### Architecture Note
All RF modules are **modular**:
- Select the MCU first
- Add the RF layer that fits your region, power budget, and network requirements
- Combine multiple communication paths where needed (e.g. BLE + LoRa, Wi-Fi + MQTT)

This makes PMSG adaptable for **wearables, sensors, smart environments, and AI-connected edge devices**.