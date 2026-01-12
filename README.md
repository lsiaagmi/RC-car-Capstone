# WiFi-Controlled RC Utility Vehicle

A custom-built, dual-microcontroller RC vehicle featuring a functional arm, claw, and storage container, utilizing TCP/IP over WiFi for control.

## Introduction
This project involved the design and development of an RC vehicle capable of high-speed navigation and object manipulation. Unlike standard RC kits, this system uses a custom-built controller and a peer-to-peer **WiFi (TCP)** connection. This architecture allows for expanded software features, such as custom button mapping and integrated logic for accessory control.

## System Architecture
The control system bypasses traditional radio signals in favor of a robust local network.
* **Protocol:** TCP/IP over 2.4GHz WiFi.
* **Server:** The car acts as a WiFi Access Point (AP).
* **Client:** The custom controller connects to the car's AP to transmit data.
* **Data Mapping:** Analog joystick values ($0-1023$) are mapped to $0-255$ to fit within a 1-byte message array for efficient TCP transmission.

## Hardware Specifications

### Electronics
| Component | Specification |
| :--- | :--- |
| **Microcontrollers** | 2x Arduino Uno R4 WiFi (Renesas + ESP32) |
| **Motor** | SunnySky X2216 Brushless Motor (1100KV) |
| **ESC** | Hobbywing Skywalker 80A V2 with BEC |
| **Battery** | 3S LiPO (11.1V) with XT-60 Connectors |
| **Phone Battery Charger** | Qualcomm phone charger powers the Car's Arduino|
| **Servos** | Metal-gear 270° High-Torque Servos |

### Mechanical Design
* **Drivetrain:** Rear-wheel drive with a differential. The reduction stage shaft is rigidly constrained with custom spacers to handle brushless torque.
* **Steering:** Servo-driven linkage system. Range is software-limited to ensure axles remain seated in the universal joint cups.
* **Modular Container:** A multi-part interlocking design created to reduce 3D printing failure risks and provide easy access to the battery and USB-C ports.

## Technical Skills & Iterations
* **EMI Shielding:** Implemented a Faraday-style shield using aluminum tape around the motor to block EMF interference from affecting the Arduino logic.
* **Rapid Prototyping:** Used Fusion360 to iterate on fragile steering arms and universal joint cups, incorporating mid-print bearing insertion for maximum precision.
* **Circuit Design:** Developed pull-up circuits with $10k\Omega$ resistors for all digital buttons to ensure clean, bounce-free signal transitions.

## Testing & Troubleshooting
- [x] **EMF Mitigation:** Solved unpredictable servo twitching by shielding the motor housing and reducing steering servo commands.
- [x] **Drivetrain Stress Test:** Verified motor torque and corrected gear clearance issues in the reduction stage.
- [x] **WiFi Stability:** Confirmed smooth joystick data transmission and low-latency response.
- [ ] **Servo Bugginess:** Tested and discovered the root was due to poor logic and a lack of power to the Arduino.
- [ ] **Button Bugginess:** Buttons are not always detected perfectly; however, only the debounce time needs to be tuned.

## Reflection
### Successes
* Established a functional TCP server-client relationship for low-latency wireless control.
* Successfully adapted a complex drivetrain to handle a powerful brushless motor.
* Modular CAD design allowed for localized repairs without reprinting the entire chassis.

### Lessons Learned
* **Structural Stress:** Installation stress often exceeds operational stress; parts like steering arms require extra reinforcement at the joints.
* **Power Sequencing:** The Arduino WiFi server must be initialized before the ESC to ensure a proper connection "handshake."

## Next Steps
1.  **Isolate Servo Power:** Troubleshoot the remaining random servo movements by checking for voltage drops or signal cross-talk.
2.  **ESC Reverse Programming:** Verify the reverse-thrust logic using the auxiliary signal wire and pulse-width manipulation.
3.  **Controller Ergonomics:** Redesign the controller housing to optimize wire routing between the batteries and the Arduino.

---
*Created by Liam Sagi - 2026*
