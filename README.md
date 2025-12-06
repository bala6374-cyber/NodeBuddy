# NodeBuddy – Web-Based OS for ESP8266 (NodeMCU)

NodeBuddy is a lightweight web-based operating system for ESP8266 boards.  
It provides a browser UI that lets you control hardware, view system stats, edit files, and update firmware.

## ✨ Features
- Web-based desktop UI
- WiFi configuration page
- LED + GPIO control app
- System monitor (RAM/Flash/WiFi status)
- Key-value storage app
- File editor using SPIFFS
- OTA Firmware update
- REST API for automation

## 🛠 Hardware Required
- ESP8266 NodeMCU
- USB Cable
- Optional LEDs, sensors, relays

## 📡 Default WiFi AP Mode
- SSID: `NodeBuddy-Setup`
- Password: `nodebuddy123`

## 🌐 Web Apps Included
- Desktop home screen
- LED Controller
- File Manager
- System Monitor
- Settings
- OTA Updater

## 🔧 How to Upload
1. Install ESP8266 board in Arduino IDE  
2. Open `NodeBuddy.ino`  
3. Select:
   - **Board:** NodeMCU 1.0 (ESP-12E Module)
   - **Flash Size:** 4MB (FS:2MB)
4. Upload Sketch
5. Upload SPIFFS Data using “ESP8266 Sketch Data Upload”

## 📄 License
MIT License
