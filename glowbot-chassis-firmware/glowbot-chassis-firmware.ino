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

// Imogens Libraries
#include <autoDelay.h>

autoDelay printDelay;
#define PRINT_DELAY_mS 1000  // Delay between debugging prints to serial monitor

#include "globals.h"









// Navigation Algorithm Variables/Constants
#define OBSTACLE_LIMIT_CM 50            //    Robot will enter obstacle avoidance mode if this limit is breached
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

    carStopNow();
    Serial.println("Obstacle Detected! ");
    avoidanceMode = true;
    avoidanceState = 0;

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
        carTurnLeft();                                                           // turn left
        if (distance_value > OBSTACLE_LIMIT_CM + OBSTACLE_AVOIDANCE_MODIFIER) {  // see if route is clear
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
        carTurnRight();                                                          // turn right
        if (distance_value > OBSTACLE_LIMIT_CM + OBSTACLE_AVOIDANCE_MODIFIER) {  // see if route is clear
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
        if (leftDistance >= rightDistance && leftDistance > OBSTACLE_LIMIT_CM + 5) {  // If left has greater distance and is greater than obsactle limit then
          carTurnLeft();
          if (navDelay.millisDelay(TURN_DELAY_mS * 2)) {  // Turn back to find the clear route
            carStopNow();
            avoidanceState = 4;
          }
        } else if (rightDistance > leftDistance && rightDistance > OBSTACLE_LIMIT_CM + 5) {  // else if right has greater distance then just exit avoidance mode
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
#define UNPARK_WAIT_mS 3000  // number of mS to wait on startup until car unparks

autoDelay stateDelay;  // delay to move between states using timer
//stateDelay.resetDelayTime_mS(); function that may be useful for testing delaytime



// INITIAL STATE
void sm_state_init() {
  if (lastState != smState) {  // Do anything here that needs to happen only once the state is entered for the first time
#if DEBUG_STATES == true
    Serial.println(F("State Machine: Init"));
#endif
    stateDelay.resetDelayTime_mS();
    lastState = smState;
  }
  if (carParked && !low_voltage_flag) {            // If car is parked and does not have low voltage detected
    if (stateDelay.millisDelay(UNPARK_WAIT_mS)) {  // Dleay for UNPARK_WAIT_mS milliseconds
      smState = STATE_UNPARK;
    }                                                       // Go to unpark state
  } else if (stateDelay.millisDelay(UNPARK_WAIT_mS * 2)) {  // else if timer runs out then go to wait state
    smState = STATE_WAIT;
  }
}

// Wait STATE
void sm_state_wait() {
  if (lastState != smState) {  // Do anything here that needs to happen only once the state is entered for the first time
#if DEBUG_STATES == true
    Serial.println(F("State Machine: Wait"));
#endif
    carStandby();
    //stateDelay.resetDelayTime_mS();
    lastState = smState;
  }
  // Function here to wait for button press, else is a dead end
  // Also standing the car up should trigger a move from this state to another state
  if (!carParked) {
    smState = STATE_FOLLOWLINE;
  }
}

// Unpark STATE
void sm_state_unpark() {
  if (lastState != smState) {  // Do anything here that needs to happen only once the state is entered for the first time
#if DEBUG_STATES == true
    Serial.println(F("State Machine: Unpark"));
#endif
    stateDelay.resetDelayTime_mS();

    lastState = smState;
  }
  unparkCar();
  if (!carParked) {
    smState = STATE_WAIT;
  }
}

// park STATE
void sm_state_park() {
  if (lastState != smState) {  // Do anything here that needs to happen only once the state is entered for the first time
#if DEBUG_STATES == true
    Serial.println(F("State Machine: Unpark"));
#endif
    stateDelay.resetDelayTime_mS();
    parkCar();
    lastState = smState;
  }
  if (carParked) {
    smState = STATE_WAIT;
  }
}


void sm_state_followline(int rightSense, int midSense, int leftSense, int direction) {
  if (lastState != smState) {  // Do anything here that needs to happen only once the state is entered for the first time
#if DEBUG_STATES == true
    Serial.println(F("State Machine: followline"));
#endif
    lastState = smState;
    
  }
  carForward();
  if (distance_value <= OBSTACLE_LIMIT_CM) {  // If obstacle is encountered
    // Provide clause to exit state
    // or provide following state to goto direct
    smState = STATE_PATHBLOCKED;
    nextState = STATE_FOLLOWLINE;  // tells statemachine where to go back to
  }
}

#define OBSTACLE_TIMEOUT_S 10

void sm_state_pathblocked() {
  if (lastState != smState) {  // Do anything here that needs to happen only once the state is entered for the first time
#if DEBUG_STATES == true
    Serial.println(F("State Machine: followline"));
#endif
    lastState = smState;
    carStandby();  // stops movement but keeps upright
    stateDelay.resetDelayTime_mS();
  }
  if (distance_value > OBSTACLE_LIMIT_CM + 5) {  // If obstacle is removed
    smState = nextState;                         // go to the state passed from the last one
  }
  if (stateDelay.secondsDelay(OBSTACLE_TIMEOUT_S)) {
    smState = STATE_PARK;
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
  if (low_voltage_flag || carParked) {
    //  carStopNow();                        // actually just want to disable motor output but still run state machine as is?
    digitalWrite(STBY_PIN, HIGH);  // otherwise could send to a stopped state
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
    case STATE_INIT:
      sm_state_init();
      break;
    case STATE_WAIT:
      sm_state_wait();
      break;
    case STATE_UNPARK:
      sm_state_unpark();
      break;
    case STATE_FOLLOWLINE:
      sm_state_followline(0, 0, 0, 0);
      break;
    case STATE_PATHBLOCKED:
      sm_state_pathblocked();
      break;
    default:
      Serial.print(F("Exception in State Machine, Unknown State: ["));
      Serial.print(smState);
      Serial.println("]");
       smState = STATE_INIT;
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
