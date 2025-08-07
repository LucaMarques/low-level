#include <ESP8266WiFi.h>
#include <Adafruit_AHTX0.h>
#define FORCE_DEEPSLEEP
#define RELE D4

Adafruit_AHTX0 aht;
Adafruit_Sensor *aht_humidity, *aht_temp;

unsigned long previousMillis = 0;   // Stores last time temperature was published
const long interval = 10000;        // Interval at which to publish sensor readings


void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  Serial.println();

  if (!aht.begin()) {
    Serial.println("Failed to find AHT10/AHT20 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("AHT10/AHT20 Found!");
  aht_temp = aht.getTemperatureSensor();
  aht_temp->printSensorDetails();

  aht_humidity = aht.getHumiditySensor();
  aht_humidity->printSensorDetails();

  pinMode(RELE, OUTPUT);

}

void loop() {
  unsigned long currentMillis = millis();
  // Every X number of seconds (interval = 10 seconds)
  // it publishes a new MQTT message
  if (currentMillis - previousMillis >= interval) {
    // Save the last time a new reading was published
    previousMillis = currentMillis;

    sensors_event_t humidity;
    sensors_event_t temp;
    aht_humidity->getEvent(&humidity);
    aht_temp->getEvent(&temp);

    Serial.printf("Message: %.2f \n", temp.temperature);
    Serial.printf("Message: %.2f \n", humidity.relative_humidity);

    if(temp.temperature > 35){
      digitalWrite(RELE, HIGH);
    }
    else
      digitalWrite(RELE, LOW);
      
    delay(2000);
  }
}
