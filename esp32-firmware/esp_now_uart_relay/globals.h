


#pragma once

bool flag = false;



// Line Sensing
const int leftSensorPin = A13;
const int centerSensorPin = A14;
const int rightSensorPin = A15;

uint8_t leftLineSense;
uint8_t centerLineSense;
uint8_t rightLineSense;
uint8_t directionLineSense;


void trimCharArray(char *inArray) {
  char nullChar[2] = "\0";             // This must be 2 char long
  int index = strcspn(inArray, "\n");  // Find the location of the newline char
  inArray[index] = *nullChar;
}