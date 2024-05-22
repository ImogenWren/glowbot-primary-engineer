# glowbot-primary-engineer
 Development Repo for Primary Engineer project - Glowbot

 ## System Description
_Glowbot is a 2 wheeled, reverse pendulumn robot capable of autonamous navigation around a domestic & controlled educational environment._

### Major System Components
#### Robot Chassis
- Elegoo Tumbler Robot Kit, Available from Amazon for ~£50
- Arduino Nano, main CPU for robot, supplied with kit and comes pre programmed with well made firmware installed.
- ESP32 MCU as a communications & sensors package due to limited pins & processing power of arduino nano.
  - Note: Later plan to move main processing to ESP32, and reserve Arduino Nano for just control of the motors & accellerometer data.
  - 
#### Remote Monitoring System
 - Remote Monitor & Raspberry Pi
 - ESP32 running UART bridge RX firmware


## System Diagram

