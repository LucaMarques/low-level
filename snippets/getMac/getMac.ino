#ifdef ESP32
    #include <WiFi.h>
    #define BAUDRATE 115200
#else
    #include <ESP8266WiFi.h>
    #define BAUDRATE 74880
#endif

void setup(){
    Serial.begin(BAUDRATE);
    Serial.println();
    Serial.print("ESP Board MAC Address:  ");
    Serial.println(WiFi.macAddress());
}
 
void loop(){

}