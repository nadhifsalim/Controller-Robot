# Wireless Gamepad-Controlled Robot

Welcome to the **Controller-Robot** repository! This project demonstrates how to build and program a remote-controlled robot using a standard Bluetooth gamepad (like a PlayStation 4/5, Xbox One, or Nintendo Switch Pro controller). 

This repository is designed with education in mind. Whether you are a beginner looking to understand wireless communication or a hobbyist building your first RC rover, this guide will walk you through the software and logic behind connecting a commercial gamepad to a microcontroller.

---

## What You Need to Install (Software Requirements)

To compile and upload the code to your robot, you will need to set up your development environment. This project relies on the **Bluepad32** library, which handles the complex Bluetooth pairing and data translation for you.

1. **[Arduino IDE](https://www.arduino.cc/en/software):** The main software used to write and upload the code to your microcontroller.
2. **ESP32 Board Package:** Because standard Arduino boards don't have built-in Bluetooth, this project uses an ESP32. You will need to add the ESP32 board manager URL to your Arduino IDE preferences and install the ESP32 package.
3. **[Bluepad32 Library](https://gitlab.com/ricardoquesada/bluepad32):** This is the magic behind the project. 
   - Open Arduino IDE.
   - Go to **Sketch** -> **Include Library** -> **Manage Libraries**.
   - Search for `Bluepad32` and click **Install**.

---

## Hardware Requirements (What You Need to Build)

While you can run the code just to test the controller connection, you will need the following hardware to build the actual robot:

*   **ESP32 Development Board** (The "brain" with built-in Bluetooth)
*   **Motor Driver Module** (e.g., L298N or TB6612FNG)
*   **2x or 4x DC Motors with Wheels**
*   **Robot Chassis**
*   **Power Source** (e.g., 18650 Li-ion batteries)
*   **A compatible Bluetooth Gamepad** (PS4, PS5, Xbox, Switch Pro)

---

## How to Use & Upload

1. **Clone this repository:**
   `git clone git@github.com:nadhifsalim/Controller-Robot.git`
2. **Open the sketch:** Open the `.ino` file in your Arduino IDE.
3. **Select your board:** Go to **Tools > Board** and select your specific ESP32 model (e.g., "DOIT ESP32 DEVKIT V1").
4. **Connect your ESP32:** Plug your ESP32 into your computer via USB and select the correct COM Port under **Tools > Port**.
5. **Upload:** Click the upload button.
6. **Pair your controller:** 
   - Turn on your ESP32.
   - Put your gamepad into Bluetooth pairing mode (e.g., holding Share + PS button on a DualShock 4).
   - The Bluepad32 firmware will automatically detect and connect to the gamepad.

---

## How the Code Works (Educational Breakdown)

If you are using this repo to learn, here is a quick overview of how the code is structured:

### 1. The Connection Loop
The ESP32 constantly listens for Bluetooth devices. When a controller enters pairing mode, Bluepad32 authenticates it and stores it in memory. The code checks the connection status every few milliseconds to ensure the controller hasn't disconnected.

### 2. Reading Inputs
Once connected, the code reads the state of the gamepad's joysticks and buttons. 
*   **Y-Axis (Left Joystick):** Typically used for moving Forward and Backward.
*   **X-Axis (Right Joystick):** Typically used for turning Left and Right.

### 3. Differential Drive Logic
To make the robot move naturally, the code mixes the X and Y axis values. 
*   Pushing the stick forward sends a `HIGH` signal to all motors.
*   Pushing the stick forward-right slows down the right motors and speeds up the left motors, creating a smooth turn.
These joystick values (usually ranging from -512 to 511) are mapped to PWM (Pulse Width Modulation) signals (0 to 255) to control the exact speed of the motors.

---

## Troubleshooting

*   **Controller won't connect:** Ensure no other devices (like your console or PC) are trying to connect to the controller. You may need to "forget" the controller on your other devices.
*   **Robot moves backward when pushing forward:** Simply swap the two motor wires connected to the motor driver for the side that is spinning backwards.
*   **Code won't compile:** Double-check that you have selected an ESP32 board in the Tools menu. Bluepad32 will throw errors if you try to compile it for a standard Arduino Uno.