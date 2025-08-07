/*
    delete existing data and write the message to the top:
    - fs::FS: file object
    - const char*: complete file path starting with / (/folder/file.csv)
    - const char*: message to be written on the file
*/
void writeFile(fs::FS &fs, const char *path, const char *message){
    xSemaphoreTake(mutex_SD, portMAX_DELAY);
        File file = fs.open(path, FILE_WRITE);
        
        if(!file)
            //-----if the file didn't open, print an error-----
            ERROR("ERROR: writeFile: error opening file");
        
        if(!file.print(message))
            //-----if the file didn't write, print an error-----
            ERROR("ERROR: writeFile: error writing file"); 
        
        file.close();
    xSemaphoreGive(mutex_SD);
}

/*
    keep existing data and write the message to the botton:
    - fs::FS: file object
    - const char*: complete file path starting with / (/folder/file.csv)
    - const char*: message to be written on the file
*/
void appendFile(fs::FS &fs, const char * path, const char * message){
    xSemaphoreTake(mutex_SD, portMAX_DELAY);
        File file = fs.open(path, FILE_APPEND);
        
        if(!file)
            ERROR("ERROR: appendFile: error opening file");
        
        if(!file.print(message))
            ERROR("ERROR: appendFile: error appending file");  
        
        file.close();
    xSemaphoreGive(mutex_SD);
}

/*
    search for an available filename, create a new file and write the title
*/
void createFile(void){
    xSemaphoreTake(mutex_SD, portMAX_DELAY);
        // -----find the next file name available-----
        short n = 0;
        while (SD.exists(FILE + String(n) + EXTENSION)) n++;
        
        fileName = FILE + String(n) + EXTENSION;
    xSemaphoreGive(mutex_SD);
        
    // -----create file and write title-----
    writeFile(SD, fileName.c_str(), TITLE);
}

/*
    convert a struct data array into a single string. returns the converted string:
    - struct *data: array of struct data to be convert
*/
String data_to_string(queue_data database[]){
    String return_msg;
    
    for(int i = 0; i < BUFFER_SIZE; i++){
        return_msg += 
            String(database[i].current_time) + "," +
            String(database[i].sensor_example, 2) + "\r\n"; // convert float to string with 2 decimal places
    }

    return return_msg;
}