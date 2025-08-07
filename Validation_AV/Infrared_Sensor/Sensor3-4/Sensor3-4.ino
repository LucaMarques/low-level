/***************
*   Sensor 3-4   *
***************/

#include <esp_now.h>
#include <WiFi.h>

#define DEBUG       (true)
#define BOARD_ID       3
#define PIN_SENSOR1  22
#define PIN_SENSOR2  23
#define DISTANCE     0.5 
#define CALIB_DEFAULT 40

void IRAM_ATTR sendInitial();
void IRAM_ATTR sendFinal();

uint8_t receiverAddress[] = {0x5C, 0xCF, 0x7F, 0xB1, 0x2A, 0xE6};

typedef struct set_up {
  int calibration = CALIB_DEFAULT;
  bool active = false;
} set_up;

typedef struct data {
  float velo;
} data;

set_up set_board;

volatile uint32_t initialTime = false;
volatile uint32_t finalTime = false;

SemaphoreHandle_t mutex;

bool veloDone;
data data_board;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&set_board, incomingData, sizeof(set_up));
  
  #if DEBUG
      Serial.println("Set up received");
  #endif
  
   if (set_board.active) { 
    attachInterrupt(PIN_SENSOR1, sendInitial, FALLING);
  }
  else {
    detachInterrupt(PIN_SENSOR1);
    detachInterrupt(PIN_SENSOR2);
  } 
}
 
void setup() {
  
  pinMode(PIN_SENSOR1, INPUT_PULLUP);
  pinMode(PIN_SENSOR2, INPUT_PULLUP);

  attachInterrupt(PIN_SENSOR1, sendInitial, FALLING);
  
  Serial.begin(115200);
 
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();  
 
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    ESP.restart();
    delay(1);
  }
  
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
  
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false; 
 
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    ESP.restart();
    delay(1);
  } 
}
 
void loop() {
  
}

void sendInitial(){
  initialTime = millis();
  detachInterrupt(PIN_SENSOR1);
  attachInterrupt(PIN_SENSOR2, sendFinal, FALLING);
}

void sendFinal(){
  finalTime = millis();
  
  data_board.velo = DISTANCE / ((finalTime - initialTime) / 1000.0);

  esp_err_t result = esp_now_send(receiverAddress, (uint8_t *) &data_board, sizeof(data));

  initialTime = false;
  finalTime = false;

  attachInterrupt(PIN_SENSOR1, sendInitial, FALLING);
  detachInterrupt(PIN_SENSOR2);
}
