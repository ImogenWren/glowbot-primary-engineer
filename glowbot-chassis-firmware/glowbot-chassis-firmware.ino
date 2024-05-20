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

bool carParked = false;

// Imogens Libraries
#include <autoDelay.h>

autoDelay printDelay;
#define PRINT_DELAY_mS 1000

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
      Serial.println("Error in Motion State Machine");
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

void carStopNow() {
  rgb.flashYellowColorFront();
  rgb.flashYellowColorback();
  motion_mode = STOP;
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
  // carForward(210);
  //  delay((kalmanfilter_angle - 30) * (kalmanfilter_angle - 30) / 8);
  carStop();  // This causes it to fall over frontways
  start_prev_time = millis();
  rgb.brightRedColor();
}



void unparkCar() {
  setting_car_speed = 40;  // bug fix to try and stop it toppeling over on start
  if (millis() - start_prev_time > 500 && kalmanfilter_angle >= balance_angle_min) {
    start_prev_time = millis();
    motion_mode = START;
  }
  motion_mode = START;
}






void printStatus() {
  if (printDelay.millisDelay(PRINT_DELAY_mS)) {
    Serial.print(F("Motion Mode: "));
    Serial.print(motionModeName[motion_mode]);
    Serial.print(" Gyro Angle: ");
    Serial.print(kalmanfilter.angle);
    Serial.print(", Distance: ");
    Serial.print(distance_value);
    Serial.print(" cm");
    Serial.print(", Volts: ");
    Serial.print(voltage);
    Serial.print(" V ");
    if (low_voltage_flag) {
      Serial.print("Low Voltage Detected!");
    }
    if (carParked){
      Serial.print("Car Parked");
    }
    Serial.println("");
  }
}





#define OBSTACLE_LIMIT 50               //    Robot will enter obstacle avoidance mode if this limit is breached
#define OBSTACLE_AVOIDANCE_MODIFIER 80  // Robot will exit obstacle avoidance mode if path of LIMIT + MODIFIER is detected, otherwise will follow whole procedure
#define NUMBER_ATTEMPTS 3

bool avoidanceMode = false;
float leftDistance = 0;
float rightDistance = 0;

int avoidanceState = 0;
int lastAvoidanceState = 0;
autoDelay navDelay;

#define TURN_DELAY_mS 2000
#define BACK_UP_DELAY_mS 1000

int avoidanceAttempts = 0;

bool directionAlternator = true;  // this helps altenerate the first turn



void navDemo() {
  if (!avoidanceMode) {  // No obstical detected - go forwards
    carForward();
    if (distance_value <= OBSTACLE_LIMIT) {  // If obstacle is encountered
      carStopNow();
      Serial.println("Obstacle Detected! ");
      avoidanceMode = true;
      avoidanceState = 0;
    }
  } else {
    // Run obstacle avoidance routine
    switch (avoidanceState) {
      case 0:  // Back up from obstacal
        if (lastAvoidanceState != avoidanceState) {
          Serial.println("Case 0: Backing Up");
          navDelay.resetDelayTime_mS();  // needed to make sure we can use multiple calls to navDelay, the delay time will be from when the reset method is called.
        }
        carBackward();
        if (navDelay.millisDelay(BACK_UP_DELAY_mS)) {
          Serial.println("Case 0: Finished Backing Up");
          carStopNow();
          avoidanceAttempts++;
          if (directionAlternator) {
            avoidanceState = 1;
          } else {
            avoidanceState = 2;
          }
        }
        break;
      case 1:
        if (lastAvoidanceState != avoidanceState) {
          Serial.println("Case 1: Turning Left");
          navDelay.resetDelayTime_mS();
        }
        carTurnLeft();                                                        // turn left
        if (distance_value > OBSTACLE_LIMIT + OBSTACLE_AVOIDANCE_MODIFIER) {  // see if route is clear
          avoidanceState = 4;
        } else if (navDelay.millisDelay(TURN_DELAY_mS)) {  // else turn until timeout
          carStopNow();
          if (directionAlternator) {
            avoidanceState = 2;  // If left is checked first then go to turn right
          } else {
            avoidanceState = 3;  // else go to comparison case
          }
        }
        leftDistance = distance_value;
        break;
      case 2:
        if (lastAvoidanceState != avoidanceState) {
          Serial.println("Case 2: Turning Right");
          navDelay.resetDelayTime_mS();
        }
        carTurnRight();                                                       // turn right
        if (distance_value > OBSTACLE_LIMIT + OBSTACLE_AVOIDANCE_MODIFIER) {  // see if route is clear
          avoidanceState = 4;
        } else if (navDelay.millisDelay(TURN_DELAY_mS * 2)) {  // else turn back past origional position to check other side
          carStopNow();
          if (directionAlternator) {
            avoidanceState = 3;  // If left is checked first then go to comparason
          } else {
            avoidanceState = 2;  // else go turn left state
          }
        }
        rightDistance = distance_value;
        break;

      case 3:
        if (lastAvoidanceState != avoidanceState) {
          Serial.println("Case 3: Deciding direction");
          navDelay.resetDelayTime_mS();
        }
        if (leftDistance >= rightDistance && leftDistance > OBSTACLE_LIMIT + 5) {  // If left has greater distance and is greater than obsactle limit then
          carTurnLeft();
          if (navDelay.millisDelay(TURN_DELAY_mS * 2)) {  // Turn back to find the clear route
            carStopNow();
            avoidanceState = 4;
          }
        } else if (rightDistance > leftDistance && rightDistance > OBSTACLE_LIMIT + 5) {  // else if right has greater distance then just exit avoidance mode
          carStopNow();
          avoidanceState = 4;
        } else {
          carStopNow();
          if (avoidanceAttempts < NUMBER_ATTEMPTS) {
            avoidanceState = 0;  // If no solution is found, go to start and try again
          } else {
            avoidanceState = 5;
          }
        }
        break;
      case 4:  // exist avoidance mode
        Serial.println("Case 4: Obstacle Avoided");
        avoidanceMode = false;
        avoidanceState = 0;
        directionAlternator = !directionAlternator;  // flip the bool so the check is performed in the opposite direction next time
        break;
      case 5:  // exist avoidance mode
        if (lastAvoidanceState != avoidanceState) {
          Serial.println("Case 5: Backing Up Further");
          navDelay.resetDelayTime_mS();
        }
        rgb.brightRedColor();
        carBackward();
        if (navDelay.millisDelay(BACK_UP_DELAY_mS * 3)) {
          avoidanceState = 0;
        }
        break;
      default:
        break;
    }
    lastAvoidanceState = avoidanceState;
  }
}




void setup() {
  Serial.begin(115200);
  ultrasonicInit();
  rgb.initialize();
  voltageInit();
  start_prev_time = millis();
  carInitialize();     // Sets balanceCar function as an interrupt routine
  motion_mode = STOP;  // added to try and stop it launching itself
}

/*

Defines switch case for state machine
*/
#define DEBUG_STATE_MACHINE true

autoDelay stateDelay;  // delay to move between states using timer
//stateDelay.resetDelayTime_mS(); function that may be useful for testing delaytime

/**
 * Defines the valid states for the state machine
 * 
 */
typedef enum {
  STATE_INIT,
  STATE_WAIT,
  STATE_UNPARK,
  STATE_PARK,
  STATE_FORWARD,
  STATE_BACKWARD,
  STATE_TURNLEFT,
  STATE_TURNRIGHT,
  STATE_ESTOP
} StateType;


char stateNames[][20] = {
  "STATE_INIT",
  "STATE_WAIT",
  "STATE_UNPARK",
  "STATE_PARK",
  "STATE_FORWARD",
  "STATE_BACKWARD",
  "STATE_TURNLEFT",
  "STATE_TURNRIGHT",
  "STATE_ESTOP"
};

/**
 * Stores the current, and previous state of the state machine
 */

StateType smState = STATE_INIT;  // define the initial state
StateType lastState;



void sm_state_acc(float acc) {
  if (lastState != smState) {  // Do anything here that needs to happen only once the state is entered for the first time
#if DEBUG_STATES == true
    Serial.print(F("State Machine: Accelleration set to: ["));
    Serial.print(acc);
    Serial.println("]");

#endif
    lastState = smState;
  }
  // Provide clause to exit state
  // or provide following state to goto direct
  smState = STATE_WAIT;
}



void checkParked() {
  if (kalmanfilter.angle >= 28 || kalmanfilter.angle <= -27) {
    carParked = true;
  } else {
    carParked = false;
  }
}

void sm_run(void) {
  // Do all Functions that happen in every state:
  getDistance();
  setMotionState();
  printStatus();
  voltageMeasure();
  checkParked();

  // Place any Movement disabling code here
  if (low_voltage_flag || carParked){
  //  carStopNow();                        // actually just want to disable motor output but still run state machine as is?
    digitalWrite(STBY_PIN, HIGH);
  }

  // For debugging state machine
#if DEBUG_STATE_MACHINE == true
  if (lastState != smState) {
    Serial.print("{\"State\":\"");
    Serial.print(stateNames[smState]);
    Serial.println("\"}");
  }
#endif

  // Run State Machine
  switch (smState) {
    case 0:
      break;
    default:
      //     Serial.print(F("Exception in State Machine, Unknown State: ["));
      //    Serial.print(smState);
      //    Serial.println("]");
      break;
  }
}


void loop() {
  // navDemo();

  // rgb.blink(100);

  sm_run();


  static unsigned long start_time;


  // if (millis() - start_time < 10) {  // Weird way of ensuring that the car stays in IDLE mode while starting up?
  //   function_mode = IDLE;
  //   motion_mode = STOP;
  //  carStop();
  // }

  // if (millis() - start_time == 2000) {  // Enter the pendulum, the car balances...
  //    unparkCar();
  //  }
}
