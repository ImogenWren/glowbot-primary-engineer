
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
  STATE_BACKWARD,
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
  "STATE_BACKWARD",
  "STATE_TURNLEFT",
  "STATE_TURNRIGHT",
  "STATE_FOLLOWLINE",
  "STATE_PATHBLOCKED",
  "STATE_ESTOP"
};


// Global Variables

bool carParked = false;

unsigned long start_prev_time = 0;
boolean carInitialize_en = true;


// Global Functions (Not the best place for them but gets them out the way)


void printStatus() {
  if (printDelay.millisDelay(PRINT_DELAY_mS)) {
    Serial.print(F("Operation State: "));
    Serial.print(stateNames[smState]);
    Serial.print(F(", Motion Mode: "));
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
    if (carParked) {
      Serial.print("Car Parked");
    }
    Serial.println("");
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
