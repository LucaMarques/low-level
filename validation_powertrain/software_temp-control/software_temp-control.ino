/*****************
*    Receiver    *
*****************/

#include <ESP8266WiFi.h>
#include <espnow.h>

#define DEBUG_MODE         false
#define BOARDID_01         1
#define ESPNOW_BUFFER_SIZE 48

// -----command lookup table-----
#define CMD_START    0x01
#define CMD_STOP     0x02
#define CMD_NEW_FILE 0x03
#define CMD_RESTART  0X04

uint8_t address_base[] = {0x7C, 0x9E, 0xBD, 0xED, 0x52, 0x70};

typedef struct set_up {
    uint8_t id;
    uint8_t command;
} set_up;

typedef struct debug_data{
    uint8_t id;
    char msg[ESPNOW_BUFFER_SIZE];
} debug_data;

String msg;

void OnDataSent(uint8_t*, uint8_t);
void OnDataRecv(uint8_t*, uint8_t*, uint8_t);
 
void setup() {
    Serial.begin(74880);

    // -----header-----
    Serial.println("|----------------------------|");
    Serial.println("|                            |");
    Serial.println("|        core-control        |");
    Serial.println("|                            |");
    Serial.println("|----------------------------|");
    
    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // Init ESP-NOW
    if (esp_now_init() != 0) {
        Serial.println("Error initializing ESP-NOW");
        ESP.restart();
        delay(1);
    }

    Serial.println("\nCommands:");
    Serial.println("start   - start writing data");
    Serial.println("stop    - stop writing data");
    Serial.println("new     - create new file");
    Serial.println("restart - restart board");
    Serial.println();
    
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

    esp_now_register_send_cb(OnDataSent);
    
    esp_now_add_peer(address_base, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
    
    esp_now_register_recv_cb(OnDataRecv); 
}

void loop(){
    while(Serial.available()) msg += Serial.readString();
    
    if (msg == "start\n"){
        //clean all variables
        msg = "";
        
        set_up config = {BOARDID_01, CMD_START};
        esp_now_send(address_base, (uint8_t *) &config, sizeof(set_up));

        Serial.println("\n\n----- start command sent -----");
    }
    else if (msg == "stop\n"){
        //clean all variables
        msg = "";
        
        set_up config = {BOARDID_01, CMD_STOP};
        esp_now_send(address_base, (uint8_t *) &config, sizeof(set_up));

        Serial.println("\n\n----- stop command sent -----");
    }
    else if (msg == "new\n"){
        //clean all variables
        msg = "";
        
        set_up config = {BOARDID_01, CMD_NEW_FILE};
        esp_now_send(address_base, (uint8_t *) &config, sizeof(set_up));

        Serial.println("\n\n----- new file command sent -----");
    }
    else if (msg == "restart\n"){
        //clean all variables
        msg = "";
        
        set_up config = {BOARDID_01, CMD_RESTART};
        esp_now_send(address_base, (uint8_t *) &config, sizeof(set_up));

        Serial.println("\n\n----- restart command sent -----");
    }
    else if (msg != "") {
        msg = "";
        Serial.println("\n\n----- command does not exist -----");
    }
}

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
#if DEBUG_MODE
    Serial.print("\r\nLast Packet Send Status: ");
    if (sendStatus == 0)
        Serial.println("Delivery success");
    else
        Serial.println("Delivery fail");
#endif
}

void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) {
    #if DEBUG_MODE
        Serial.print("packet received size: ");
        Serial.println(len);
    #endif

    debug_data received;
    memcpy(&received, incomingData, sizeof(received));

    Serial.print("received: ");
    Serial.print(received.msg);
    Serial.print("\n\n");
}
