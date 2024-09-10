# PMSG   (Paul's Mooij Smart Glasses )

![PMSG Cool Glasses](/img/cool.png)

Introducing the **Poor Men’s Smart Glasses (PMSG)**—because who needs designer wearables when you’ve got DIY smarts and a pair of old goggles? 🤓

Ever looked at your old sunglasses and thought, *“These could use a little IoT love?”* Well, now they can! The PMSG kit lets you swap out those boring old arms for a custom-made PCBA that looks and functions like something a hacker MacGyver would dream up. It fits just like the original—using the same hinges, screws, and probably duct tape somewhere—but now you’ve got space for sensors, connectors, and all the IoT magic you can pack onto your temples.

Perfect for those who want to look cool while tracking air quality, blinking Morse code, or… just pretending they’re Tony Stark. **Now with 100% more tech nerd street cred**. 🕶️

## How It Works

- Replace one (or both) arms of your glasses with a custom PCBA 
- The PCBA has special cutouts for hinges, making it easy to attach to any old frame.
- Add or remove sensors and connectors with ease—think plug-and-play for your face!
![PMSG ](/img/pmsgv1.png)

## Parts List

Here’s what you’ll need to bring your PMSG to life:

- Old pair of safety glasses or sunglasses
- Custom PCBA (available in this repo its bassed on the seeed studio XIAO )
- buy extra pair of hinges simular to the original glasses see [Link]([https://www.aliexpress.com/item/1005006578852003.html?spm=a2g0o.productlist.main.19.6881yaeCyaeCyD&algo_pvid=940ad7e5-1196-4d4c-bb03-062302b6db00&algo_exp_id=940ad7e5-1196-4d4c-bb03-062302b6db00-9&pdp_npi=4%40dis%21EUR%211.62%211.62%21%21%211.75%211.75%21%40211b653717259752143568539e1d24%2112000037718488758%21sea%21NL%214699555345%21X&curPageLogUid=cUCXLkrXc9w7&utparam-url=scene%3Asearch%7Cquery_from%3A]) 
- Sensors (IoT-enabled sensors of your choice: e.g., air quality, light, temperature, Oled display and camera bassed off the Seeedstudio Grove and XIAO modular platform )
- Including A onboard batt holder and system simular to XIAO to run stand alone plans are to add solar pannels at side to dubble as UV sensor to warn user 
- Connectors (to hook your glasses up to the web or other devices)

## Configuration 
- The WIFI and BLE can we switch off and power and data can com via a USB-C host like Smartphone 

## PCB Design " WARNING IT'S STILL UNTESTED DRAFT DESIGN"

The PCB design includes:
- **Special cutouts** for attaching to standard eyewear hinges.
- **Mounting holes** for securing the arms to the glasses frame.
- Modular layout to easily add/remove components like sensors or IoT modules.

## Installation

1. Print the PCB from the `pcb_design` folder.
2. Use the original screws and hinges from your glasses to attach the PCBA.
3. Mount your sensors and connectors onto the provided slots on the PCBA.
4. Voilà! Your Poor Men’s Smart Glasses are now ready for some tech magic.

