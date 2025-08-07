/*****************
*    Receiver    *
*****************/

#include <ESP8266WiFi.h>
#include <espnow.h>

#define DEBUG true
#define BOARD_01 1
#define BOARD_02 2
#define BOARD_03 3
#define PASSAGES 999
#define CALIB   40

uint8_t sender_01Address[] = {0x48, 0x3F, 0xDA, 0x92, 0x03, 0x43};
uint8_t sender_02Address[] = {0x5C, 0xCF, 0x7F, 0xEE, 0x58, 0x65};
uint8_t sender_03Address[] = {0x7C, 0x9E, 0xBD, 0xED, 0x52, 0x70};

typedef struct ID {
  short id;
} ID;

typedef struct set_up {
  int calibration;
  bool activated;
} set_up;

typedef struct data {
  float velo;
} data;

String msg;
bool newPass = false;
int passageCounter = 0;
float passages[PASSAGES] = {};
unsigned long initialTime = false;
unsigned long finalTime = false;
float velo = false;
short calibration = CALIB;
bool activ = false;

void OnDataSent(uint8_t *, uint8_t);
void OnDataRecv(uint8_t *, uint8_t *, uint8_t);
void activation(bool);
void setCalib(short);
 
void setup() {
  Serial.begin(115200);

  #if DEBUG
    Serial.print("\nID size: ");
    Serial.println(sizeof(ID));
    Serial.print("set_up size: ");
    Serial.println(sizeof(set_up));
    Serial.print("data size: ");
    Serial.println(sizeof(data));
  #endif
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    ESP.restart();
    delay(1);
  }

  Serial.println("\n\nCommands:");
  Serial.println("new - new passage");
  Serial.println("dtl - delete last one");
  Serial.println("on  - turn on the boards");
  Serial.println("off - turn off the boards");
  Serial.println("set xx - set calibration to xx");
  Serial.println();
  
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  esp_now_register_send_cb(OnDataSent);
  
  esp_now_add_peer(sender_01Address, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_add_peer(sender_02Address, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_add_peer(sender_03Address, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  
  esp_now_register_recv_cb(OnDataRecv); 

  activation(activ);
}

void loop(){

  if (newPass){
    passages[passageCounter] = (finalTime - initialTime)/1000.0;
    Serial.print("Passagem N");
    Serial.print(passageCounter + 1);
    Serial.print(": ");
    Serial.print(passages[passageCounter]);
    Serial.print("s\n");
    initialTime = false;
    finalTime = false;
    newPass = false;
    passageCounter++;
  }

  while(Serial.available()) msg += Serial.readString();
  
  if (msg == "new\n"){
    //clean all variables
    msg = "";
    passageCounter = false;
    for (int i = false; i < PASSAGES; i++)
      passages[i] = false;
    initialTime = false;
    finalTime = false;
    newPass = false;
    Serial.println("\n\n----- New passage -----");
  }
  else if (msg == "dtl\n"){
    //clean all variables
    msg = "";
    passages[passageCounter] = false;
    passageCounter--;
    Serial.println("deleted last one");
    initialTime = false;
    finalTime = false;
    newPass = false;
  }
  else if (msg == "on\n"){
    msg = "";
    activation(true);
    Serial.println("Turned on");
  }
  else if (msg == "off\n"){
    msg = "";
    activation(false);
    Serial.println("Turned off");
  }
  else if (msg.indexOf("set ") != -1) {
    String str = "";
    
    str.concat(msg[4]);
    str.concat(msg[5]);
    
    long number = str.toInt();

    #if DEBUG
      Serial.println("Message received: ");
      Serial.print(msg);
      Serial.print("Message[4]: ");
      Serial.println(msg[4]);
      Serial.print("Message[5]: ");
      Serial.println(msg[5]);
      Serial.print("String number: ");
      Serial.println(str);
      Serial.print("Int number: ");
      Serial.println(number);
    #endif

    Serial.print("Set to: ");
    Serial.println(number);

    if (number)
      setCalib(number);
    else
      Serial.println("invalid input command");

    msg = "";
  }
  else if (msg != "") {
    msg = "";
    Serial.println("command does not exist");
  }
}

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

void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) {

  #if DEBUG
    Serial.print("packet received size: ");
    Serial.print(len);
    Serial.print("\n\n");
  #endif

  if (len == sizeof(ID)){
    ID received;
    memcpy(&received, incomingData, sizeof(ID));

    #if DEBUG
      Serial.print("Board received: ");
      Serial.println(received.id);
    #endif
   
    if (received.id == BOARD_01 && !initialTime) {
      initialTime = millis();
      #if DEBUG
        Serial.println("Time started");
      #endif
    }
    else if (received.id == BOARD_02 && initialTime && !finalTime){
      finalTime = millis();
      #if DEBUG
        Serial.println("Time finished");
      #endif
      newPass = true;
    }
  }
  else if (len == sizeof(data)) {
    data received;
    memcpy(&received, incomingData, sizeof(data));
    velo = received.velo;
    Serial.print("Speed: ");
    Serial.print(velo);
    Serial.print("m/s");
    Serial.print(", Speed: ");
    Serial.print(velo * 3.6);
    Serial.println("km/h");
    
  }
  else 
    Serial.println("Incoming data does not match");

}

void activation(bool act) {
  activ = act;
  set_up test;
  test.calibration = calibration;
  test.activated = activ;

  esp_now_send(sender_01Address, (uint8_t *) &test, sizeof(set_up));

  esp_now_send(sender_02Address, (uint8_t *) &test, sizeof(set_up));

  esp_now_send(sender_03Address, (uint8_t *) &test, sizeof(set_up));

}

void setCalib(short calib) {
  calibration = calib;
  set_up test;
  test.calibration = calibration;
  test.activated = activ;

  esp_now_send(sender_01Address, (uint8_t *) &test, sizeof(set_up));

  esp_now_send(sender_02Address, (uint8_t *) &test, sizeof(set_up));

  esp_now_send(sender_03Address, (uint8_t *) &test, sizeof(set_up));
}
