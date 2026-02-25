# Created by Kaustav Ray
# Date: 25 Feb, 2026
# Copyright (c) 2026 Kaustav Ray. All Rights Reserved.

import argparse
import os
import sys
import time
import subprocess
import threading
import re

def clear_screen():
    os.system('cls' if os.name == 'nt' else 'clear')

def check_dependencies():
    print("[*] Checking for PlatformIO...")
    try:
        subprocess.check_call(["pio", "--version"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        print("[+] PlatformIO is installed.")
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("[-] PlatformIO not found. Attempting to install via pip...")
        try:
            subprocess.check_call([sys.executable, "-m", "pip", "install", "platformio"])
            print("[+] PlatformIO installed successfully.")
        except subprocess.CalledProcessError:
            print("[-] Failed to install PlatformIO. Please install it manually: 'pip install platformio'")
            sys.exit(1)

def flash_firmware():
    print("\n[!] PREPARING TO FLASH FIRMWARE")
    print("    1. Connect ESP32-CAM to computer via USB-TTL.")
    print("    2. CONNECT IO0 pin to GND (Put in Download Mode).")
    print("    3. Ensure no other serial monitors are open.")
    print("    4. AFTER UPLOAD: Disconnect IO0 from GND and press RESET button.")
    print("    5. WATCH THE SERIAL MONITOR OUTPUT FOR THE IP ADDRESS!")
    print("    6. PRESS CTRL+C TO EXIT THE MONITOR ONCE YOU HAVE THE IP.")

    input("    Press ENTER when ready to start flashing and monitoring...")

    print("\n[*] Flashing Firmware and Starting Monitor...")
    try:
        # Run PlatformIO upload and then monitor
        # This allows the user to see the IP address immediately after boot
        subprocess.call(["pio", "run", "--target", "upload", "--target", "monitor"])
        print("\n[+] Flashing/Monitoring session ended.")
    except subprocess.CalledProcessError:
        print("[-] Flashing Failed. Check connections and try again.")
        # Don't exit, maybe they just want to try entering IP manually
    except KeyboardInterrupt:
        print("\n[!] Monitor stopped.")

def main():
    clear_screen()
    print("========================================")
    print("      ESP32 SNIPER ADDON SYSTEM        ")
    print("           AUTOMATIC SETUP             ")
    print("========================================")

    check_dependencies()

    print("\n[?] Do you want to FLASH the firmware to the ESP32 now? (y/n)")
    print("    (Select 'y' if this is a new board or code update)")
    choice = input("> ").strip().lower()

    if choice == 'y':
        flash_firmware()
        print("\n[!] Did you catch the IP address from the Serial Monitor?")

    print("\n[?] Enter the ESP32 IP Address (e.g., 192.168.1.105): ")
    try:
        ip = input("> ").strip()
        if not ip:
            print("[-] No IP provided. Exiting.")
            sys.exit(1)

        print(f"\n[+] Configuring for {ip}...")
        time.sleep(1)

        print("[+] Launching Tactical Dashboard...")
        print("    (Press Ctrl+C to exit dashboard)")
        time.sleep(1)

        # Launch the client securely
        subprocess.run([sys.executable, "sniper_client.py", ip])

    except KeyboardInterrupt:
        print("\nExiting Setup...")
        sys.exit(0)

if __name__ == "__main__":
    main()
