// main/main.swift - PMSG Smart Glasses Test in Embedded Swift
// Ported from Arduino sketch for Seeed Studio XIAO ESP32C6
// Features:
// - Button on D9 (GPIO20, Boot button, active LOW with internal pull-up)
// - Vibration motor PWM on D10 (GPIO18)
// - 4x WS2812 LEDs on D1 (GPIO1)
// - Internal yellow LED on GPIO15 (active LOW)
//
// Prerequisites:
// - Follow https://wiki.seeedstudio.com/xiao-esp32-swift/ for setup
// - Use swift-embedded-examples (esp32c6 branch or similar) as base
// - Include Led.swift and LedStrip.swift from the examples
// - Project built with ESP-IDF v5.2 + Swift 6.0 toolchain
// - In CMakeLists.txt: add REQUIRES driver (for gpio/ledc/rmt)

import Foundation  // For millis() approximation if needed

// Wrapper classes from swift-embedded-examples (copy them into your project)
class Led {
    private let gpio: gpio_num_t
    
    init(gpioPin: Int32) {
        self.gpio = gpio_num_t(gpioPin)
        gpio_reset_pin(self.gpio)
        gpio_set_direction(self.gpio, GPIO_MODE_OUTPUT)
    }
    
    func setLed(value: Bool) {
        gpio_set_level(self.gpio, value ? 0 : 1)  // Active LOW for internal LED
    }
}

class LedStrip {
    private let strip: led_strip_handle_t?
    
    init(gpioPin: Int32, maxLeds: Int) {
        var handle: led_strip_handle_t?
        let config = led_strip_config_t(
            strip_gpio_num: gpioPin,
            max_leds: UInt32(maxLeds),
            dev: nil  // Use default RMT
        )
        let err = led_strip_new_rmt_device(&config, &handle)
        self.strip = (err == ESP_OK) ? handle : nil
        led_strip_clear(self.strip)
        led_strip_refresh(self.strip)
    }
    
    func clear() {
        led_strip_clear(strip)
    }
    
    func refresh() {
        led_strip_refresh(strip)
    }
    
    func setPixel(index: Int, color: (r: UInt8, g: UInt8, b: UInt8)) {
        led_strip_set_pixel(strip, UInt32(index), UInt32(color.r), UInt32(color.g), UInt32(color.b))
    }
}

// Helper for HSV to RGB (simple implementation for rainbow effect)
func hsvToRgb(h: UInt8, s: UInt8 = 255, v: UInt8 = 255) -> (r: UInt8, g: UInt8, b: UInt8) {
    let hi = Int(h / 43)
    let f = Int(h) * 6 - hi * 43 * 6  // fractional part scaled
    let p = (Int(v) * (255 - Int(s))) >> 8
    let q = (Int(v) * (255 - (Int(s) * f / 256))) >> 8
    let t = (Int(v) * (255 - (Int(s) * (255 - f) / 256))) >> 8
    
    switch hi {
    case 0, 6: return (UInt8(v), UInt8(t), UInt8(p))
    case 1: return (UInt8(q), UInt8(v), UInt8(p))
    case 2: return (UInt8(p), UInt8(v), UInt8(t))
    case 3: return (UInt8(p), UInt8(q), UInt8(v))
    case 4: return (UInt8(t), UInt8(p), UInt8(v))
    default: return (UInt8(v), UInt8(p), UInt8(q))
    }
}

@_cdecl("app_main")
public func app_main() {
    print("PMSG Smart Glasses Test: Vib GPIO18 + 4 WS2812 GPIO1 + Internal LED GPIO15 + Button GPIO20")
    
    // Pins
    let buttonPin: gpio_num_t = GPIO_NUM_20  // D9, Boot button
    let vibPin: gpio_num_t = GPIO_NUM_18     // D10
    let internalLed = Led(gpioPin: 15)       // GPIO15, active LOW
    let ledStrip = LedStrip(gpioPin: 1, maxLeds: 4)  // D1, GPIO1
    
    // Vibration PWM setup (LEDC high-speed channel)
    let vibChannel: ledc_channel_t = LEDC_CHANNEL_0
    let vibTimer: ledc_timer_t = LEDC_TIMER_0
    ledc_timer_config(&(ledc_timer_config_t(
        speed_mode: LEDC_HIGH_SPEED_MODE,
        duty_resolution: LEDC_TIMER_10_BIT,
        timer_num: vibTimer,
        freq_hz: 5000,
        clk_cfg: LEDC_AUTO_CLK
    )))
    ledc_channel_config(&(ledc_channel_config_t(
        gpio_num: vibPin,
        speed_mode: LEDC_HIGH_SPEED_MODE,
        channel: vibChannel,
        intr_type: LEDC_INTR_DISABLE,
        timer_sel: vibTimer,
        duty: 0,
        hpoint: 0
    )))
    
    // Button input with pull-up
    gpio_set_direction(buttonPin, GPIO_MODE_INPUT)
    gpio_set_pull_mode(buttonPin, GPIO_PULLUP_ONLY)
    
    var oldButtonState = true  // HIGH (pulled up)
    var testOn = false
    
    print("Ready! Press Boot button (D9) to toggle test.")
    
    while true {
        // Read button (active LOW)
        let buttonState = gpio_get_level(buttonPin) == 0
        
        // Detect falling edge (press)
        if buttonState && !oldButtonState {
            testOn.toggle()
            print(testOn ? "Button: Test ON (Vib + LEDs + Internal)" : "Button: Test OFF")
            vTaskDelay(50 / portTICK_PERIOD_MS)  // Simple debounce
        }
        oldButtonState = buttonState
        
        if testOn {
            // Vibration full on
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, vibChannel, 1023)  // 10-bit max
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, vibChannel)
            
            // Rainbow cycle on WS2812
            let hueOffset = UInt32(Date().timeIntervalSince1970 * 1000 / 50) % 256  // Approx millis()/50
            for i in 0..<4 {
                let hue = UInt8((hueOffset + UInt32(i * 64)) % 256)
                let rgb = hsvToRgb(h: hue)
                ledStrip.setPixel(index: i, color: rgb)
            }
            ledStrip.refresh()
            
            // Internal LED on (active LOW)
            internalLed.setLed(value: true)
        } else {
            // All off
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, vibChannel, 0)
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, vibChannel)
            ledStrip.clear()
            ledStrip.refresh()
            internalLed.setLed(value: false)
        }
        
        vTaskDelay(20 / portTICK_PERIOD_MS)
    }
}