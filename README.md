# Smart Alarm Clock for Hearing-Impaired Users

A complete, open-source alarm clock system designed specifically for hearing-impaired users, utilizing multi-sensory alerts including vibration, flashing LED, and visual display.

## Project Overview

This project provides a fully functional smart alarm clock built on the ESP32 microcontroller with the following features:

### Core Features
- **Multi-Sensory Alerts**: Vibration motor + LED flashing + visual display
- **Physical Interface**: 5 buttons for easy operation
- **Web Dashboard**: Remote alarm management via browser
- **WiFi Connectivity**: On-demand WiFi mode for web control
- **Multiple Alarms**: Support for 3 independent alarms
- **Customizable Patterns**: Gentle, Medium, or Strong vibration patterns
- **RTC Accuracy**: DS3231 real-time clock for precise timekeeping
- **OLED Display**: Status and time display
- **Power Efficient**: Sleep mode reduces consumption to ~5mA

## Hardware Components

### Required
- **ESP32 DevKit Board** (Rs 350-450) - Main microcontroller
- **DS3231 RTC** (Rs 100-150) - Real-time clock for accuracy
- **Vibration Motor** (Rs 150-300) - Tactile alert mechanism
- **IRF520 MOSFET** (Rs 50-80) - Safe motor control
- **WS2812B LED** (Rs 100-200) - RGB flashing alerts
- **Push Buttons x5** (Rs 20-50) - Physical controls
- **5V/2A Power Adapter** (Rs 150-250) - System power
- **Breadboard & Wires** (Rs 100-150) - Prototyping supplies

### Optional
- **0.96" OLED Display** (Rs 150-200) - Status display (not included, requires separate purchase)

## Project Structure

```
smart-alarm-clock/
├── smart_alarm_clock.ino          # Main sketch (upload this)
├── config.h                        # Pin definitions & constants
├── rtc_manager.h/cpp               # RTC time management
├── display_manager.h/cpp           # OLED display control
├── button_handler.h/cpp            # Button input processing
├── alarm_manager.h/cpp             # Alarm storage & logic
├── alert_controller.h/cpp          # Vibration & LED patterns
├── wifi_manager.h/cpp              # WiFi connectivity
├── web_server.h/cpp                # REST API server
├── data/
│   ├── index.html                  # Web dashboard
│   ├── style.css                   # Dashboard styling
│   └── script.js                   # Dashboard logic
└── docs/
    ├── README.md                   # This file
    ├── SETUP_GUIDE.md              # Installation guide
    ├── HARDWARE_SETUP.md           # Wiring guide
    ├── USER_MANUAL.md              # Usage instructions
    └── TROUBLESHOOTING.md          # Issue solutions
```

## Quick Start

### 1. Install Development Environment
```bash
# Install Arduino IDE 2.0+
# Add ESP32 board support via Board Manager
# Install 6 required libraries (see SETUP_GUIDE.md)
```

### 2. Copy Project Files
- Copy all .h and .cpp files to your project folder
- Copy smart_alarm_clock.ino
- Create data/ subfolder with HTML/CSS/JS files

### 3. Compile & Upload
```
Sketch -> Verify/Compile (Ctrl+R)
Sketch -> Upload (Ctrl+U)
Tools -> ESP32 Sketch Data Upload  # For web interface
```

### 4. Access Web Dashboard
- Open browser to `http://192.168.4.1`
- Set alarms and configure patterns
- Monitor alarm status in real-time

## Documentation

### For Setup & Installation
- **Start here**: `docs/SETUP_GUIDE.md` - Complete development setup
- **Hardware**: `docs/HARDWARE_SETUP.md` - Detailed wiring diagrams

### For Operation
- **User Guide**: `docs/USER_MANUAL.md` - How to use the device
- **Troubleshooting**: `docs/TROUBLESHOOTING.md` - Common issues & fixes

## System Architecture

### Hardware Configuration

**I2C Bus (GPIO 21/22):**
- DS3231 RTC (0x68)
- OLED Display (0x3C)

**GPIO Inputs (Pull-up Buttons):**
- GPIO 14: Mode/Menu
- GPIO 15: Increment
- GPIO 16: Decrement  
- GPIO 17: Confirm/Set
- GPIO 18: Snooze/Stop

**GPIO Outputs:**
- GPIO 12: Vibration Motor (via MOSFET)
- GPIO 13: WS2812B LED (NeoPixel)

**Power:**
- 5V: Motor + LED power
- 3.3V: RTC, OLED, ESP32 logic
- GND: Common ground

### Software Architecture

**Core Modules:**
1. **RTCManager** - Time synchronization
2. **DisplayManager** - OLED output
3. **ButtonHandler** - Input processing (debounced)
4. **AlarmManager** - Alarm storage in EEPROM
5. **AlertController** - Motor & LED pattern generation
6. **WiFiManager** - Network connectivity
7. **WebServer** - REST API for web control

**State Machine:**
- IDLE -> MENU -> SET_ALARM/WIFI_CONFIG
- IDLE -> ALARM_ACTIVE -> ALARM_SNOOZED/STOPPED

## Key Features

### Multi-Sensory Alerts
- **Vibration Patterns**: Gentle (60%), Medium (80%), Strong (100%)
- **LED Flashing**: Color changes with intensity (Yellow -> Orange -> Red)
- **Visual Display**: "ALARM!" message with time
- **Escalation**: Gradual increase in intensity if not acknowledged

### User Interface
**Physical Buttons:**
- Simple 5-button interface
- Debounced input (50ms)
- Long-press detection for extended actions

**Web Dashboard:**
- Responsive design (mobile-friendly)
- Real-time alarm management
- Customizable vibration patterns
- Per-alarm day-of-week selection

### Reliability
- **RTC Accuracy**: +-2ppm (+-2 min/month drift)
- **Persistent Storage**: EEPROM-based alarm storage
- **Power Management**: Auto sleep mode when WiFi disabled
- **Error Handling**: Graceful degradation if components unavailable

## Performance Metrics

| Metric | Value |
|--------|-------|
| Idle Power | 5-10mA |
| WiFi Active | 50-80mA |
| Alarm Active | 200-300mA |
| RTC Accuracy | +-2ppm |
| Flash Storage | 4MB |
| EEPROM | 512 bytes |
| Alarms Supported | 3 |

## Customization Options

### Easy Customizations (No code change needed)
- Alarm time via physical buttons or web interface
- Vibration pattern selection (Gentle/Medium/Strong)
- Repeat days selection (Mon-Sun)
- WiFi on/off toggle

### Advanced Customizations (Requires code modification)
- Alarm duration (default: 5 minutes)
- Snooze duration (default: 5 minutes)
- Vibration intensity levels
- LED brightness and color schemes
- Button sensitivity/debounce timing
- Max number of alarms (default: 3)

See config.h for all adjustable constants.

## Educational Value

This project demonstrates:
- **Embedded Systems**: ESP32 GPIO, PWM, I2C protocols
- **Hardware Integration**: Multi-device coordination
- **State Management**: Complex system state handling
- **Web Development**: REST APIs, responsive design
- **EEPROM Management**: Persistent data storage
- **Power Management**: Sleep modes and optimization

## License

This project is open-source and available for educational and personal use.

## Contributing

Improvements and bug fixes are welcome! Consider:
- Better alarm management UI
- Bluetooth connectivity
- Cloud synchronization
- Mobile app development
- Additional alert patterns

## Support

### Getting Help
1. Check `docs/TROUBLESHOOTING.md` first
2. Review `docs/SETUP_GUIDE.md` for installation
3. Look at Serial Monitor output (115200 baud)
4. Check hardware connections in `docs/HARDWARE_SETUP.md`

### Common Issues
- **ESP32 not detected**: Install CH340 driver
- **Libraries missing**: Reinstall via Library Manager
- **Display not showing**: Check I2C address and wiring
- **Buttons not responding**: Verify pull-up resistors (10k ohm)

## Specifications

### Microcontroller
- ESP32 dual-core 240MHz processor
- 4MB flash storage
- 512 bytes EEPROM
- 10 ADCs, SPI, I2C, UART interfaces

### RTC (DS3231)
- +-2ppm accuracy (+-2 minutes/month)
- Automatic leap year
- Battery backup support
- Temperature-compensated

### Connectivity
- WiFi 802.11 b/g/n
- On-demand AP mode
- No external server required
- Local web interface only

## Use Cases

- **Bedroom Alarm**: Wake up with vibration + visual alerts
- **Important Events**: Multiple alarms with escalating intensity
- **Accessibility**: Tailored for hearing-impaired users
- **Testing**: Learn embedded systems programming
- **DIY Project**: Customize for personal needs

## Safety Considerations

- MOSFET protects ESP32 from motor current spikes
- Pull-up resistors protect GPIO inputs
- Capacitors stabilize power to LED and RTC
- 5V/2A power supply provides sufficient current headroom
- Thermal management: Device stays cool even during continuous alarm

## Future Enhancements

Planned features for future versions:
- Bluetooth connectivity
- Cloud alarm synchronization
- Mobile app (iOS/Android)
- Weather display integration
- Sleep tracking
- Advanced statistics
- Voice command support
- Integration with smart home systems

---

**Ready to build?** Start with `docs/SETUP_GUIDE.md`

**Questions?** Check `docs/TROUBLESHOOTING.md`

**Need help with hardware?** See `docs/HARDWARE_SETUP.md`
