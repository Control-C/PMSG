# PMSG   (Paul's Mooij Smart Glasses )

![PMSG Cool Glasses](/img/cool.png)

Introducing the **Personal Monitoring Smart Glasses (PMSG)**—because who needs designer wearables when you’ve got DIY smarts and a pair of old goggles? 🤓

Ever looked at your old sunglasses and thought, *“These could use a little IoT love?”* Well, now they can! The PMSG kit lets you swap out those boring old arms for a custom-made PCBA that looks and functions like something a hacker MacGyver would dream up. It fits just like the original—using the same hinges, screws, and probably duct tape somewhere—but now you’ve got space for sensors, connectors, and all the IoT magic you can pack onto your temples.

Perfect for those who want to look cool while tracking air quality, blinking Morse code, or… just pretending they’re Tony Stark. **Now with 100% more tech nerd street cred**. 🕶️


![PMSG Cool Glasses](/img/Example_flow.png)

## How It Works

- Replace one (or both) arms of your glasses with a custom PCBA 
- The PCBA has special cutouts for hinges, making it easy to attach to any old frame.
- Add or remove sensors and connectors with ease—think plug-and-play for your face!
![PMSG ](/img/pmsgv1.png)

## Parts List

Here’s what you’ll need to bring your PMSG to life:

- Old pair of safety glasses or sunglasses
- Custom PCBA (available in this repo its bassed on the seeed studio XIAO )
- buy extra pair of hinges simular to the original glasses  
- Sensors (IoT-enabled sensors of your choice: e.g., air quality, light, temperature, Oled display and camera bassed off the Seeedstudio Grove and XIAO modular platform )
- Including A onboard batt holder and system simular to XIAO to run stand alone plans are to add solar pannels at side to dubble as UV sensor to warn user 
- Connectors (to hook your glasses up to the web or other devices)

## Configuration 
- The WIFI and BLE can we switch off and power and data can com via a USB-C host like Smartphone this protocol is called USB OTG 
experimental* ( tested on Android for apple ios we need to enter the Apple MFi program or have look at https://github.com/rsms/peertalk/tree/master/Peertalk%20iOS%20Example )

The fix possition of 2.4gHz antannane ( wifi/ble ) and possible more like Lora, UHF 800/900mhz I im still researching to also compli with [SAR](https://en.wikipedia.org/wiki/Specific_absorption_rate), RED, and FCC compliance 

## PCB Design " WARNING IT'S STILL UNTESTED DRAFT DESIGN"

The PCB design includes:
- **Special cutouts** for attaching to standard eyewear hinges.
- **Mounting holes** for securing the arms to the glasses frame.
- Modular layout to easily add/remove components like sensors or IoT modules.

## Installation

1. Get the PCB from the `pcb_design` folder. go to https://www.seeedstudio.com/fusion_pcb.html to *oder sample
   *( dubble check hinges ar simular to you glasses and holes on PCB )  
![PMSG ](/img/hinges.png)

[Purchase on AliExpress](https://www.aliexpress.com/item/1005006578852003.html?spm=a2g0o.productlist.main.19.6881yaeCyaeCyD&algo_pvid=940ad7e5-1196-4d4c-bb03-062302b6db00&algo_exp_id=940ad7e5-1196-4d4c-bb03-062302b6db00-9&pdp_npi=4%40dis%21EUR%211.62%211.62%21%21%211.75%211.75%21%40211b653717259752143568539e1d24%2112000037718488758%21sea%21NL%214699555345%21X&curPageLogUid=cUCXLkrXc9w7&utparam-url=scene%3Asearch%7Cquery_from%3A)

3. Use the original screw from your glasses to attach the PCBA
4. Mount your sensors and connectors into the designated slots on the PCBA. To ensure a better fit, you can enhance the setup by adding 3D-printed parts or applying paint/epoxy to the PCB.
5. Voilà! Your Poor Men’s Smart Glasses are now ready for some tech magic.

## Software: TODO !!!
Best is to go and check the offical website of [Seeed studio for ESP32C3](https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/?gad_source=1&gclid=CjwKCAjw3P-2BhAEEiwA3yPhwBK6aCH6z2kCqP-esxGC8T7rXoWwqtbKFqweMOIq9PhEG1TkwNyJIxoCtSoQAvD_BwE) this will help and let you setup your system

REV v1 does not include battery management. The plan for hardware REV 2 is to add battery charging and A0 monitoring for the battery later... 

I primarily use PlatformIO, but since I started  also with the Arduino IDE in my early years, I’ll also include files compatible with it make bit easyer to upload make stuff work... 

------- WIP  ------- 

The PMSG v1 includes: 
- MCU - Esp32C3 *( possible to use the WIFI and BLE please add external antenna its compatible XIAO 2.4Hgz also on todo for v2 )
- Lux sensor - BH1721FVC-TR
- Humidety and Temperure sensor - HDC1080DMBR
- Button 1  port D8 3v3 
- Button 2  port D10 3v3
- USB - C  *( send data via serial 119200 for OTG ) 
- Grove port for I2C ( for adding display on to glass inside inlcuding lens and  [Oled Display 0.96](https://wiki.seeedstudio.com/Grove-OLED_Display_0.96inch/)  )
- Grove port for Uart 3v3  ( for adding a uart bassed camera to side of frame ) 
- Grove port for GPIO  - D2 and D3 3v3
  
- A0 will be used to read out Batt level  [Battery voltage monitor]([https://wiki.seeedstudio.com/Grove-OLED_Display_0.96inch/](https://forum.seeedstudio.com/t/battery-voltage-monitor-and-ad-conversion-for-xiao-esp32c/267535) )

  TODO
  Add companion apps for iOS and Android or WIFI linkup, integrating them with machine learning capabilities for enhanced functionality I think adding OLED / buzzer - [vibration_Motor]( https://wiki.seeedstudio.com/Grove-Vibration_Motor) wil be fun to try out - like cycling and letting AI send feedback and getting data from glasses... 

