# Anti-Theft Vehicle Security System using ESP8266 NodeMCU

## Project Overview
Developed an anti-theft vehicle security system with real-time GPS tracking and vehicle control using ESP8266 NodeMCU, SIM900A GSM module, and GPS module.

## Features
- Sends SMS alert when vehicle starts moving with GPS coordinates.
- Provides periodic location updates every 90 seconds.
- Receives "STOP" SMS command to stop the motor and vehicle.
- Uploads location and status data to ThingSpeak for online monitoring.

## Components Required
- ESP8266 NodeMCU V3
- SIM900A GSM Module
- NEO-6M GPS Module
- L298N Motor Driver
- DC Motor
- Power supply (5V for SIM900A, 3.3V for NodeMCU & GPS)
- Connecting wires and breadboard

## Wiring Diagram

| Module       | NodeMCU Pin          |
|--------------|----------------------|
| SIM900A RX   | D6 (GPIO12)          |
| SIM900A TX   | D5 (GPIO14)          |
| GPS RX       | D7 (GPIO13)          |
| GPS TX       | D8 (GPIO15)          |
| Motor IN1    | D1 (GPIO5)           |
| Motor IN2    | D2 (GPIO4)           |
| Motor ENA    | D4 (GPIO2)           |
| SIM900A VCC  | External 5V supply   |
| GPS VCC      | 3.3V                 |
| GND          | Common Ground        |

> **Note:** Disconnect external modules during code upload to avoid interference.

## Library Dependencies
- [TinyGPSPlus](https://github.com/mikalhart/TinyGPSPlus)
- EspSoftwareSerial (included with ESP8266 Arduino Core)
- ESP8266WiFi (built-in)

Install these libraries via Arduino IDE Library Manager if not already installed.

## How to Upload and Run
1. Open `VehicleSecuritySystem.ino` in Arduino IDE.
2. Select "NodeMCU 1.0 (ESP-12E Module)" as board.
3. Choose correct COM port.
4. Disconnect SIM900A, GPS, and motor modules from NodeMCU pins.
5. Upload the code.
6. After successful upload, connect all external modules as per wiring diagram.
7. Power everything appropriately.
8. The system will send SMS alerts and update ThingSpeak automatically.

## Troubleshooting
- Ensure stable 5V power for SIM900A.
- Use external power for motors and GSM module; NodeMCU’s 3.3V pin is insufficient.
- Check GPS fix before movement notifications.
- Monitor serial output for debugging info.

- ![WhatsApp Image 2025-08-01 at 08 49 41_ad0344ad](https://github.com/user-attachments/assets/24402634-63a1-4563-b1fc-3da7743b2469)


---

*Feel free to contribute or raise issues!*

