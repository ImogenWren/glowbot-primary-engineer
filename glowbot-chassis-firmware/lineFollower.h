

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


void moveBasedOnLastSensor() {
  if (lineLastDetected == LEFT_SENSOR) {
    carNudgeRight();
  } else if (lineLastDetected == RIGHT_SENSOR) {
    carNudgeLeft();
  }
}

autoDelay lostDelay;

void lineLostFunction() {
  if (lostDelay.secondsDelay(1)) {
    lineLostCount++;
  }
  if (lineLostCount <= 10) {
    moveBasedOnLastSensor();
  } else if (lineLostCount > 10) {  // this stops it getting stuck in a loop going around in circles
    carForward();
  } else if (lineLostCount > 20) {
    lineLostCount = 0;
  }
}

void lineCapturedFunction(uint8_t direction) {
  // ~TODO try this if without the lnState
  if ((direction == 50) && lnState == LINE_CAPTURED) {
    carForward();
    //  Serial.println("On Line");
  } else if (direction <= 48) {  /// was working at 48
    carTurnRight();
    // carSpeedRight();
    // Serial.println("TURN RIGHT");
  } else if (direction >= 52) {  // was working at 52
    carTurnLeft();
    // carSpeedLeft();
    // Serial.println("TURN LEFT");
  } else if (direction <= 49) {
    //carTurnRight();
    carNudgeRight();
    // carSpeedRight();
    // Serial.println("TURN RIGHT")
  } else if (direction >= 51) {
    //  carTurnLeft();
    carNudgeLeft();
    //carSpeedLeft();
    // Serial.println("TURN LEFT");
  } else if (direction <= 50) {
    carNudgeRight();
    // Serial.println("TURN RIGHT");
  } else if (direction >= 50) {
    carNudgeLeft();
    // Serial.println("TURN LEFT");
  }
}


void dodgeObstical(){
if (lostDelay.secondsDelay(1)) {
      obsticalDetectedCount++;
    }
    if (obsticalDetectedCount < 10) {
      carBackward();
    } else if (obsticalDetectedCount < 20) {
      carReverseRight();
    } else if (obsticalDetectedCount < 30) {
      carReverseLeft();
    } else if (obsticalDetectedCount >= 30) {
      obsticalDetectedCount = 0;
    }
}


bool lineFollow(uint8_t left, uint8_t center, uint8_t right, uint8_t direction) {

  lastHighSensor();  // Track which sensor detected the line last

  if (left < 9 && center < 9 && right < 9) {  // work out if the sensors do not see any line at all
    lnState = LINE_LOST;
  }
  if (left > 140 && center > 140 && right > 140) {  // work out if the sensors do not see any line at all
    lnState = LINE_LOST;
  }

  if (center > 10 || left > 10 || right > 10) {  // if any one sensor has detected the line
    lineLostCount = 0;
    lnState = LINE_CAPTURED;
  }




  switch (lnState) {
    case LINE_LOST:
      lineLostFunction();
      break;
    case LINE_CAPTURED:
      lineCapturedFunction(direction);
      break;
    default:
      Serial.println(F("Err in lnState"));
      break;
  }



  // Put this last so it overwrites the state set by other functions
  if (distance_value <= OBSTACLE_LIMIT_CM) {  // If obstacle is encountered
    obsticalDetected = true;
  }

  if (distance_value > OBSTACLE_LIMIT_CM + 30) {
    obsticalDetectedCount = 0;
    obsticalDetected = false;
  }

  if (obsticalDetected) {

    dodgeObstical();
    
  }






  // This bit isnt working very well at the moment
  if (!leftSwitch && !rightSwitch) {  // nothing to keep this in these states
    carBackward();
  } else if (!leftSwitch) {
    carReverseLeft();
  } else if (!rightSwitch) {
    carReverseRight();
  }
  return onLine;
}
