/* ESP-NOW on data sent callback */
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status){
#if DEBUG_MODE
    // -----check if the data was delivered-----
    Serial.print("Send status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
#endif
}

/* ESP-NOW on data receive callback*/
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len){
    set_up config = {};
    memcpy(&config, incomingData, sizeof(config));

    if (config.command == CMD_START){
        xTaskCreatePinnedToCore(task_readData,         // task function
                                "readData",            // task name
                                2048,                  // stack size
                                NULL,                  // parameters
                                10,                    // priority
                                &TaskHandle_readData,  // handler  
                                APP_CPU_NUM);          // CPU core number

        xTaskCreatePinnedToCore(task_writeData, 
                                "writeData", 
                                8128, 
                                NULL, 
                                5, 
                                &TaskHandle_writeData, 
                                PRO_CPU_NUM);

        INFO("INFO: start writing data");
        delay(50); // give time to send the espnow message
    }
    else if (config.command == CMD_STOP){
        vTaskDelete(TaskHandle_readData);
        vTaskDelete(TaskHandle_writeData);

        INFO("INFO: stop writing data");
        delay(50); // give time to send the espnow message
    }
    else if (config.command == CMD_NEW_FILE){ 
        // -----create new file-----
        createFile();

        INFO("INFO: new file created");
        delay(50); // give time to send the espnow message
    }
    else if (config.command == CMD_RESTART){
        INFO("INFO: restarting . . .\n----------------------");
        delay(50); // give time to send the espnow message

        ESP.restart();
    }
    else{
        INFO("INFO: command does not exist");
        delay(50); // give time to send the espnow message
    }
}