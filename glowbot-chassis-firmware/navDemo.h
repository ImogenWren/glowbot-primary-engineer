


#pragma once






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

