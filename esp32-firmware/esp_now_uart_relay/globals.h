


#pragma once

bool flag = false;



// Line Sensing
const int rightSensorPin = 34;   //Yellow
const int centerSensorPin = 35;   // Green
const int leftSensorPin = 32;     // Pink

uint8_t leftLineSense;
uint8_t centerLineSense;
uint8_t rightLineSense;
uint8_t directionLineSense;


void trimCharArray(char *inArray) {
  char nullChar[2] = "\0";             // This must be 2 char long
  int index = strcspn(inArray, "\n");  // Find the location of the newline char
  inArray[index] = *nullChar;
}