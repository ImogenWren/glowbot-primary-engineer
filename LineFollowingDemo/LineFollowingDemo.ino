// Define the pins connected to the analog sensors
const int leftSensorPin = A13;
const int centerSensorPin = A14;
const int rightSensorPin = A15;

// Define constants for the control system
const int MAX_TURN = 127;  // Maximum turn value

void setup() {
  Serial.begin(9600);
}

void loop() {
    int leftSensorValue = analogRead(leftSensorPin);
    int centerSensorValue = analogRead(centerSensorPin);
    int rightSensorValue = analogRead(rightSensorPin);

    // Normalize sensor values to the range of 0 to 100
     leftSensorValue = map(leftSensorValue, 0, 1023, 0, 100);
     centerSensorValue = map(centerSensorValue, 0, 1023, 0, 100);
     rightSensorValue = map(rightSensorValue, 0, 1023, 0, 100);

    // Compute a weighted difference; factor in sensor positions
    // Calculate the motor control using integer arithmetic to ensure all are integers
    // Normalize from scale [-200 to 200] (worst case) to [-127 to 127] directly
    int motorControl = (((-1) * normalizedLeft) + 
                        (0 * normalizedCenter) + 
                        (1 * normalizedRight)) / 3;

    // Scale and adjust the motor control to fit within -127 to 127
    // Ensure motorControl falls within the expected motor output range
    motorControl = map(motorControl, 0, 100, -MAX_TURN, MAX_TURN);
    motorControl = constrain(motorControl, -MAX_TURN, MAX_TURN);  // safety clamp

    // Output for monitoring
    Serial.print("Left: ");
    Serial.print(normalizedLeft);
    Serial.print(", Center: ");
    Serial.print(normalizedCenter);
    Serial.print(", Right: ");
    Serial.print(normalizedRight);
    Serial.print(", Motor Output: ");
    Serial.println(motorControl);  // Print the final motor output

    delay(100);
}
