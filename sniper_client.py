# Created by Kaustav Ray
# Date: 25 Feb, 2026
# Copyright (c) 2026 Kaustav Ray. All Rights Reserved.

import argparse
import urllib.request
import urllib.error
import json
import time
import os
import sys

def clear_screen():
    # Cross-platform clear screen
    os.system('cls' if os.name == 'nt' else 'clear')

def get_data(ip):
    url = f"http://{ip}/api/data"
    try:
        with urllib.request.urlopen(url, timeout=2) as response:
            if response.status == 200:
                data = response.read().decode('utf-8')
                return json.loads(data)
            else:
                return None
    except (urllib.error.URLError, json.JSONDecodeError):
        return None
    except Exception as e:
        # print(f"Error: {e}") # Debug off for clean dashboard
        return None

def display_dashboard(data):
    clear_screen()
    print("========================================")
    print("      ESP32 SNIPER ADDON SYSTEM        ")
    print("      AUTOMATIC BALLISTICS ENGINE      ")
    print("========================================")

    if data:
        # Status indicators
        lock_status = "LOCKED" if data.get('locked') else "SCANNING"
        motion_status = "DETECTED" if data.get('motion') else "NONE"

        print(f"STATUS:       [{lock_status}]")
        print(f"MOTION:       [{motion_status}]")
        print("----------------------------------------")
        print("ENVIRONMENT:")
        print(f"  Temp:       {data.get('temp', 0):.1f} C")
        print(f"  Humidity:   {data.get('hum', 0):.1f} %")
        print(f"  Pressure:   {data.get('pres', 0):.1f} hPa")
        print(f"  Altitude:   {data.get('alt', 0):.1f} m")

        # Calculate Air Density
        # rho = p / (R * T) -> p in Pa, T in Kelvin. R = 287.058
        temp_c = data.get('temp', 20)
        pres_hpa = data.get('pres', 1013)
        try:
            rho = (pres_hpa * 100) / (287.058 * (temp_c + 273.15))
        except ZeroDivisionError:
            rho = 0

        print(f"  Air Density:{rho:.3f} kg/m^3")
        print("----------------------------------------")
        print("BALLISTICS & SERVO CONTROL:")
        print(f"  Wind Speed: {data.get('wind', 0):.1f} m/s")
        dist = data.get('dist', 0)
        dist_m = dist / 100.0
        dist_str = "OUT OF RANGE" if dist > 90000 else f"{dist_m:.2f} m" # 900m = 90000cm
        print(f"  Distance:   {dist_str}")

        # Servo Data
        pan = data.get('pan', 90)
        tilt = data.get('tilt', 90)
        print(f"  Windage (Pan):   {pan - 90:+.1f} deg")
        print(f"  Elevation (Tilt):{tilt - 90:+.1f} deg")

        print("========================================")
        print("Press Ctrl+C to exit.")
    else:
        print("Connection Lost... Retrying...")

def main():
    parser = argparse.ArgumentParser(description="ESP32 Sniper Addon Client")
    parser.add_argument("ip", help="IP Address of the ESP32 device")
    args = parser.parse_args()

    print(f"Connecting to {args.ip}...")

    try:
        while True:
            data = get_data(args.ip)
            display_dashboard(data)
            time.sleep(1)
    except KeyboardInterrupt:
        print("\nExiting...")
        sys.exit(0)

if __name__ == "__main__":
    main()
