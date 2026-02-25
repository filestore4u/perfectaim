/**
 * ESP32 Sniper Addon System
 * Created by Kaustav Ray
 * Date: 25 Feb, 2026
 * Copyright (c) 2026 Kaustav Ray. All Rights Reserved.
 */

#include "Sensors.h"
#include "Config.h"
#include <Wire.h>

// --- Environment Sensor (BME680) ---

bool EnvironmentSensor::begin() {
    // Initialize I2C on specific pins
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);

    if (!bme.begin(0x76, &Wire)) { // Default I2C address
        if (!bme.begin(0x77, &Wire)) { // Secondary address
             return false;
        }
    }

    // Set up oversampling and filter initialization
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320, 150); // 320*C for 150 ms

    return true;
}

void EnvironmentSensor::update() {
    // Perform a reading (blocking, but necessary for this sensor library structure)
    // To make it non-blocking, we'd need to manage state manually or use async calls
    // For this application, a short block every second is acceptable.
    if (!bme.performReading()) {
        return;
    }
    temperature = bme.temperature;
    humidity = bme.humidity;
    pressure = bme.pressure / 100.0; // Convert Pa to hPa
    gasResistance = bme.gas_resistance / 1000.0; // KOhms
}

float EnvironmentSensor::getTemperature() { return temperature; }
float EnvironmentSensor::getHumidity() { return humidity; }
float EnvironmentSensor::getPressure() { return pressure; }
float EnvironmentSensor::getGasResistance() { return gasResistance; }
float EnvironmentSensor::getAltitude(float seaLevelPressure) {
    return bme.readAltitude(seaLevelPressure);
}

// --- Wind Sensor ---

WindSensor::WindSensor(int p) : pin(p) {}

void WindSensor::begin() {
    pinMode(pin, INPUT);
    // Note: If using ADC2, ensure WiFi is handled correctly or use ADC1
}

void WindSensor::update() {
    int raw = analogRead(pin);
    // Simple Exponential Moving Average
    smoothedValue = (alpha * raw) + ((1.0 - alpha) * smoothedValue);
}

float WindSensor::getSpeed() {
    // Convert ADC value to Voltage (ESP32 ADC is 12-bit, 0-4095, 3.3V ref)
    float voltage = smoothedValue * (3.3 / 4095.0);

    // Placeholder conversion: Assume 0-3.3V maps to 0-30 m/s
    // Real sensors have specific curves.
    return voltage * 9.09;
}

// --- Distance Sensor ---

DistanceSensor::DistanceSensor(int t, int e) : triggerPin(t), echoPin(e) {}

void DistanceSensor::begin() {
    pinMode(triggerPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(triggerPin, LOW);
}

void DistanceSensor::update(float tempC) {
    unsigned long now = millis();
    if (now - lastUpdate < interval) return;
    lastUpdate = now;

    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);

    long duration = pulseIn(echoPin, HIGH, 30000); // 30ms timeout (~5m)

    if (duration == 0) {
        // Timeout, no target or out of range
        currentDistance = 999.9;
    } else {
        // Speed of sound calculation based on temperature
        // c = 331.3 + 0.606 * T (m/s)
        float speedOfSound = 331.3 + (0.606 * tempC);
        // Convert to cm/us: (speed * 100) / 1000000 = speed / 10000
        float speedCmUs = speedOfSound / 10000.0;

        currentDistance = (duration / 2.0) * speedCmUs;
    }
}

float DistanceSensor::getDistance() {
    return currentDistance;
}

bool DistanceSensor::isTargetDetected(float thresholdCm) {
    return (currentDistance > 0 && currentDistance < thresholdCm);
}

// --- Lock Sensor ---

LockSensor::LockSensor(int p) : pin(p) {}

void LockSensor::begin() {
    pinMode(pin, INPUT_PULLUP); // Use pullup for switch
}

void LockSensor::update() {
    // Simple digital read. Can add debounce if needed.
    // Assuming switch connects to GND when "Locked" (LOW) or "Active" (HIGH)
    // Adjust based on wiring. Assuming HIGH = Locked based on prompt
    // "isLocked = digitalRead(lockSensorPin) == HIGH;"
    lockedState = (digitalRead(pin) == HIGH);
}

bool LockSensor::isLocked() {
    return lockedState;
}
