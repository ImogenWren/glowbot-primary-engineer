

#pragma once




void sendUARTdata() {
  if (uartTXdata_available) {
    mySerial.write(startMarker);
    mySerial.write((byte*)&uartData, uartDataLen);
#if PRINT_UART_DATA_SENT == true
    Serial.print("Sent: ");
    Serial.print(uartData.msg);
    Serial.print(' ');
    Serial.print(uartData.val_0);
    Serial.print(' ');
    Serial.print(uartData.val_1);
    Serial.print(' ');
    Serial.print(uartData.val_2);
    Serial.print(' ');
    Serial.print(uartData.val_3);
    Serial.println("");
#endif
    uartTXdata_available = false;
  }
}
