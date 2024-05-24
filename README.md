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

