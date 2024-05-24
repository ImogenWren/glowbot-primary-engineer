

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

bool lineFollow(uint8_t left, uint8_t center, uint8_t right, uint8_t direction) {

  if (left < 10 && center < 10 && right < 10) {
   // onLine = false;
  } else if (direction == 50 && center > 30 && left < 10 && right < 10) {
    onLine = true;
  }
//Serial.println(direction);
 // if (onLine) {
   // if ((direction >= 49 && direction <= 51) && onLine == true) {
    if ((direction == 50) && onLine == true) {
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


 // } else {
 //   findLine(left, center, right, direction);
 //   if (!printOnce) {
 //     Serial.println("Finding Line");
 //     printOnce = true;
 //   }
//  }
  return onLine;
}
