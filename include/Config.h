/**
 * ESP32 Sniper Addon System
 * Created by Kaustav Ray
 * Date: 25 Feb, 2026
 * Copyright (c) 2026 Kaustav Ray. All Rights Reserved.
 */

#pragma once

#include <Arduino.h>

// WiFi Configuration
#define WIFI_SSID "your_SSID"
#define WIFI_PASSWORD "your_PASSWORD"

// Server Configuration
#define WEB_SERVER_PORT 80

// Pin Definitions - Sensors & Actuators
// Note: We are using nearly every exposed pin on the ESP32-CAM.
// SD Card usage is NOT possible with this configuration.

// LED Configuration
// Green LED (GPIO 2) is repurposed for Tilt Servo.
// Use -1 to disable code that references it.
#define PIN_LED_GREEN -1
#define PIN_LED_RED 4         // Flash LED

// Sensors
#define PIN_WIND_SPEED_ADC 33 // ADC1_CH5 (Safe for WiFi)

#define PIN_RADAR_TRIGGER 12
#define PIN_RADAR_ECHO 13

#define PIN_LOCK_SENSOR 3     // U0RXD (Serial Receive). Serial input will be disabled!

// Servos (New Feature)
// Repurposing Serial TX (GPIO 1) and Onboard LED (GPIO 2)
#define PIN_SERVO_PAN 1       // U0TXD - Serial Debug output will be disabled after setup!
#define PIN_SERVO_TILT 2      // Onboard LED - Green LED functionality removed.

// I2C for BME680 (Software Wire)
#define PIN_I2C_SDA 14
#define PIN_I2C_SCL 15

// Camera Pin Definitions (AI Thinker Model)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM       5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// System Settings
#define SERIAL_BAUD_RATE 115200
#define SENSOR_READ_INTERVAL_MS 1000
#define MOTION_THRESHOLD 15
