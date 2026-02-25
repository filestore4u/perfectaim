/**
 * ESP32 Sniper Addon System
 * Created by Kaustav Ray
 * Date: 25 Feb, 2026
 * Copyright (c) 2026 Kaustav Ray. All Rights Reserved.
 */

#pragma once
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

// Environmental Sensor Wrapper (BME680)
class EnvironmentSensor {
private:
    Adafruit_BME680 bme;
    float temperature = 0;
    float humidity = 0;
    float pressure = 0;
    float gasResistance = 0;

public:
    bool begin();
    void update();
    float getTemperature();    // Celsius
    float getHumidity();       // %
    float getPressure();       // hPa
    float getGasResistance();  // KOhms
    float getAltitude(float seaLevelPressure = 1013.25); // Meters
};

// Wind Speed Sensor Wrapper (Analog)
class WindSensor {
private:
    int pin;
    float smoothedValue = 0;
    const float alpha = 0.1; // Smoothing factor for exponential moving average

public:
    WindSensor(int pin);
    void begin();
    void update();
    float getSpeed(); // Returns m/s (estimated)
};

// Ultrasonic Distance Sensor Wrapper
class DistanceSensor {
private:
    int triggerPin;
    int echoPin;
    float currentDistance = 0;
    unsigned long lastUpdate = 0;
    const unsigned long interval = 60; // Min time between pings (ms)

public:
    DistanceSensor(int trigger, int echo);
    void begin();
    void update(float temperatureC = 20.0); // Update distance with temp compensation
    float getDistance(); // cm
    bool isTargetDetected(float thresholdCm);
};

// Lock Sensor Wrapper (Digital)
class LockSensor {
private:
    int pin;
    bool lockedState = false;

public:
    LockSensor(int pin);
    void begin();
    void update();
    bool isLocked();
};
