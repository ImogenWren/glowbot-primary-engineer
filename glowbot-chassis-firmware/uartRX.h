#pragma once



struct RxStruct {
  char msg[16];      // 16 bytes
  uint8_t val_0;     //  1
  uint8_t val_1;     //  1
  uint8_t val_2;     //  1
  uint8_t val_3;     //  1
  bool leftSwitch;   //  1
  bool rightSwitch;  //  1
  bool button;       //  1
  byte padding[9];   //  9
                     //------
                     // 32
};



RxStruct rxData;

bool newData = false;

const byte startMarker = 255;
const byte rxDataLen = sizeof(rxData);




void uartRxStructure() {
  static byte recvData[rxDataLen];
  byte rb;
  byte *structStart;
  structStart = reinterpret_cast<byte *>(&rxData);
  if (Serial.available() >= rxDataLen + 1 and newData == false) {
    rb = Serial.read();

    if (rb == startMarker) {
      // copy the bytes to the struct
      for (byte n = 0; n < rxDataLen; n++) {
        *(structStart + n) = Serial.read();
      }
      // make sure there is no garbage left in the buffer
      while (Serial.available() > 0) {
        byte dumpTheData = Serial.read();
      }
      newData = true;
    }
  }
}

void parseUARTdata() {
  if (newData) {
    leftLine = rxData.val_0;
    centerLine = rxData.val_1;
    rightLine = rxData.val_2;
    lineDirection = rxData.val_3;
    leftSwitch = rxData.leftSwitch;
    rightSwitch = rxData.rightSwitch;
    button = rxData.button;

#if PRINT_UART_RX == true
    Serial.print(F("UART-Rx: "));
    Serial.print(rxData.msg);
    Serial.print(' ');
    Serial.print(rxData.val_0);
    Serial.print(' ');
    Serial.print(rxData.val_1);
    Serial.print(' ');
    Serial.print(rxData.val_2);
    Serial.print(' ');
    Serial.print(rxData.val_3);
    Serial.print(' ');
    Serial.print(rxData.leftSwitch);
    Serial.print(' ');
    Serial.print(rxData.rightSwitch);
    Serial.print(' ');
    Serial.print(rxData.button);

    Serial.println("");


#endif
    newData = false;
  }
}
