




enum FUNCTION_MODE {
  IDLE,
  IRREMOTE,
  OBSTACLE,
  FOLLOW,
  BLUETOOTH,
  FOLLOW2,
} function_mode = IDLE;

enum MOTION_MODE {
  STANDBY,
  FORWARD,
  BACKWARD,
  TURNLEFT,
  TURNRIGHT,
  SPEEDLEFT,
  SPEEDRIGHT,
  NUDGELEFT,
  NUDGERIGHT,
  REVERSE_LEFT,
  REVERSE_RIGHT,
  FREEDRIVE,
  STOP,
  START,
} motion_mode = START;

char motionModeName[][14]{
  "STANDBY",
  "FORWARD",
  "BACKWARD",
  "TURNLEFT",
  "TURNRIGHT",
  "SPEEDLEFT",
  "SPEEDRIGHT",
  "NUDGELEFT",
  "NUDGERIGHT",
  "REVERSE_LEFT",
  "REVERSE_RIGHT",
  "FREEDRIVE",
  "STOP",
  "START"
};


void modes() {  // this is not a working function, just a way to group things found in the origional code
                // obstacleAvoidanceMode();
  //followMode();
  // followMode2();
}
