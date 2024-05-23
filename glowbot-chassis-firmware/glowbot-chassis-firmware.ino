/* GlowBot Chassis Firmware
 * 
 *
 * @Description: Basic API for controlling reverse pendulum 2 wheeled robot
 * @Author: Imogen Heard
 * @Date: 2024-05-07

 */

/*  ## Version Control

### Pre-Alpha:0.0.1  
Data: 2024-05-22
`
Sketch uses 17294 bytes (56%) of program storage space. Maximum is 30720 bytes.
Global variables use 1655 bytes (80%) of dynamic memory, leaving 393 bytes for local variables. Maximum is 2048 bytes.
`
Notes:
- Saved 6% moving some text strings to flash

*/

// Program Options Enable/Disable
#define ENABLE_MOTOR_DRIVE false

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
#define PRINT_DELAY_mS 1000  // Delay between debugging sheduled prints to serial monitor

// Debugging & Serial Print options (Note - when UART bridge is attached all serial printouts will be transmitted to receiver)
#define PRINT_STATUS_UPDATE true   // prints periodic status messages to serial monitor
#define PRINT_UART_RX false        // prints out data recieved over UART connection
#define DEBUG_STATE_MACHINE false  // Prints output from state machine once per new state
#define DEBUG_STATES false         // prints debug output from individual states when enabled



// Navigation Algorithm Variables/Constants
#define OBSTACLE_LIMIT_CM 50  //    Robot will enter obstacle avoidance mode if this limit is breached


#include "globals.h"
#include "uartRX.h"
#include "lineFollower.h"



void setup() {
  Serial.begin(115200);
  ultrasonicInit();
  rgb.initialize();
  voltageInit();
  start_prev_time = millis();
  carInitialize();  // Sets balanceCar function as an interrupt routine
                    // motion_mode = STOP;  // added to try and stop it launching itself
}



/*

Defines switch case for state machine
*/

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
    lastState = smState;
  }
  parkCar();
  if (carParked) {
    smState = STATE_WAIT;
  }
}


void sm_state_followline(uint8_t left, uint8_t center, uint8_t right, uint8_t direction) {
  if (lastState != smState) {  // Do anything here that needs to happen only once the state is entered for the first time
#if DEBUG_STATES == true
    Serial.println(F("State Machine: followline"));
#endif
    lastState = smState;
  }

  bool line = lineFollow(left, center, right, direction);
  if (!line){
  //  smState = STATE_REVERSE;
  //  nextState = STATE_FOLLOWLINE;
  }
  if (distance_value <= OBSTACLE_LIMIT_CM) {  // If obstacle is encountered
    // Provide clause to exit state
    // or provide following state to goto direct
    smState = STATE_PATHBLOCKED;
    nextState = STATE_FOLLOWLINE;  // tells statemachine where to go back to
  }
}

#define OBSTACLE_TIMEOUT_S 5

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
  if (stateDelay.secondsDelay(OBSTACLE_TIMEOUT_S)) {  // if timeout is reached, park the robot
    smState = STATE_REVERSE;
  }
}


void sm_state_reverse() {
  if (lastState != smState) {  // Do anything here that needs to happen only once the state is entered for the first time
#if DEBUG_STATES == true
    Serial.println(F("State Machine: reverse"));
#endif
    lastState = smState;
    carBackward();  // stops movement but keeps upright
    stateDelay.resetDelayTime_mS();
  }
  if (distance_value > OBSTACLE_LIMIT_CM + 20) {  // If obstacle is removed
    smState = nextState;                          // go to the state passed from the last one
  }
  if (stateDelay.secondsDelay(OBSTACLE_TIMEOUT_S)) {  // if timeout is reached, park the robot
    smState = nextState;
    // motion_mode = REVERSE;
  }
}


void sm_run(void) {
  // Do all Functions that happen in every state:
 // getDistance();
 distance_value = 55.0;
  setMotionState();
  printStatus();
  voltageMeasure();
  checkParked();
  uartRxStructure();  // Recieve UART Data, these two functions could be tied into one call
  parseUARTdata();    // Parse UART data into required variables

  // Place any Movement disabling code here
  if (low_voltage_flag || carParked) {
    //  carStopNow();                        // actually just want to disable motor output but still run state machine as is?
    //  digitalWrite(STBY_PIN, HIGH);  // otherwise could send to a stopped state
  }

  // For debugging state machine
#if DEBUG_STATE_MACHINE == true
  if (lastState != smState) {
    Serial.print(F("{\"State\":\""));
    Serial.print(stateNames[smState]);
    Serial.println(F("\"}"));
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
    case STATE_PARK:
      sm_state_park();
      break;
    case STATE_FOLLOWLINE:
      sm_state_followline(leftLine, centerLine, rightLine, lineDirection);
      break;
    case STATE_PATHBLOCKED:
      sm_state_pathblocked();
      break;
    case STATE_REVERSE:
      sm_state_reverse();
      break;
    default:
      Serial.print(F("Exception in State Machine, Unknown State: ["));
      Serial.print(smState);
      Serial.println(F("]"));
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
