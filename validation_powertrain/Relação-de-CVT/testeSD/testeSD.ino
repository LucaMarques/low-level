#include "FS.h"
#include "SD.h"
#include "SPI.h"

String file = "/datalog_ESP32.txt";

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void setup(){
  Serial.begin(115200);
  
  if(!SD.begin(5)){
    Serial.println("Card Mount Failed");
    return;
  }
    else {
    Serial.println("SD Card mounted with success");
  }
  
  writeFile(SD, file.c_str(), "Time, Humidity, Temperature \r\n");
}

void loop(){

  String dataString = "teste";
  appendFile(SD, file.c_str(), dataString.c_str());
    
  delay(2000);
}
