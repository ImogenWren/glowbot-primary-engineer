

#pragma once



void lineFollowerSensorRead() {
  int leftSensorValue = analogRead(leftSensorPin);
  int centerSensorValue = analogRead(centerSensorPin);
  int rightSensorValue = analogRead(rightSensorPin);

  // Normalize sensor values to the range of 0 to 100
  leftLineSense = map(leftSensorValue, 0, 1023, 0, 100);
  centerLineSense = map(centerSensorValue, 0, 1023, 0, 100);
  rightLineSense = map(rightSensorValue, 0, 1023, 0, 100);

  // Compute a weighted difference; factor in sensor positions
  // Calculate the motor control using integer arithmetic to ensure all are integers
  // Normalize from scale [-200 to 200] (worst case) to [-127 to 127] directly
  int motorControl = (((-1) * leftLineSense) + (0 * centerLineSense) + (1 * rightLineSense)) / 3;

  // Scale and adjust the motor control to fit within -127 to 127
  // Ensure motorControl falls within the expected motor output range
  motorControl = map(motorControl, -100, 100, 0, 100);
  directionLineSense = constrain(motorControl, 0, 100);  // safety clamp

  // Output for monitoring
#if PRINT_LINE_SENSOR_DATA == true
  Serial.print("Left: ");
  Serial.print(leftLineSense);
  Serial.print(", Center: ");
  Serial.print(centerLineSense);
  Serial.print(", Right: ");
  Serial.print(rightLineSense);
  Serial.print(", Direction: ");
  Serial.println(directionLineSense);  // Print the final motor output
#endif
}