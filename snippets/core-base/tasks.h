/*
    read data task running on core 1
*/
void task_readData(void *arg){ // read I2C bus
    (void)arg;

    // -----create local variables-----
    queue_data local_data;
    uint32_t last_time;
    uint32_t counter = false;

#if DEBUG_MODE
    // see the remaining space of this task
    // always after create the variables
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    char info[ESPNOW_BUFFER_SIZE];
    snprintf(
        info, 
        ESPNOW_BUFFER_SIZE,
        "INFO: %d bytes remaining of the readData task space", 
        uxHighWaterMark); 
    INFO(info);
    delay(50); // give time to send the espnow message
#endif

    // -----update timer-----
    last_time = millis();

    while (true){  // run continuously on core 1 (APP_CPU: application CPU)
        if ((millis() - last_time) >= RATE_ms){
            // -----add to timer-----
            last_time += RATE_ms;

            // -----read sensors-----
            local_data.current_time = millis();
            local_data.sensor_example = float(counter++);
            
            // -----send data to queue-----
            xQueueSend(queue_handler, &local_data, pdMS_TO_TICKS(0));
            // delay(RATE_ms/2); // wait half the rate period

            // -----clear local data-----
            memset(&local_data, 0, sizeof(local_data));
        }
        
        // -----feed the watchdog so this task can run without delay-----
        // yield();    // only needed if there is no pause
    }
}

/*
    write data task running on core 0
*/
void task_writeData(void *arg){
    (void)arg;

    // -----local variables-----
    queue_data data_received;
    queue_data buffer_data[BUFFER_SIZE];
    uint16_t buffer_count = false;

#if DEBUG_MODE
    // see the remaining space of this task
    // always after create the variables
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    char info[ESPNOW_BUFFER_SIZE];
    snprintf(
        info, 
        ESPNOW_BUFFER_SIZE,
        "INFO: %d bytes remaining of the writeData task space", 
        uxHighWaterMark); 
    INFO(info);
    delay(50); // give time to send the espnow message
#endif

    while (true){  // run continuously on core 1 (APP_CPU: application CPU)
        if (buffer_count < BUFFER_SIZE){
            if (xQueueReceive(queue_handler, &buffer_data[buffer_count], pdMS_TO_TICKS(QUEUE_TIMEOUT)) == false){
                ERROR("ERROR: timeout buffer_data");
            }
            else{
                buffer_count++;
            }
        }
        else{
            buffer_count = false;

            #if DEBUG_MODE
                uint32_t timer_sd = millis();
            #endif

            // -----write data-----
            String data_string = data_to_string(buffer_data);
            appendFile(SD, fileName.c_str(), data_string.c_str());

            #if DEBUG_MODE
                snprintf(
                    info, 
                    ESPNOW_BUFFER_SIZE,
                    "INFO: time to write file %ld ms", 
                    (millis() - timer_sd)); 
                INFO(info);
            #endif
            
            // -----clear buffer-----
            memset(buffer_data, 0, sizeof(buffer_data));
        }
    }
}
