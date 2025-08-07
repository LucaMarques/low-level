//-----includes-----
#include <FS.h>
#include <SPI.h>
#include <SD.h>


//-----definitions-----
#define mainSwitchPin   14
#define chipSelect      5
#define buzzerPin       21
#define yellowPin       25
#define greenPin        33
#define bluePin         32
#define redPin          26
#define InductivePin    27
#define HallPin         22
#define DIAMETER        0.4/3.0  // 1/3 do diâmetro da roda do carro (em metros)
#define AVERAGE         3.0
#define HALLDELAY       4000
#define NO_ACTIVITY     4000

//-----variables-----
int loops = false;                // Número de loops totais
unsigned long lastLoop = false;   // Instante do último loop da roda recordado
unsigned long lastHall = false;   // Instante do último loop do motor recordado
unsigned long lastSwitch = false; // Instante do último início de gravação
short hallCount = false;          

bool indCurrentRead = false;  // Leitura atual do sensor indutivo
bool indLastRead = false;     // Última leitura do sensor indutivo
bool hallCurrentRead = false; // Leitura atual do sensor Hall
bool hallLastRead = false;    // Última leitura do sensor Hall 

float travelledDistance = false; // Distância percorrida
float instantVelocity = false;   // Velocidade instantânea 

int RPM = false;               // rotações por min 
unsigned long totalTime = false;       // Tempo total decorrido desde o início de gravação

bool onOff = false;
bool SwitchTrack = false;
String fileName = "max_length";


//-----objects----- 
File csvFile;


//-----functions-----
void createFile(void);
void updateData(void);
void writeData(unsigned long, float, float, int);


void setup() {
   //-----pin modes-----
   pinMode(mainSwitchPin,   INPUT_PULLUP);
   pinMode(chipSelect,            OUTPUT);
   pinMode(buzzerPin,             OUTPUT);
   pinMode(yellowPin,             OUTPUT);
   pinMode(greenPin,              OUTPUT);
   pinMode(bluePin,               OUTPUT);
   pinMode(redPin,                OUTPUT);
   pinMode(InductivePin,           INPUT);
   pinMode(HallPin,         INPUT_PULLUP);


   //-----initial confirmation-----
   digitalWrite(yellowPin, HIGH); // turn on yellow LED
   digitalWrite(buzzerPin, HIGH); 
   delay(500);
   digitalWrite(buzzerPin, LOW);


   //-----Serials initiallization-----
   Serial.begin(115200);  // esp default freq
   while (!Serial) 
      digitalWrite(redPin, HIGH); // turn on red LED
   digitalWrite(redPin, LOW); // turn off red LED
   Serial.println("\n\n\nValidation");

   
   //-----File initiallization-----
   Serial.println("Initializing SD card...   ");
   if (!SD.begin(chipSelect)) {
      Serial.println("file initialization failed!");
      digitalWrite(redPin, HIGH); // turn on red LED
      ESP.restart(); // keep restarting until it works
   }
   createFile();
   Serial.println("file initialization done.");


   //-----final confirmation-----
   delay(500);
   digitalWrite(buzzerPin, HIGH); 
   delay(500);
   digitalWrite(buzzerPin, LOW);
   Serial.println("It's all done!");
   digitalWrite(yellowPin, LOW); // turn off yellow LED
   digitalWrite(greenPin, HIGH); // turn on green LED
}

void loop() {
   /***********************************************************************************
   * The program will start with 1 beep and is ready to be used after the second beep *
   ************************************************************************************/

   //-----read main switch-----
   onOff = !digitalRead(mainSwitchPin); // maybe needs a debounce function

   //-----main switch-----
   if (onOff){

      //-----change file after main switch rising edge detection-----
      if (!SwitchTrack){
         createFile();
         lastSwitch = millis();
         lastLoop = millis();
         lastHall = millis();
         loops = 0;
      }
      
      digitalWrite(bluePin, HIGH); // turn on blue LED

      //-----update all variables-----
      updateData();

      Serial.print("rpm: ");
      Serial.print(RPM);
      Serial.print("    Velo: ");
      Serial.println(instantVelocity);
   
      //-----Write in the file-----
      writeData(totalTime, travelledDistance, instantVelocity, RPM);
   }
   else {
      digitalWrite(bluePin, LOW); // turn off blue LED
   }

   //-----preserve the last main switch state-----
   SwitchTrack = onOff;
   
}

void createFile(void){
   short n = 0;

   while(SD.exists("/PWR_T" + String(n) + ".csv")) n++;

   fileName = "/PWR_T" + String(n) + ".csv";

   writeFile(SD, fileName.c_str(), "totalTime,travelledDistance,instantVelocity,RPM\r\n");

}

void updateData(void){

   // ------------ Sensor Indutivo ------------ //
   
   indCurrentRead = !digitalRead(InductivePin); // Recepção do sinal
   unsigned long instantTime1 = millis(); // Variável para armazenar instante de tempo atual

   if(indCurrentRead && !indLastRead){
      loops++;
      instantVelocity = (3.14*DIAMETER) / ((instantTime1 - lastLoop) / 1000.0);
      lastLoop = instantTime1;
   }

   if((instantTime1 - lastLoop) > NO_ACTIVITY) // 4 segundos de inércia para entender como Velocidade = 0
      instantVelocity = 0;

   indLastRead = indCurrentRead; // Atualização da nova última leitura


   // ------------ Sensor Hall ------------ //

   hallCurrentRead = !digitalRead(HallPin); // Recepção do sinal
   unsigned long instantTime2 = millis(); // Variável para armazenar instante de tempo atual

   if(hallCurrentRead && !hallLastRead && (instantTime2 - lastHall) > HALLDELAY){
      hallCount++;
      if (hallCount >= AVERAGE) {
         RPM = AVERAGE/(((instantTime2 - lastHall) / 1000.0) / 60.0);
         lastHall = instantTime2;
      }
   }   

   if((instantTime2 - lastHall) > NO_ACTIVITY) // 4 segundos de inércia para entender como RPM = 0
      RPM = 0;

   hallLastRead = hallCurrentRead; // Atualização da nova última leitura  


   // ------------ Cálculo de dados ------------ //
   travelledDistance = loops * DIAMETER;

   totalTime = millis() - lastSwitch;

}

void writeData(unsigned long Time, float travelledDist, float instantVel, int rpm){
   
   String dataString = String(Time) + "," +
                       String(travelledDist) + "," +
                       String(instantVel) + "," +
                       String(rpm) + "\r\n";

   appendFile(SD, fileName.c_str(), dataString.c_str());

}

void writeFile(fs::FS &fs, const char * path, const char * message){
   Serial.printf("Writing file: %s\n", path);

   File file = fs.open(path, FILE_WRITE);
   if(!file){
      //-----if the file didn't open, print an error-----
      Serial.print("createFile: error opening ");
      Serial.println(fileName);
      digitalWrite(redPin, HIGH); // turn on red LED
      while (1);
   }
   if(file.print(message)){
      Serial.println("File written");
   } else {
      //-----if the file didn't write, print an error-----
      Serial.print("createFile: error writing to ");
      Serial.println(fileName);
      digitalWrite(redPin, HIGH); // turn on red LED
      while (1);
   }
   file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
   Serial.printf("Appending to file: %s\n", path);

   File file = fs.open(path, FILE_APPEND);
   if(!file){
      Serial.print("writeData: error opening ");
      Serial.println(fileName);
      digitalWrite(redPin, HIGH); // turn on red LED
      while (1);
   }
   if(file.print(message)){
      Serial.println("Message appended");
   } else {
      Serial.print("writeData: error appending to ");
      Serial.println(fileName);
      digitalWrite(redPin, HIGH); // turn on red LED
      while (1);
   }
   file.close();
}
