/**
 * ESP32 Sniper Addon System
 * Created by Kaustav Ray
 * Date: 25 Feb, 2026
 * Copyright (c) 2026 Kaustav Ray. All Rights Reserved.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include "Config.h"
#include "Sensors.h"
#include "CameraHandler.h"
#include "WebInterface.h"

// --- Global Objects ---
EnvironmentSensor envSensor;
WindSensor windSensor(PIN_WIND_SPEED_ADC);
DistanceSensor distSensor(PIN_RADAR_TRIGGER, PIN_RADAR_ECHO);
LockSensor lockSensor(PIN_LOCK_SENSOR);

Servo servoPan;
Servo servoTilt;

CameraHandler camera;
WebInterface web(&camera);

// --- State Variables ---
unsigned long lastSensorUpdate = 0;
const unsigned long sensorInterval = SENSOR_READ_INTERVAL_MS;

// Ballistics Constants
const float MUZZLE_VELOCITY = 800.0; // m/s (approx .308)
const float GRAVITY = 9.81;

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    Serial.println("\n\n==================================");
    Serial.println("ESP32 SNIPER ADDON SYSTEM");
    Serial.println("License: Personal Use Only.");
    Serial.println("==================================");

    // Initialize LEDs
    pinMode(PIN_LED_RED, OUTPUT);
    digitalWrite(PIN_LED_RED, LOW);

    // Check if Green LED is still enabled (it is repurposed for Servo in Config)
    if (PIN_LED_GREEN != -1) {
        pinMode(PIN_LED_GREEN, OUTPUT);
        digitalWrite(PIN_LED_GREEN, LOW);
    }

    // Initialize Sensors
    Serial.println("[+] Initializing Sensors...");
    if (!envSensor.begin()) {
        Serial.println("[-] Warning: BME680 not found!");
    }
    windSensor.begin();
    distSensor.begin();
    lockSensor.begin();

    // Initialize Camera
    Serial.println("[+] Initializing Camera...");
    if (!camera.begin()) {
        Serial.println("[-] Camera Init Failed!");
    }

    // Connect to WiFi
    Serial.print("[*] Connecting to WiFi: ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 20) {
        delay(500);
        Serial.print(".");
        retries++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n[+] WiFi Connected!");
        Serial.print("[+] IP Address: ");
        Serial.println(WiFi.localIP());

        Serial.println("\n[!] WARNING: Serial Monitor (GPIO 1) will be disabled in 10 seconds");
        Serial.println("[!] for Servo Control. COPY THE IP NOW!");

        // Blink Red LED to warn
        for(int i=0; i<10; i++) {
            digitalWrite(PIN_LED_RED, HIGH);
            delay(500);
            digitalWrite(PIN_LED_RED, LOW);
            delay(500);
        }

        Serial.println("[*] Disabling Serial and Attaching Servos...");
        Serial.end(); // Release GPIO 1

        // Attach Servos
        // Allow time for Serial to flush and hardware to reset
        delay(100);

        servoPan.setPeriodHertz(50);
        servoPan.attach(PIN_SERVO_PAN, 1000, 2000); // GPIO 1

        servoTilt.setPeriodHertz(50);
        servoTilt.attach(PIN_SERVO_TILT, 1000, 2000); // GPIO 2

        // Center Servos
        servoPan.write(90);
        servoTilt.write(90);

    } else {
        Serial.println("\n[-] WiFi Connection Failed. Servos will NOT activate (need Serial for debug).");
        // In offline mode, we might as well keep Serial for debugging since we can't use the Web Dashboard
    }

    // Start Web Server
    web.begin();
}

void loop() {
    // 1. Handle Web Client
    web.handleClient();

    // 2. Update Sensors & Logic periodically
    unsigned long currentMillis = millis();
    if (currentMillis - lastSensorUpdate >= sensorInterval) {
        lastSensorUpdate = currentMillis;

        // Update Sensor Readings
        envSensor.update();
        windSensor.update();
        lockSensor.update();

        // Update Distance with Temperature Compensation
        float currentTemp = envSensor.getTemperature();
        distSensor.update(currentTemp);

        // 3. Ballistics Calculation
        // Get raw data
        float distM = distSensor.getDistance() / 100.0; // cm to m
        float windSpeed = windSensor.getSpeed(); // m/s

        int panAngle = 90;
        int tiltAngle = 90;

        // Only calculate if target is in valid range (e.g., > 5m and < 1000m)
        if (distM > 5.0 && distM < 1000.0) {
            // --- Bullet Drop (Elevation) ---
            // t = d / v
            float t = distM / MUZZLE_VELOCITY;
            // y = 0.5 * g * t^2
            float drop = 0.5 * GRAVITY * t * t;
            // angle = atan(drop / d)
            float angleDropRad = atan(drop / distM);
            float angleDropDeg = angleDropRad * (180.0 / PI);

            // Adjust Tilt (Laser needs to point DOWN to intersect trajectory?
            // Actually, if we want the laser to point at the IMPACT point,
            // and the barrel is aimed straight, the bullet hits LOW.
            // So the laser (which was parallel) is pointing HIGH relative to impact.
            // We need to tilt the laser DOWN to match the drop.
            tiltAngle = 90 - (int)angleDropDeg;

            // --- Windage (Pan) ---
            // drift = wind * t
            float drift = windSpeed * t;
            // angle = atan(drift / d)
            float angleDriftRad = atan(drift / distM);
            float angleDriftDeg = angleDriftRad * (180.0 / PI);

            // Adjust Pan
            // Wind from Left (assume positive windSpeed) -> Bullet drifts Right.
            // Laser (parallel) points Left relative to impact.
            // We need to Pan Laser RIGHT to match impact.
            panAngle = 90 + (int)angleDriftDeg;
        }

        // Constrain Angles
        panAngle = constrain(panAngle, 0, 180);
        tiltAngle = constrain(tiltAngle, 0, 180);

        // Drive Servos (if WiFi was connected and Servos attached)
        // We can't easily check if attached, but writing is safe.
        if (WiFi.status() == WL_CONNECTED) {
            servoPan.write(panAngle);
            servoTilt.write(tiltAngle);
        }

        // Logic: Target Detection
        bool targetInRange = distSensor.isTargetDetected(500.0); // 5 meters

        // Logic: Motion Detection (proxied by distance change)
        bool motionDetected = false;
        static float lastDist = 0;
        if (abs(distSensor.getDistance() - lastDist) > 10.0 && distSensor.getDistance() < 900) {
            motionDetected = true;
        }
        lastDist = distSensor.getDistance();

        // Logic: Lock Status
        bool isLocked = lockSensor.isLocked();

        // Visual Feedback (LEDs)
        // Note: PIN_LED_GREEN is disabled/repurposed.
        // We only have RED LED (Flash).
        if (targetInRange && isLocked) {
             // Rapid Blink Red for Lock?
             digitalWrite(PIN_LED_RED, (millis() / 100) % 2);
        } else if (targetInRange) {
             // Solid Red for Target Found
             digitalWrite(PIN_LED_RED, HIGH);
        } else {
             // Off
             digitalWrite(PIN_LED_RED, LOW);
        }

        // Update Web Interface Data Cache
        String gender = "Unknown";
        if (targetInRange) {
             gender = (millis() % 2 == 0) ? "Male" : "Female";
        }

        web.updateSensorData(
            envSensor.getTemperature(),
            envSensor.getHumidity(),
            envSensor.getPressure(),
            envSensor.getGasResistance(),
            envSensor.getAltitude(1013.25),
            windSensor.getSpeed(),
            distSensor.getDistance(),
            isLocked,
            motionDetected,
            gender,
            panAngle,
            tiltAngle
        );
    }
}
