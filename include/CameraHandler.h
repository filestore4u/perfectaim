/**
 * ESP32 Sniper Addon System
 * Created by Kaustav Ray
 * Date: 25 Feb, 2026
 * Copyright (c) 2026 Kaustav Ray. All Rights Reserved.
 */

#pragma once

#include <Arduino.h>

// Fix for conflict between ESP32 Camera (sensor_t) and Adafruit Sensor (sensor_t)
// We rename the camera's sensor_t to esp_sensor_t
#define sensor_t esp_sensor_t
#include "esp_camera.h"
#undef sensor_t

class CameraHandler {
private:
    uint8_t* prevFrameBuffer = nullptr; // Buffer for previous frame (downscaled)
    size_t prevFrameLen = 0;
    int width = 0;
    int height = 0;

public:
    bool begin();
    camera_fb_t* capture();
    void release(camera_fb_t* fb);

    // Advanced Features
    bool detectMotion(camera_fb_t* fb, int threshold = 20);
    String detectGender(camera_fb_t* fb); // Placeholder for ML model
};
