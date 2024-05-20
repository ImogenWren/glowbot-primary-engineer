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
  STOP,
  START,
} motion_mode = START;

char motionModeName[][10]{
  "STANDBY",
  "FORWARD",
  "BACKWARD",
  "TURNLEFT",
  "TURNRIGHT",
  "STOP",
  "START",
};
