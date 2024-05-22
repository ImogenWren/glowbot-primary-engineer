

#pragma once




byte byteCount = 0;
byte strLength;

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/

void serialEvent() {
  // here we could use our MySerial normally
  while (mySerial.available() > 0) {
    // uint8_t byteFromSerial = MySerial.read();
    char inChar = (char)Serial.read();
    //  Serial.print(inChar);
    //  Serial.print("  byteCount: ");
    //  Serial.println(byteCount);
    // add it to the inputString:
    if (byteCount <= STRUCT_MSG_SIZE) {
      inputString[byteCount] = inChar;  // Compiler has issue with this line
      byteCount++;                      // if the incoming character is a newline, set a flag so the main loop can
      strLength = byteCount;
    } else {
      Serial.print("ESP32: UART Message Exceeds Buffer Size, filling overflow ");
      overflowBuffer[byteCount = STRUCT_MSG_SIZE];
    }

    // do something about it:
    if (inChar == '\n' or byteCount >= UART_MSG_SIZE) {  // if null character reached or buffer is filled, then string is completed
      stringComplete = true;
      byteCount = 0;
      //  Serial.println("\n\nUART Data Received: ");
      trimCharArray(inputString);  // added 22/05/2024
#if PRINT_UART_RX == true
      Serial.println(inputString);
#endif
    }
  }
}
