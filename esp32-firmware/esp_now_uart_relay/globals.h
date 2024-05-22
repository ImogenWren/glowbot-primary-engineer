


#pragma once

bool flag = false;


void trimCharArray(char *inArray) {
  char nullChar[2] = "\0";             // This must be 2 char long
  int index = strcspn(inArray, "\n");  // Find the location of the newline char
  inArray[index] = *nullChar;
}