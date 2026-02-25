# ESP32 Sniper Addon System

**Turn your airsoft or simulation gear into a smart, tactical system!**

This project uses an ESP32-CAM to add high-tech features to your scope. It can measure distance, wind speed, and even automatically adjust a laser or scope to help you hit your target.

---

## 🎯 What Does It Do?

*   **Smart Aiming**: Automatically moves a laser pointer or scope up/down and left/right based on distance and wind. No more guessing!
*   **Distance Finder**: Uses an ultrasonic sensor to tell you exactly how far away the target is.
*   **Wind Reader**: Measures wind speed to help you correct your aim.
*   **Weather Station**: Shows temperature, humidity, and air pressure.
*   **Live Camera**: View a live video feed from the scope on your phone or computer.
*   **Target Lock**: A physical switch lets you "lock" onto a target.

---

## 🛠️ What You Need (Parts List)

To build this, you will need a few electronic parts:

1.  **ESP32-CAM Board** (The brain of the system)
2.  **BME680 Sensor** (For weather data like temp/pressure)
3.  **HC-SR04 Sensor** (Ultrasonic distance sensor)
4.  **Wind Speed Sensor** (Analog type, 0-3.3V output)
5.  **2x Micro Servos** (Small motors to move the laser/scope)
6.  **Red LED** (Light for status)
7.  **Switch** (Simple toggle switch for locking)
8.  **Power Supply** (5V battery or USB power bank)

---

## 🚀 How to Install (Easy Mode)

We made a special script to do almost everything for you!

### Step 1: Download the Code
Click the green "Code" button above and select "Download ZIP", or run this command if you know Git:
```bash
git clone https://github.com/filestore4u/perfectaim.git
```

### Step 2: Connect Your ESP32
Plug your ESP32-CAM into your computer using a USB cable.
*   **Important**: Make sure pin **IO0** is connected to **GND** before plugging it in to put it in "Download Mode".

### Step 3: Run the Setup Script
Open your terminal (Command Prompt on Windows) inside the project folder and run:
```bash
python setup.py
```
*   Follow the instructions on the screen.
*   It will install necessary tools, upload the code to your ESP32, and even start the dashboard for you!

---

## 🔌 Wiring Guide (Where do wires go?)

Connect your parts to these pins on the ESP32-CAM:

| Part | Pin on Part | Pin on ESP32 | Note |
| :--- | :--- | :--- | :--- |
| **Weather Sensor** (BME680) | SDA | **14** | |
| | SCL | **15** | |
| **Distance Sensor** (HC-SR04) | Trigger | **12** | |
| | Echo | **13** | |
| **Wind Sensor** | Signal | **33** | |
| **Left/Right Motor** (Pan Servo) | Signal | **1** | **Warning**: This pin is also used for uploading code! |
| **Up/Down Motor** (Tilt Servo) | Signal | **2** | |
| **Lock Switch** | Signal | **3** | |
| **Status Light** (Red LED) | Positive (+) | **4** | |

**⚠️ Important Note**:
Because we use so many features, the **Serial Monitor** (the text output on your computer) will only work for the first 10 seconds after turning it on. After that, it turns off so the motors can work. **Write down the IP address quickly!**

---

## 🎮 How to Use

1.  **Power On**: Connect your battery.
2.  **Connect**: The ESP32 will connect to your WiFi.
3.  **Dashboard**: Open the `sniper_client.py` script (or let the setup script do it) to see the live dashboard on your computer.
    *   **Status**: Shows if you are "Scanning" or "Locked".
    *   **Environment**: Shows temp, humidity, and wind.
    *   **Ballistics**: Shows how much the system is adjusting your aim.
4.  **Aim & Fire**: Point at your target. The system will measure the distance and adjust your laser automatically!

---

## 📄 License & Credits
**Created by Kaustav Ray.**
**Date: 25 Feb, 2026**

This project is for **personal use only**.
**Copyright (c) 2026 Kaustav Ray. All Rights Reserved.**
