




enum FUNCTION_MODE
{
  IDLE,
  IRREMOTE,
  OBSTACLE,
  FOLLOW,
  BLUETOOTH,
  FOLLOW2,
} function_mode = IDLE;

enum MOTION_MODE
{
  STANDBY,
  FORWARD,
  BACKWARD,
  TURNLEFT,
  TURNRIGHT,
  NUDGELEFT,
  NUDGERIGHT,
  STOP,
  START,
} motion_mode = START;

char motionModeName[][10]{
  "STANDBY",
  "FORWARD",
  "BACKWARD",
  "TURNLEFT",
  "TURNRIGHT",
  "NUDGELEFT",
  "NUDGERIGHT",
  "STOP",
  "START",
};


void modes() {  // this is not a working function, just a way to group things found in the origional code
 // obstacleAvoidanceMode();
  //followMode();
 // followMode2();
}

