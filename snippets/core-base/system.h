/*
    print ERROR messages on serial and through esp-now:
    - const char returnMsg[]: message to be sent
*/
void ERROR(char returnMsg[]){
    Serial.println();
    Serial.println(returnMsg);
    send_debug(returnMsg);

    on_error(returnMsg, true);
}

/*
    print ERROR messages on serial and through esp-now:
    - const char[]: message to be sent
    - bool: esp-now active flag
*/
void ERROR(char returnMsg[], bool espnow_active){
    Serial.println();
    Serial.println(returnMsg);
    if (espnow_active)
        send_debug(returnMsg);
    
    on_error(returnMsg, espnow_active);
}

/*
    print INFO messages on serial and through esp-now:
    - const char[]: message to be sent
*/
void INFO(const char returnMsg[]){
    Serial.println();
    Serial.println(returnMsg);
    send_debug(returnMsg);
}

/*
    print INFO messages on serial and through esp-now:
    - const char[]: message to be sent
    - bool: esp-now active flag
*/
void INFO(const char returnMsg[], bool espnow_active){
    Serial.println();
    Serial.println(returnMsg);
    if (espnow_active)
        send_debug(returnMsg);
}

void send_debug(const char returnMsg[]){
    if (sizeof(returnMsg) <= ESPNOW_BUFFER_SIZE){
        debug_data msg_data;
        msg_data.id = BOARDID_01;
        strcpy(msg_data.msg, returnMsg);
        esp_now_send(address_control, (uint8_t *) &msg_data, sizeof(msg_data));
    }
    else
        Serial.println("ERROR: espnow buffer overflow");
}

void on_error(char returnMsg[], bool espnow_active){
    if (REBOOT_ON_ERROR){
        ESP.restart();
        delay(1);
    }
    else{
        while(true){
            Serial.println(returnMsg);

            if (espnow_active)
                send_debug(returnMsg);
            
            delay(DEBUG_DELAY);
        }
    }
}