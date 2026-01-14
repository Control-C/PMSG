AIS “AI-Setup guide” Instructions – PMSG AI-Assisted Development
> 🛸 Tip: Don’t forget your towel! Just like in Hitchhiker’s Guide. Use this as a reminder while exploring AI setup always think 42 is anwser. - Paul Stefaan Mooij 2025


This folder enables AI-assisted setup, development, and debugging for the **PMSG (Personal Smart Monitoring Glasses)** project.

It is designed to work with:
- ChatGPT / OpenAI
- Grok (xAI)
- Cursor IDE
- GitHub Copilot
- Google Gemini

AI tools can use the files in this folder as **structured context** to provide accurate, hardware-aware guidance.

📁 **AIS Folder:** [https://github.com/Control-C/PMSG/tree/main/AIS](https://github.com/Control-C/PMSG/tree/main/AIS)

---

## 👉 Start with AI-SetupGuide Setup 🤖

Use your favorite AI assistant to set up PMSG quickly.  
Each option opens a pre-configured prompt tailored to your hardware and project.

<p align="center">
  <a href="https://chat.openai.com/?q=Guide+me+to+set+up+PMSG+from+this+repo:+https://github.com/Control-C/PMSG/tree/main/Arduino%20IDE+Ask+which+XIAO+board+I+have+and+if+I+use+I2C+sensors">
    <img src="https://img.shields.io/badge/Ask_ChatGPT-Setup_Guide-10a37f?style=for-the-badge&logo=openai&logoColor=white" alt="Ask ChatGPT"/>
  </a>
  <a href="[https://gemini.google.com/share?q=Guide+me+to+set+up+PMSG+from+this+repo:+https://github.com/Control-C/PMSG/tree/main/Arduino%20IDE+Ask+which+XIAO+board+I+have+and+if+I+use+I2C+sensors+and+explain+wiring](https://gemini.google.com/gem/1Hbn7N0X7nBb2Egw54Rex0iQe2veH4cP_?usp=sharing)">
    <img src="https://img.shields.io/badge/Ask_Gemini-Setup_Guide-blue?style=for-the-badge&logo=google-gemini" alt="Ask Gemini"/>
  </a>
  <a href="https://grok.com/?q=Help+me+set+up+PMSG+from+this+repo%3A+https%3A%2F%2Fgithub.com%2FControl-C%2FPMSG%2Ftree%2Fmain%2FArduino%2520IDE.+Ask+which+XIAO+board+I+have+and+if+I+use+I2C+sensors.+Guide+me+through+the+setup%2C+including+BLE%2C+LEDs%2C+vibration%2C+and+sensors.&referrer=grok.com">
    <img src="https://img.shields.io/badge/Ask_Grok-AI_Setup-000000?style=for-the-badge&logo=x&logoColor=white" alt="Ask Grok"/>
  </a>
  <a href="javascript:void(prompt('Copy this prompt for Le Chat:', 'Guide me to set up PMSG from this repo: https://github.com/Control-C/PMSG/tree/main/Arduino%20IDE\nAsk which XIAO board I have and if I use I2C sensors.\nAlso, take extra info from: https://github.com/Control-C/PMSG/tree/main/AIS\nAnd use https://pmsg.online for reference.'));">
  <img src="https://img.shields.io/badge/Copy_Prompt_for_Le_Chat-Setup_Guide-ff69b4?style=for-the-badge&logo=mistralai&logoColor=white" alt="Copy Prompt"/>
</a>
</p>

**How to use:**  
1. Click the badge for your preferred AI.  
2. Follow the AI’s prompts to select your **XIAO board** and **I²C sensors**.  
3. Get step-by-step guidance for **BLE, LEDs, vibration, and more**.  

> 💡 Tip: You can try multiple AIs to compare instructions or get alternative approaches.

---

## ⚡ Tools & IDEs

### Arduino IDE
[![Download Arduino IDE](https://img.shields.io/badge/Download-Arduino_IDE-00979D?style=for-the-badge&logo=arduino&logoColor=white)](https://www.arduino.cc/en/software)

### PlatformIO
[![PlatformIO](https://img.shields.io/badge/PlatformIO-Install-ff7f00?style=for-the-badge&logo=platformio&logoColor=white)](https://github.com/Control-C/PMSG/tree/main/PlatformIO)

> 💡 Tip  
> Always tell the AI which **Seeed Studio XIAO board** you are using and whether you have **I²C sensors** connected.  
> Recommended:
> - Open the PlatformIO folder (plus Agent to help out)  
> - Let Agent read `/ai/mcp/*.json` (if present)  
> - Ask Agent to respect XIAO board pinouts

### GitHub Copilot
<p align="left">
   <a href="https://github.com/copilot/spaces/Control-C/1">
   <img src="https://img.shields.io/badge/GitHub-Copilot-24292e?style=for-the-badge&logo=github&logoColor=white" alt="PMSG COPILOT SPART"/> </a>
</p>

### Cursor IDE
[![Open in Cursor](https://img.shields.io/badge/Open_in-Cursor-ff6f00?style=for-the-badge&logo=cursor&logoColor=white)](https://cursor.sh)

### Apple / Xcode – Embedded Swift
[![Xcode](https://img.shields.io/badge/Apple-Xcode-000000?style=for-the-badge&logo=apple&logoColor=white)](https://github.com/Control-C/PMSG/tree/main/Embedded%20Swift)

For Apple-centric workflows and Swift-based embedded experiments.
