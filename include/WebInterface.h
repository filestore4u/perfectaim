/**
 * ESP32 Sniper Addon System
 * Created by Kaustav Ray
 * Date: 25 Feb, 2026
 * Copyright (c) 2026 Kaustav Ray. All Rights Reserved.
 */

#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "CameraHandler.h"

class WebInterface {
private:
    WebServer server;
    CameraHandler* camera;

    // Sensor Data Cache
    float temp = 0;
    float hum = 0;
    float pres = 0;
    float gas = 0;
    float alt = 0;
    float wind = 0;
    float dist = 0;
    bool locked = false;
    bool motion = false;
    String gender = "Unknown";

    // Servo Positions (Degrees)
    int pan = 90;
    int tilt = 90;

public:
    WebInterface(CameraHandler* cam);
    void begin();
    void handleClient();

    void updateSensorData(float t, float h, float p, float g, float a, float w, float d, bool l, bool m, String gen, int panPos, int tiltPos);

private:
    void handleRoot();
    void handleCapture();
    void handleData();
};
