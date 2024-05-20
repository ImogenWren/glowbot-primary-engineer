/* GlowBot Chassis Firmware
 * 
 *
 * @Description: Basic API for controlling reverse pendulum 2 wheeled robot
 * @Author: Imogen Heard
 * @Date: 2024-05-07

 */
#include <Arduino.h>
#include "Pins.h"
#include "mode.h"
#include "Command.h"
#include "BalanceCar.h"
#include "Rgb.h"
#include "Ultrasonic.h"
#include "voltage.h"
#include "EnableInterrupt.h"

unsigned long start_prev_time = 0;
boolean carInitialize_en = true;

// Imogens Libraries
#include <autoDelay.h>

autoDelay printDelay;
#define PRINT_DELAY_mS 100

autoDelay stateChangeDelay;
#define STATE__CHANGE_TIMER_S 2


void modes() {  // this is not a working function, just a way to group things found in the origional code
  obstacleAvoidanceMode();
  followMode();
  followMode2();
}


void setMotionState() {
  switch (motion_mode) {
    case FORWARD:
      setting_car_speed = 40;
      setting_turn_speed = 0;
      break;
    case BACKWARD:
      setting_car_speed = -40;
      setting_turn_speed = 0;
      break;
    case TURNLEFT:
      setting_car_speed = 0;
      setting_turn_speed = 50;
      break;
    case TURNRIGHT:
      setting_car_speed = 0;
      setting_turn_speed = -50;
      break;
    case STANDBY:
      setting_car_speed = 0;
      setting_turn_speed = 0;
      break;
    case STOP:
      if (millis() - start_prev_time > 1000) {
        function_mode = IDLE;
        if (balance_angle_min <= kalmanfilter_angle && kalmanfilter_angle <= balance_angle_max) {
          motion_mode = STANDBY;
          rgb.lightOff();
        }
      }
      break;
    case START:
      if (millis() - start_prev_time > 2000) {
        if (balance_angle_min <= kalmanfilter_angle && kalmanfilter_angle <= balance_angle_max) {
          car_speed_integeral = 0;
          setting_car_speed = 0;
          motion_mode = STANDBY;
          rgb.lightOff();
        } else {
          motion_mode = STOP;
          carStop();
          rgb.brightRedColor();
        }
      }
      break;
    default:
      Serial.println("Error in State Machine");
      break;
  }
}


// Are these for changing mode?

void carStandby() {
  rgb.lightOff();
  motion_mode = STANDBY;
}


void carForward() {
  rgb.flashBlueColorFront();
  motion_mode = FORWARD;
}

void carBackward() {
  rgb.flashBlueColorback();
  motion_mode = BACKWARD;
}



void carTurnLeft() {
  rgb.flashBlueColorLeft();
  motion_mode = TURNLEFT;
}


void carTurnRight() {
  rgb.flashBlueColorRight();
  motion_mode = TURNRIGHT;
}





void lightShow() {
  rgb.flag++;
  if (rgb.flag > 6) {
    rgb.flag = 1;
  }
  rgb.brightness = 0;
  rgb.setBrightness(rgb.brightness);
  rgb.show();
  // Switch case for rgb.flag?
  // rgb.theaterChaseRainbow(50);
  // rgb.rainbowCycle(20);
  // rgb.theaterChase(127, 127, 127, 50);
  // rgb.rainbow(20);
  // rgb.whiteOverRainbow(20, 30, 4);
  // rgb.rainbowFade2White(3, 50, 50);
}


void parkCar() {
  function_mode = IDLE;
  motion_mode = STOP;
  carBack(110);
  delay((kalmanfilter_angle - 30) * (kalmanfilter_angle - 30) / 8);
  carStop();
  start_prev_time = millis();
  rgb.brightRedColor();
}



void unparkCar() {
  if (millis() - start_prev_time > 500 && kalmanfilter_angle >= balance_angle_min) {
    start_prev_time = millis();
    motion_mode = START;
  }
  motion_mode = START;
}


void setup() {
  Serial.begin(115200);
  ultrasonicInit();
  // keyInit(); bluetooth crap
  rgb.initialize();
  voltageInit();
  start_prev_time = millis();
  carInitialize();
}


void printStatus() {
  if (printDelay.millisDelay(PRINT_DELAY_mS)) {
    Serial.println(kalmanfilter.angle);
  }
}

int currentState = 0;
void moveDemo() {
  if (stateChangeDelay.secondsDelay(STATE__CHANGE_TIMER_S)){
    currentState++;
    if (currentState > 5) {
      currentState = 0;
    }
    switch (currentState) {
      case 0:
        carForward();
        break;
      case 1:
        carTurnRight();
        break;
      case 2:
        carForward();
        break;
      case 3:
        carTurnLeft();
        break;
      case 4:
        carBackward();
        break;
      case 5:
        parkCar();   // This just falls over
        break;        
      default:
        break;
    }
  }
}

void loop() {
  moveDemo();  // runs through movement states as demo
  // getKeyValue();  just used for bluetooth
  //getBluetoothData();
  //  keyEventHandle();  just IR remote stufff
  getDistance();  // TODO: Check this is working
  voltageMeasure();
  setMotionState();
  // functionMode();
  checkObstacle();
  rgb.blink(100);
  printStatus();
  static unsigned long start_time;
  if (millis() - start_time < 10) {
    function_mode = IDLE;
    motion_mode = STOP;
    carStop();
  }

  if (millis() - start_time == 2000) {  // Enter the pendulum, the car balances...
    unparkCar();
  }
}
