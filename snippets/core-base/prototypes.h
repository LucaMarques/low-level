// -----tasks functions-----

void task_readData(void*);
void task_writeData(void*);


// -----SD card module functions-----

void writeFile(fs::FS&, const char*, const char*);
void appendFile(fs::FS&, const char*, const char*);
void createFile(void);
String data_to_string(queue_data*);


// -----esp-now functions-----

void OnDataSent(const uint8_t*, esp_now_send_status_t);
void OnDataRecv(const uint8_t*, const uint8_t*, int);


// -----system-----

void ERROR(char*);
void ERROR(char*, bool);
void INFO(const char*);
void INFO(const char*, bool);
void send_debug(const char*);
void on_error(char*, bool);