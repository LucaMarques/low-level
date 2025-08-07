/*
    Temperature reading with GY-906 MLX90614 sensors
*/

/* 
    command list
- start: start writing data
- stop: stop writing data
- new: create new file
- restart: restart base esp
*/

// -----FreeRTOS includes-----
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include "soc/rtc_wdt.h"

// -----espnow includes-----
#include <esp_now.h>
#include <WiFi.h>

// -----sd card includes-----
#include <Wire.h>
#include <FS.h>
#include <SPI.h>
#include <SD.h>

// -----temperature sensor include-----
#include <Adafruit_MLX90614.h>

// -----include external files-----
#include "configs.h"      // general configs
#include "lookup_table.h" // static parameters
#include "var_global.h"   // global variables must be declared before functions definitions
#include "prototypes.h"   // functions prototypes must be explicitly declared 
#include "tasks.h"        // task functions definitions
#include "SD_module.h"    // SD card module functions definitions
#include "system.h"       // system functions
#include "esp-now.h"      // esp-now functions

void setup(){
    // -----always start setup with pinMode-----
    pinMode(CHIPSELECT, OUTPUT);

    // -----Serial initiallization-----
    Serial.begin(SERIAL_FREQ);
    while (!Serial){}; // wait until is initialized
    Wire.begin();
    
    // -----header-----
    Serial.println("|-------------------------|");
    Serial.println("|                         |");
    Serial.println("|        core-base        |");
    Serial.println("|                         |");
    Serial.println("|-------------------------|");

    // -----ESPNOW settings-----
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    if (esp_now_init() != ESP_OK)    // check if was initialized successfully
        ERROR("ERROR: initializing ESP-NOW", false); // DO NOT send through esp-now
    INFO("INFO: Done initializing ESP-NOW", false); // DO NOT send through esp-now
    esp_now_register_send_cb(OnDataSent);

    // register peer
    esp_now_peer_info_t peerInfo = {};
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    memcpy(peerInfo.peer_addr, address_control, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
        ERROR("ERROR: Failed to add peer", false); // DO NOT send through esp-now
    INFO("INFO: Peer added");
    delay(50); // give time to send the espnow message
    esp_now_register_recv_cb(OnDataRecv);

    // -----create semaphores-----
    mutex_SD = xSemaphoreCreateMutex();
    if(mutex_SD == NULL)
        ERROR("ERROR: Semaphore init failed");

    // -----create queues-----
    queue_handler = xQueueCreate(QUEUE_SIZE, sizeof(queue_data));

    // -----File initiallization-----
    if (!SD.begin(CHIPSELECT))
        ERROR("ERROR: File initialization failed!");
    INFO("INFO: file initialization done.");
    delay(50); // give time to send the espnow message

    createFile();
    INFO("INFO: file created");
    delay(50); // give time to send the espnow message

    // -----external modules initialization-----
    // 
    //

    // -----final confirmation-----
    INFO("INFO: it's all done!");
    delay(50); // give time to send the espnow message
}

void loop(){
   vTaskDelete(NULL);
}
