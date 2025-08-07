#include <Wire.h> 
#include <LiquidCrystal_I2C.h> 

#define HALLPIN 
#define AVERAGE 3.0

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

bool hallValue = true;
bool lastHall = true;
short count = false;
int RPM = false;
unsigned long timeValue = false;
unsigned long lastTime = false;

void setup(){
  pinMode(HALLPIN, INPUT_PULLUP);
  Serial.begin(115200);

  lcd.init();                      
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  Hello world!  ");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RPM :  ");
}

void loop() {
  hallValue = digitalRead(HALLPIN);

  // detectar borda de descida
  if (!hallValue && lastHall){
    count++;
    delay(3);
    if (count >= AVERAGE){
      count = false;
      lastTime = timeValue;
      timeValue = millis();

      RPM = AVERAGE / (((timeValue - lastTime) / 1000.0) / 60.0); // rotações por minuto

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("RPM :  ");
      lcd.print(RPM);
    }
  }

  lastHall = hallValue;
}
