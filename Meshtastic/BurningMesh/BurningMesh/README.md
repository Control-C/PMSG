# Burning Mesh notes for PMSG **Playa Mesh Smart Glasses**

Put camp-specific files here:

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

## Quick reminder

1. Flash with https://burn.meshtastic.org
2. Set correct region
3. Configure D1 / D2 in the Meshtastic app
4. Join your camp channel

That’s all most people need.
