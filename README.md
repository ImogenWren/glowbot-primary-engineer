# glowbot-primary-engineer
 Development Repo for Primary Engineer project - Glowbot

 ## System Description
_Glowbot is a 2 wheeled, reverse pendulumn robot capable of autonamous navigation around a domestic & controlled educational environment._

## System Diagram
![image](https://github.com/ImogenWren/glowbot-primary-engineer/assets/97303986/249ec820-b74b-4bbd-a434-96b13475d0b5)




### Major System Components
#### Robot Chassis
- Elegoo Tumbler Robot Kit, Available from Amazon for ~£50

 #### Microcontrollers
- Arduino Nano, main CPU for robot, supplied with kit and comes pre programmed with functional software including app integration.
     - Origional code will be stripped and re-written to suit new purpose
- ESP32 MCU as a communications & sensors package due to limited pins & processing power of arduino nano.
   - Note: Later plan to move main processing to ESP32, and reserve Arduino Nano for just control of the motors & accellerometer data.
- ESP32 Faerfly Smart LED development board for lighting control.

#### Sensors
- Accellerometer
- Line following IR sensor x3
- Ultrasound sensor
- Microswitch bumper x2
- Capacative touch sensor for activation

#### Output Devices
- Motor driver x2
- Robot chassis smart LED indicators
- ESPnow wireless communication transceiver

  


#### Remote Monitoring System
 - Remote Monitor & Raspberry Pi
 - ESP32 running UART bridge RX firmware

## Features
_The Following features are intended to be functional in the first iteration_
- Line Following using IR sensors
- Obstacle avoidance using Ultrasound sensors
- Obstacle detection using microswitch bumpers
- Glowing Head Nightlight
- Activation on touch sensor
- Wireless communication to "Monitoring System"

## Future Features
_The following features are strech or future goals that could be incorperated with this platform to improve functionality_
- Full two way communication between Base station & Vehicle, allow users from around the house to issue commands
- Screen or interactive LED face
- Natural Language processing for voice commands
- Additional ultrasound sensors for 360deg obstacle tracking
- Machine learning to build up picture of operational environment
- Machine Vision to aid in navigation, target following etc
- Audio output device

## Programming Instructions
 The robot chassis has 2 independent microcontrollers, the Arduino Nano, which for now acts as the main CPU, and the ESP32 which acts as a communications package, and as an interface for additional sensors.

### Programing the Arduino Nano: Main CPU
1. Remove the UART connection from pins 0 & 1 of the arduino nano. This is found on the **yellow** header on the rear side of the GlowBot Chassis
2. Open `glowbot-chassis-firmware in a suitable IDE, like the Arduino IDE 2.x
3. Program Arduino
4. Open Serial monitor at `baud=115200` and ensure that status messages are being printed
5. Power off system and replace the UART connection to pins 0 & 1. **Blue** to **Rx** (Pin 0) and **Green** to **Tx** (Pin 1)

### Programming the ESP32: Sensor & Communication Package
1. Program ESP32 with `esp32-firmware`
2. Plug USB into Arduino Nano, open Serial port at `baud=115200` and ensure that `line` data is being recieved by main MCU.
3. Power off system

### Programming the Remote Monitoring System
1. Download Repo https://github.com/ImogenWren/uart-esp-now-bridge
2. Open folder `esp-now-basic-rx` in suitable IDE
3. Program external ESP32 to act as Data Reciever
4. Open Serial monitor at `baud=115200`
5. Power on Glowbot
6. Ensure that Status messages are being printed
_Once ESP32 is programmed, it can be viewed by plugging into Raspberry Pi via USB & using the desktop icon `ESPnow Rx` (Double check this is correct). All UART messages sent by Arduino Nano CPU, should display in the terminal program._

