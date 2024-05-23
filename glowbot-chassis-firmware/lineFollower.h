

#pragma once


// This function should be run within the linefollow state and takes all control of setting motion mode to follow the line
// given the 4 input values
// Additionally obstacle avoidance will be run in this state as a seperate function
// direction of 50 = in the middle



autoDelay lineDelay;



#define LINE_MODE_DELAY_mS 200
int lineFindMode = 0;

void findLine(uint8_t left, uint8_t center, uint8_t right, uint8_t direction) {
  if (left >= 100 || right >= 100) {
    onLine = true;
  }

  switch (lineFindMode) {
    case 0:
      carTurnLeft();
      break;
    case 1:
      carForward();
      break;
    case 2:
      carTurnRight();
      break;
    case 3:
      carForward();
      break;
    default:
      Serial.print("Exeption in lineFindMode");
      break;
  }

  if (lineDelay.millisDelay(LINE_MODE_DELAY_mS)) {
    lineFindMode++;
    if (lineFindMode > 3) {
      lineFindMode = 0;
    }
  }
}



bool lineFollow(uint8_t left, uint8_t center, uint8_t right, uint8_t direction) {
 // if (onLine) {
    if (left < 10 && center < 10 && right < 10) {
      onLine = false;
    } 
    if (direction >= 48 || direction < 52) {
      onLine = true;
      carForward();
    } else if (direction < 48) {
      carTurnRight();
    } else if (direction >= 52) {      
      carTurnLeft();
    }
 // } else {
 //   findLine(left, center, right, direction);
 // }
  return onLine;
}
