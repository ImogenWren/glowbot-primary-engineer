/** 

# ESPnow - UART Sender

   Date: 17/05/2024
   Author: Imogen Wren <https://github.com/ImogenWren>
   Purpose: UART Transmission link using ESPnow
   Description: This sketch consists of the code for the Sender Module
   Resources: (A bit outdated)
   a. https://espressif.com/sites/default/files/documentation/esp-now_user_guide_en.pdf
   b. http://www.esploradores.com/practica-6-conexion-esp-now/
   c. https://randomnerdtutorials.com/esp-now-esp32-arduino-ide/


   << This Device Sender >>
moduleTx
   Flow: UART Sender (moduleTx)
   Step 1 : ESPNow Init on Sender and set it in STA mode
   Step 2 : Start scanning for Receiver ESP32 (we have added a prefix of `moduleRx` to the SSID of moduleRx for an easy setup)
   Step 3 : Once found, add moduleRx as peer
   Step 4 : Register for send callback
   Step 5 : Start Transmitting data from Sender to moduleRx

   Flow: Receiver (moduleRx)
   Step 1 : ESPNow Init on moduleRx
   Step 2 : Update the SSID of moduleRx with a prefix of `moduleRx`
   Step 3 : Set moduleRx in AP mode
   Step 4 : Register for receive callback and wait for data
   Step 5 : Once data arrives, print it in the serial monitor

   Note: Sender and Receiver have been defined to easily understand the setup.
         Based on the ESPNOW API, there is no concept of Sender and Receiver.
         Any devices can act as moduleRx or moduleTx.
*/

/*
As always with ESP32 - Follow RandomNerdTutorials if you want something to work!

ESP-NOW supports the following features:

    - Encrypted and unencrypted unicast communication;
    - Mixed encrypted and unencrypted peer devices;
    - Up to 250-byte payload can be carried;
    - Sending callback function that can be set to inform the application layer of transmission success or failure.

ESP-NOW technology also has the following limitations:

    - Limited encrypted peers.
    - 10 encrypted peers at the most are supported in Station mode;
    - 6 at the most in SoftAP or SoftAP + Station mode;
    - Multiple unencrypted peers are supported, however, their total number should be less than 20, including encrypted peers;
    - Payload is limited to 250 bytes. 

*/

/* ESP32 UART

- https://www.luisllamas.es/en/esp32-uart/
- https://github.com/PanGalacticTech/boxthatscreams - Previous project that incorperated a serial message repeater





*/


//RTC_DATA_ATTR int bootCount = 0;  a way to store data through shutdowns? RESEARCH THIS!

#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>  // only for esp_wifi_set_channel()
#include <autoDelay.h>


// Global copy of peer data for Receiver
esp_now_peer_info_t moduleRx;


// Debugging Printout
#define PRINT_SCAN_SUMMARY false
#define PRINTSCANRESULTS false
#define PRINT_MODULEFOUND false
#define WIFI_DEBUG false
#define PRINT_TX_STATS false
#define PRINT_TX_STATUS false
#define PRINT_REMOTE_STATUS false
#define PRINT_UART_RX true
#define PRINT_ESPNOW_TX false
#define PRINT_UART_DATA_SENT false
#define PRINT_LINE_SENSOR_DATA false

// ESPnow options
#define CHANNEL 1
#define DELETEBEFOREPAIR 0

#include <HardwareSerial.h>

//HardwareSerial mySerial(1);  // define a Serial for UART1
const int MySerialRX = 16;
const int MySerialTX = 17;

#define STRUCT_MSG_SIZE 150
#define UART_MSG_SIZE 150




// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  char msg[STRUCT_MSG_SIZE];
  int num_0;
  int num_1;
  int num_2;
  int num_3;
  bool flag;
} struct_message;

// Create a struct_message called txData
struct_message txData;


// data to be sent -> Limited to 32 bytes!
struct uartStruct {
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

uartStruct uartData = { "xxx", 0, 0, 0, 0, 0, 0, 0, 0 };  // dummy load of data

const byte startMarker = 255;  // used to note start of packet for UART struct parsing
const byte uartDataLen = sizeof(uartData);

char inputString[STRUCT_MSG_SIZE];  // specify max length of 32 chars? bytes?
char overflowBuffer[STRUCT_MSG_SIZE];
bool stringComplete;


bool bufferOverflow = false;
bool uartTXdata_available = false;  // flags if new sensor data is available to send over UART connection

/*
https://forum.arduino.cc/t/simple-code-to-send-a-struct-between-arduinos-using-serial/672196
*/


#include "globals.h"
#include "uartFunctions_Rx.h"
#include "uartFunctions_Tx.h"
#include "esp-wireless.h"
#include "ESPnowFunctions.h"
#include "lineFollower.h"

#define RIGHT_SWITCH_PIN 26
#define LEFT_SWITCH_PIN 27

bool leftSwitchState;
bool rightSwitchState;


autoDelay sampleDelay;
#define SENSOR_SAMPLERATE_mS 200
// Gather and sort all local sensors into data structure to send via UART to local periferal device
void gatherSensors() {
  if (sampleDelay.millisDelay(SENSOR_SAMPLERATE_mS)) {
    uartData.val_0 = leftLineSense;
    uartData.val_1 = centerLineSense;
    uartData.val_2 = rightLineSense;
    uartData.val_3 = directionLineSense;
    uartData.leftSwitch = leftSwitchState;
    uartData.rightSwitch = rightSwitchState;
    uartTXdata_available = true;
  }
}

void sampleLimitSwitches() {
  leftSwitchState = digitalRead(LEFT_SWITCH_PIN);
  rightSwitchState = digitalRead(RIGHT_SWITCH_PIN);
}




void setup() {
  Serial.begin(115200);
  Serial.print("ESPnow - UART sender Starting");
  //Set device in STA mode to begin with
  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE);
  Serial.println("Glowbot: ESP32: UART Bridge & Sensor Expansion Package");
  // This is the mac address of the moduleTx in Station Mode
  Serial.print("STA MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.print("STA CHANNEL ");
  Serial.println(WiFi.channel());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  pinMode(LEFT_SWITCH_PIN, INPUT_PULLUP);
  pinMode(RIGHT_SWITCH_PIN, INPUT_PULLUP);
  // Setup serial port to listen for incoming data
  Serial2.begin(115200, SERIAL_8N1, MySerialRX, MySerialTX);
}



void loop() {
  lineFollowerSensorRead();  // samples line sensors
  sampleLimitSwitches();
  gatherSensors();  // gather data from local sensors
  sendUARTdata();   // send sensor data over UART if new data is available
  serialEvent();
  handleESPnow();  // handles all ESPnow data transmission, if serial data is available to send
}
