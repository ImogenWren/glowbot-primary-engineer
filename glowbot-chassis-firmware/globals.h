
#pragma once


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
  STATE_REVERSE,
  STATE_TURNLEFT,
  STATE_TURNRIGHT,
  STATE_FOLLOWLINE,
  STATE_PATHBLOCKED,
  STATE_ESTOP
} StateType;

/**
 * Stores the current, and previous state of the state machine
 */

StateType smState = STATE_INIT;  // define the initial state
StateType lastState;
StateType nextState;  // rarely used, but useful to recycle some states



char stateNames[][20] = {
  "STATE_INIT",
  "STATE_WAIT",
  "STATE_UNPARK",
  "STATE_PARK",
  "STATE_FORWARD",
  "STATE_REVERSE",
  "STATE_TURNLEFT",
  "STATE_TURNRIGHT",
  "STATE_FOLLOWLINE",
  "STATE_PATHBLOCKED",
  "STATE_ESTOP"
};


typedef enum {
  LINE_LOST,
  LINE_CAPTURED,
  LINE_FOUND_LEFT,
  LINE_FOUND_RIGHT
} lineState;

lineState lnState;

char lineStateName[][10]{
  "LOST",
  "CAPTURED",
  "FOUND_LEFT",
  "FOUND_RIGHT",
};


typedef enum {
  NULL_VALUE,
  LEFT_SENSOR,
  CENTER_SENSOR,
  RIGHT_SENSOR
} lineSensor;

lineSensor lineLastDetected = NULL_VALUE;


// Global Variables

bool carParked = false;

unsigned long start_prev_time = 0;
boolean carInitialize_en = true;

uint8_t leftLine = 0;
uint8_t centerLine = 0;
uint8_t rightLine = 0;
uint8_t lineDirection = 0;

bool onLine = true;

bool leftSwitch;
bool rightSwitch;
bool button;

//char comma[3] = ", "

// Global Functions (Not the best place for them but gets them out the way)


void printStatus() {
#if PRINT_STATUS_UPDATE == true
  if (printDelay.millisDelay(PRINT_DELAY_mS)) {
    Serial.print(F("State: "));
    Serial.print(stateNames[smState]);
    Serial.print(F(", Move: "));
    Serial.print(motionModeName[motion_mode]);
    //    Serial.print(F(" Gyro: "));
    //    Serial.print(kalmanfilter.angle);
    //   Serial.print(F(", Ping: "));
    //   Serial.print(distance_value);
    //Serial.print(F(" cm"));
     Serial.print(F(", Batt: "));
     Serial.print(voltage);
    Serial.print(F(", "));
    Serial.print(F(" Line: "));
    Serial.print(leftLine);
    Serial.print(F(", "));
    Serial.print(centerLine);
    Serial.print(F(", "));
    Serial.print(rightLine);
    Serial.print(F(", "));
    Serial.print(lineDirection);
    Serial.print(F(", "));
    Serial.print(F("lnState: "));
    Serial.print(lineStateName[lnState]);
    Serial.print(F(" Switch: "));
    Serial.print(leftSwitch);
    Serial.print(F(", "));
    Serial.print(rightSwitch);
    Serial.print(F(", "));
    //  Serial.print(button);
    //  Serial.print(F(", "));



    if (low_voltage_flag) {
      Serial.print(F(" Low Voltage Detected!"));
    }
    if (carParked) {
      Serial.print(F(" Car Parked"));
    }
    Serial.println("");
  }
#endif
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
  rgb.flashYellowColorLeft();
  motion_mode = TURNLEFT;
}


void carTurnRight() {
  rgb.flashYellowColorRight();
  motion_mode = TURNRIGHT;
}

void carNudgeLeft() {
  rgb.flashBlueColorLeft();
  motion_mode = NUDGELEFT;
}


void carNudgeRight() {
  rgb.flashBlueColorRight();
  motion_mode = NUDGERIGHT;
}

void carReverseLeft() {
  rgb.flashRedColor();
  motion_mode = REVERSE_LEFT;
}

void carReverseRight() {
  rgb.flashRedColor();
  motion_mode = REVERSE_RIGHT;
}

void carStopNow() {
  rgb.flashYellowColorFront();
  rgb.flashYellowColorback();
  motion_mode = STOP;
}

void freeDrive() {
  rgb.flashGreenColorFront();
  rgb.flashGreenColorback();
  motion_mode = FREEDRIVE;
}


void parkCar() {
  function_mode = IDLE;
  motion_mode = STOP;
  carForward(210);
  //  delay((kalmanfilter_angle - 30) * (kalmanfilter_angle - 30) / 8);
  carStop();  // This causes it to fall over frontways
  start_prev_time = millis();
  rgb.brightRedColor();
}



void unparkCar() {
  setting_car_speed = 20;  // bug fix to try and stop it toppeling over on start
  if (millis() - start_prev_time > 500 && kalmanfilter_angle >= balance_angle_min) {
    start_prev_time = millis();
    motion_mode = START;
  }
  motion_mode = START;
}


void setMotionState() {
  switch (motion_mode) {
    case FORWARD:
      setting_car_speed = 10;  // origionally 40 // working at 5
      setting_turn_speed = 0;
      break;
    case BACKWARD:
      setting_car_speed = -10;
      setting_turn_speed = 0;
      break;
    case TURNLEFT:
      setting_car_speed = 0;
      setting_turn_speed = 20;  // Origionally 50
      break;
    case TURNRIGHT:
      setting_car_speed = 0;
      setting_turn_speed = -20;
      break;
    case NUDGELEFT:  // Added new 24/05/2024
      setting_car_speed = 5;
      setting_turn_speed = 20;
      break;
    case NUDGERIGHT:  // Added new 24/05/2024
      setting_car_speed = 5;
      setting_turn_speed = -20;
      break;
    case REVERSE_LEFT:  // Added new 25/05/2024
      setting_car_speed = -5;
      setting_turn_speed = -20;
      break;
    case REVERSE_RIGHT:  // Added new 25/05/2024
      setting_car_speed = -5;
      setting_turn_speed = 20;
      break;


    case FREEDRIVE:  // Added new 25/05/2024
      //setting_car_speed = 5;    // car speed and turns should be set via algorythm
      // setting_turn_speed = -20;
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
      Serial.println(F("Error in Motion State Machine"));
      break;
  }
}



void checkParked() {
  if (kalmanfilter.angle >= 28 || kalmanfilter.angle <= -27) {
    carParked = true;
  } else {
    carParked = false;
  }
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
