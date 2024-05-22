
#pragma once

// send data
void sendESPnowData() {
  if (stringComplete) {
    // Serial.println(inputString);
    // Package data as required
    // char dataBuffer[32];
    // sprintf(dataBuffer, "Data: %2i, %2i ", data, data + 1);
    strcpy(txData.msg, inputString);
    txData.num_0 = 1;
    txData.num_1 = 2;
    txData.num_2 = 3;
    txData.num_3 = 4;
    txData.flag = flag;
    // send data
    const uint8_t *peer_addr = moduleRx.peer_addr;
#if PRINT_ESPNOW_TX == true
    Serial.print("Sending: ");
    Serial.println(txData.msg);
#endif
    esp_err_t result = esp_now_send(peer_addr, (uint8_t *)&txData, sizeof(txData));
#if PRINT_TX_STATUS == true
    // Analise result
    Serial.print("Send Status: ");
    if (result == ESP_OK) {
      Serial.println("Success");
    } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
      // How did we get so far!!
      Serial.println("ESPNOW not Init.");
    } else if (result == ESP_ERR_ESPNOW_ARG) {
      Serial.println("Invalid Argument");
    } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
      Serial.println("Internal Error");
    } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
      Serial.println("ESP_ERR_ESPNOW_NO_MEM");
    } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
      Serial.println("Peer not found.");
    } else {
      Serial.println("Not sure what happened");
    }
#endif
    stringComplete = false;
  }
}



// High Level Functions
void handleESPnow() {
  // In the loop we scan for moduleRx
  ScanForRx();
  // If moduleRx is found, it would be populate in `moduleRx` variable
  // We will check if `moduleRx` is defined and then we proceed further
  if (moduleRx.channel == CHANNEL) {  // check if moduleRx channel is defined
    // `moduleRx` is defined
    // Add moduleRx as peer if it has not been added already
    bool isPaired = manageRx();
    if (isPaired) {
      // pair success or already paired
      // Send data to device
      sendESPnowData();  // NOTE this only sends data if flag has been set that a new UART message is complete
    } else {
      // moduleRx pair failed
      Serial.println("moduleRx pair failed!");
    }
  } else {
    // No moduleRx found to process
  }
}


// callback when data is sent from moduleTx to moduleRx
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
#if PRINT_TX_STATS == true
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to: ");
  Serial.println(macStr);
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
#endif
}
