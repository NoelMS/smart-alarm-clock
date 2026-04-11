#ifndef CONFIG_H
#define CONFIG_H

// ==================== PIN DEFINITIONS ====================
#define RTC_SDA 21
#define RTC_SCL 22

#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_ADDR 0x3C

#define BUTTON_MODE 14
#define BUTTON_INC 15
#define BUTTON_DEC 16
#define BUTTON_CONFIRM 17
#define BUTTON_SNOOZE_STOP 18

#define MOTOR_PIN 12
#define LED_PIN 13
#define NUM_LEDS 1

// ==================== TIMING CONSTANTS ====================
#define ALARM_DURATION_MS 300000  // 5 minutes
#define SNOOZE_DURATION_MS 300000 // 5 minutes
#define BUTTON_DEBOUNCE_MS 50
#define BUTTON_LONG_PRESS_MS 2000

// ==================== ALARM SETTINGS ====================
#define MAX_ALARMS 3
#define DEFAULT_VIBRATION_PATTERN 1 // 0=gentle, 1=medium, 2=strong

// ==================== EEPROM ADDRESSES ====================
#define EEPROM_SIZE 512
#define EEPROM_WIFI_START 0
#define EEPROM_WIFI_SSID_ADDR 0
#define EEPROM_WIFI_SSID_SIZE 32
#define EEPROM_WIFI_PASS_ADDR 32
#define EEPROM_WIFI_PASS_SIZE 64

#define EEPROM_ALARM_START 100
#define EEPROM_ALARM_SIZE 10 // Per alarm
// Alarm struct: hour(1) + minute(1) + enabled(1) + days(1 byte for 7 days) + pattern(1) + reserved(5)

#define EEPROM_SETTINGS_ADDR 250
// Settings: vibration_pattern(1) + led_brightness(1) + snooze_duration(2) + reserved

// ==================== DISPLAY SETTINGS ====================
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define DISPLAY_REFRESH_MS 500

// ==================== WIFI SETTINGS ====================
#define WIFI_TIMEOUT_MS 10000
#define WIFI_RETRY_INTERVAL_MS 5000
#define WIFI_SSID_MAX_LEN 32
#define WIFI_PASS_MAX_LEN 64

// ==================== SERIAL DEBUGGING ====================
#define DEBUG_SERIAL 115200
#define DEBUG_ENABLED 1

#if DEBUG_ENABLED
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINTF(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINTF(fmt, ...)
#endif

// ==================== VIBRATION PATTERNS ====================
enum VibrationType {
  GENTLE = 0,
  MEDIUM = 1,
  STRONG = 2
};

// Gentle: 100ms on, 100ms off, 60% duty
// Medium: 200ms on, 150ms off, 80% duty, escalating intensity
// Strong: 300ms on, 100ms off, 100% duty, continuous

#endif // CONFIG_H
