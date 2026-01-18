# PMSG-XIAO – Bitchat BLE Mesh Test Node
![PMSG Cool Glasses](/img/bitchat_PMSG_demo2026.jpg)

**Seeed XIAO ESP32C6** + PMSG4PixelDisplay  + Buzzer  
Simple BLE peripheral that tries to play nice with the **Bitchat app**[](https://bitchat.free)

### What currently works

- Appears in Bitchat app scan (same service UUID)
- Receives writes from the app → logs in Serial Monitor
- LEDs flash **blue** on startup and when finding Bitchat peers
- Recognizes plaintext commands (best tested in nRF Connect app):
  - `RED-PMSG`   → red LEDs + 2 beeps
  - `BLU-PMSG`   → blue LEDs + 3 beeps
  - `GRN-PMSG`   → green LEDs + 1 long beep
  - `PMSG`       → rainbow animation + 4 beeps
- Sends dummy sensor replies on commands like: * not in bitchat app :'(
  - `PMSG-data UV`
  - `PMSG-data temp`
  - `PMSG-data`

→ You see replies in **Serial Monitor** and in **nRF Connect notifications**

### What does NOT work (yet)

- Replies **do NOT appear** in the official Bitchat app chat  
  (app expects encrypted Noise-protocol packets — we send raw text)
- Cannot **publish new messages** into Bitchat channels  
  (requires full client-mode + Noise handshake + framing)
- No real sensors connected yet (UV/temp values are fake)

### Quick test

1. Upload sketch
2. Open Serial Monitor (115200 baud)
3. Use **nRF Connect** app:
   - Connect → enable TX notifications
   - Write to RX char:
     - `PMSG` → rainbow
     - `PMSG-data UV` → cyan flash + reply text in notifications
4. In **Bitchat app**:
   - Send commands above → LEDs/buzzer react, but reply stays invisible in chat

### Future hope

- Add real UV/temp sensor
- Port full Bitchat protocol (see https://github.com/bitchatz/bitchatz-cpp)
- Send periodic sensor data into #mesh like:  
  `[XIAO @ Lelystad] UV: 4.2 | Temp: 22.5 °C`

Made by ᯅ Paul Mooij (@paulmooij) – Amsterdam, January 2026  
Hope one day real sensor data flows into Bitchat! 🌈📡
