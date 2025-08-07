/***************
*   Sensor 1   *
***************/

#include <ESP8266WiFi.h>
#include <espnow.h>

#define DEBUG       true
#define BOARD_ID       1
#define PIN_SENSOR    0
#define CALIB_DEFAULT 40

typedef struct ID {
  short id = BOARD_ID;
} ID;

typedef struct set_up {
  int calibration = CALIB_DEFAULT;
  bool active = false;
} set_up;

ID id_board;
set_up set_board;

void ICACHE_RAM_ATTR sendSinal();

uint8_t receiverAddress[] = {0x5C, 0xCF, 0x7F, 0xB1, 0x2A, 0xE6};

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  #if DEBUG
    Serial.print("\r\nLast Packet Send Status: ");
    if (sendStatus == 0){
      Serial.println("Delivery success");
    }
    else{
      Serial.println("Delivery fail");
    }
  #endif
  
}

void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {

  if (len == sizeof(set_up)) {
    memcpy(&set_board, incomingData, sizeof(set_up));
    #if DEBUG
      Serial.println("Set up received");
    #endif
  }
  else 
    Serial.println("Incoming data does not match");

  if (set_board.active){
    attachInterrupt(digitalPinToInterrupt(PIN_SENSOR), sendSinal, FALLING);
  }
  else {
    detachInterrupt(digitalPinToInterrupt(PIN_SENSOR));
  }
  
}

void setup() {

  pinMode(PIN_SENSOR, INPUT_PULLUP);
  
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    ESP.restart();
    delay(1);
  } 

  #if DEBUG
    Serial.print("\nBoard id: ");
    Serial.println(id_board.id);
    Serial.print("Set up calibration: ");
    Serial.println(set_board.calibration);
    Serial.print("Set up activation: ");
    Serial.println(set_board.active ? "true" : "false");
  #endif

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  esp_now_register_send_cb(OnDataSent);
  
  esp_now_add_peer(receiverAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  
  esp_now_register_recv_cb(OnDataRecv); 
}
 
void loop(){
  
}

void sendSinal()
{
  esp_now_send(receiverAddress, (uint8_t *) &id_board, sizeof(id_board));
}
