

#pragma once


// This function should be run within the linefollow state and takes all control of setting motion mode to follow the line
// given the 4 input values
// Additionally obstacle avoidance will be run in this state as a seperate function
// direction of 50 = in the middle



autoDelay lineDelay;



int LINE_MODE_DELAY_mS = 200;
int lineFindMode = 0;
bool printOnce = false;

void findLine(uint8_t left, uint8_t center, uint8_t right, uint8_t direction) {
  if (left >= 100 || right >= 100) {
    onLine = true;
  }

  switch (lineFindMode) {
    case 0:
      LINE_MODE_DELAY_mS = 200;
      carTurnLeft();
      break;
    case 1:
      carForward();
      LINE_MODE_DELAY_mS = 500;
      break;
    case 2:
      LINE_MODE_DELAY_mS = 200;
      carTurnRight();
      break;
    case 3:
      LINE_MODE_DELAY_mS = 500;
      carForward();
      break;
    default:
      Serial.print("Exeption in lineFindMode");
      break;
  }

  if (lineDelay.millisDelay(LINE_MODE_DELAY_mS)) {
    lineFindMode++;
    if (lineFindMode > 3) {
      printOnce = false;
      lineFindMode = 0;
    }
  }
}

//#TODO create a bool

/**
 * Defines the valid states for the state machine
 * 
 */


#define INCLUDE_PING_SENSOR true
#define INCLUDE_BUMPER_SWITCH true


//leftLine, centerLine, rightLine, lineDirection  < global line sensor values to use directly
//setting_car_speed = 5;
//     setting_turn_speed
//    global values for bumper switched


// We need to track the last "high" reading from a specific sensor so if the line is lost we can define the direction to turn
void lastHighSensor() {
  if (lnState != LINE_LOST) {                                 //If the state is not line lost, then compare all the sensors to see which one is highest
    if (centerLine >= leftLine && centerLine >= rightLine) {  // First check center as if this is higher so this will have all the =< statements
      lineLastDetected = CENTER_SENSOR;
    } else if (leftLine > centerLine && leftLine > rightLine) {
      lineLastDetected = LEFT_SENSOR;
    } else if (rightLine > centerLine && rightLine > leftLine) {
      lineLastDetected = RIGHT_SENSOR;
    }
  }
}


void lineLostFunction() {
  if (lineLastDetected == LEFT_SENSOR) {
    carNudgeRight();
  } else if (lineLastDetected == RIGHT_SENSOR) {
    carNudgeLeft();
  }
}

void lineCapturedFunction(uint8_t direction) {
  if ((direction == 50)&& lnState == LINE_CAPTURED) {
    carForward();
    //  Serial.println("On Line");
  } else if (direction <= 48) {
    carTurnRight();
    // Serial.println("TURN RIGHT");
  } else if (direction >= 52) {
    carTurnLeft();
    // Serial.println("TURN LEFT");
  } else if (direction <= 50) {
    carNudgeRight();
    // Serial.println("TURN RIGHT");
  } else if (direction >= 50) {
    carNudgeLeft();
    // Serial.println("TURN LEFT");
  }
}



bool lineFollow(uint8_t left, uint8_t center, uint8_t right, uint8_t direction) {



  if (distance_value <= OBSTACLE_LIMIT_CM) {  // If obstacle is encountered
    carBackward();
  }

  if (!leftSwitch && !rightSwitch) {  // problem with these calls is no timing to back up a certain amount before switching back to forwards
    carBackward();
  } else if (!leftSwitch) {
    carReverseLeft();
  } else if (!rightSwitch) {
    carReverseRight();
  }

  if (left < 9 && center < 9 && right < 9) {
    lnState = LINE_LOST;
  }

  if (center > 10 || left > 10 || right > 10) {
    lnState = LINE_CAPTURED;
  }

  lastHighSensor();



  switch (lnState) {
    case LINE_LOST:
      lineLostFunction();
      return;
    case LINE_CAPTURED:
      lineCapturedFunction(direction);
  }




  return onLine;
}
