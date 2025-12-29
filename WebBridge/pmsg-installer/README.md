
## 🔌 Install firmware on your PMSG device

PMSG devices can be flashed **directly from the browser** — no Arduino IDE, no drivers, no apps.

### 👉 Open the installer
Go to:

**https://pmsg.2qr.at/pmsg-installer/**

> ⚠️ Works in **Chrome** or **Microsoft Edge** only  
> Safari and Firefox do not support WebUSB.

---

### What you need
- A PMSG device (ESP32-based)
- A **USB data cable** (not charge-only)
- Chrome or Edge browser

---

### How to install (30 seconds)

1. Plug your PMSG device into your computer via USB  
2. Open the installer link above  
3. Select the firmware you want (e.g. *ProxiLens*, *Demo*)  
4. Click **Install**  
5. Choose the USB device when prompted  
6. Wait until flashing is complete  
7. The device will reboot automatically

That’s it — your PMSG device is ready.

---

### If the device is not detected
- Try another USB cable (many cables are power-only)
- Unplug and reconnect the device
- If your board has a **BOOT** button:
  - Hold **BOOT**
  - Plug in the USB cable
  - Click **Install**
  - Release **BOOT** when flashing starts

---

### Supported hardware
- PMSG devices based on:
  - ESP32-S3
  - ESP32-C3
  - ESP32-C6
- Including Seeed Studio **XIAO ESP32** variants

---

### Notes
- Installing firmware may erase existing data on the device
- Only flash devices you own or are allowed to modify
- This tool is experimental and under active development

---

Part of the **PMSG ecosystem**  
*Browser → body is a valid interface* 👓
