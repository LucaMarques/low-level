/***************
*   Sender 3   *
***************/

#include <esp_now.h>
#include <WiFi.h>
#include <Ultrasonic.h>

#define DEBUG       true
#define BOARD_ID       3
#define TRIGGER_PIN1  26
#define ECHO_PIN1     27
#define TRIGGER_PIN2  19
#define ECHO_PIN2     18
#define CALIB_DEFAULT 40
#define DISTANCE     1.0

Ultrasonic ultrasonic1(TRIGGER_PIN1, ECHO_PIN1);
Ultrasonic ultrasonic2(TRIGGER_PIN2, ECHO_PIN2);

uint8_t receiverAddress[] = {0x5C, 0xCF, 0x7F, 0xB1, 0x2A, 0xE6};

typedef struct set_up {
  int calibration = CALIB_DEFAULT;
  bool active = false;
} set_up;

typedef struct data {
  float velo;
} data;

set_up set_board;
data data_board;

float cmMsec1 = false;
long microsec1 = false;
float cmMsec2 = false;
long microsec2 = false;

unsigned long initialTime = false;
unsigned long finalTime = false;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  #if DEBUG
    char macStr[18];
    Serial.print("Packet to: ");
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.print(macStr);
    Serial.print(" send status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  #endif
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&set_board, incomingData, sizeof(set_up));
}
 
void setup() {
  Serial.begin(115200);
 
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
 
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    ESP.restart();
    delay(1);
  }
  
  esp_now_register_send_cb(OnDataSent);
   
  // register peer
  esp_now_peer_info_t peerInfo;
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  // register first peer  
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    ESP.restart();
    delay(1);
  }

  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
  if (set_board.active) {
    microsec1 = ultrasonic1.timing();
    cmMsec1 = ultrasonic1.convert(microsec1, Ultrasonic::CM);

    microsec2 = ultrasonic2.timing();
    cmMsec2 = ultrasonic2.convert(microsec2, Ultrasonic::CM);

    Serial.print(cmMsec1);
    Serial.print("\t");
    Serial.println(cmMsec2);

    if (!initialTime && cmMsec1 < set_board.calibration)
      initialTime = millis();

    if (!finalTime && cmMsec2 < set_board.calibration) {
      finalTime = millis();

      data_board.velo = DISTANCE / ((finalTime - initialTime) / 1000.0);

      esp_err_t result = esp_now_send(receiverAddress, (uint8_t *) &data_board, sizeof(data));

      if (result == ESP_OK)
        Serial.println("Sent with success");
      else 
        Serial.println("Error sending the data");

      initialTime = false;
      finalTime = false;

      delay(1000);
    }
  }
}
