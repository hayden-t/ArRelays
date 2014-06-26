#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

const int LCD_TIMEOUT = 10;//seconds

const int LCD_ADDRESS = 0x27;
const int BUTTON_ADDRESS = 0x26;

typedef struct {
  char name[16];//max characters for name
  boolean toggle;//if false, = momentary
  byte status;//on/off/auto 0/1/2
  boolean saved;//if status saved to eeprom
  int depends;//pin relay dependant on active (for auto) 0 = none (uses digital read, >=3v : HIGH/ON , <=2v : LOW/OFF) (uses pullups)
  int pin;//which pin number relay is connected to
} relay;

const int maxRelays = 13;//must equal number of specified relays

relay relays[maxRelays + 1] = {
  {""},//not used
  {"ROOF LIGHT BAR ", true, 0, true, 53, 23},
  {"BULL BAR LIGHT ", true, 0, true, 53, 25},
  {"LED SPOTTIES   ", true, 0, true, 53, 27},
  {"9\" HID LIGHT  ", true, 0, true, 53, 29},
  {"7\" HID LIGHT  ", true, 0, true, 53, 31},
  {"LEFT LED BAR   ", true, 0, true, 0, 33},
  {"RIGHT LED Bar  ", true, 0, true, 0, 35},
  {"REAR LED BAR   ", true, 0, true, 0, 37},
  {"AUX BATTERY    ", true, 0, false, 0, 39},
  {"WINCH MASTER   ", true, 0, false, 0, 41},
  {"WINCH IN       ", false, 0, false, 0, 43},
  {"WINCH OUT      ", false, 0, false, 0, 45},
  {"REVERSE CAMERA ", true, 0, false, 0, 47}
};

//init LCD
LiquidCrystal_I2C lcd(LCD_ADDRESS, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
int lcdLines = 4;

int menuState = 1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  
  //Wire.begin();
  lcd.begin(20, lcdLines);
  lcd.backlight();
  
   for(int i = 1; i <= maxRelays; i++){
       pinMode(relays[i].pin, OUTPUT);
       if(relays[i].depends > 0)pinMode(relays[i].depends, INPUT_PULLUP);
  }  
 
  loadSettings();
  
  Serial.print("Controller Starting Up...");
  lcd.print("   Arduino Relay");
  lcd.setCursor(0, 1);
  lcd.print("     Controller");
  lcd.setCursor(0, 2);
  lcd.print(" ");
  lcd.setCursor(0, 3);
  lcd.print(" www.httech.com.au");
  delay(5000);  
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
    
    checkButtons();   
    outputRelays();
    drawMenu();

}

void drawMenu(void){
  
   for(int i = 0; i < lcdLines; i++){   

     lcd.setCursor(0, i);
      
      if(!i)lcd.print(">");
      else lcd.print(" ");
     
      if((menuState + i <= maxRelays) && (menuState || i)){
          lcd.print(relays[menuState + i].name);
          lcd.print(" ");
          
          lcd.setCursor(17, i);
          if(relays[menuState + i].status == 0)lcd.print("OFF");
          if(relays[menuState + i].status == 1)lcd.print("ON ");
          if(relays[menuState + i].status == 2)lcd.print("AUT");
          
      }
      else if(!menuState)lcd.print("Back");
      else lcd.print("                    ");
      
   } 
}
int key = 247;//no key
unsigned long buttonTimer = 1;

void checkButtons(void){

 Wire.requestFrom(BUTTON_ADDRESS,1);
 
 if(Wire.available())     //If the request is available
    {
     byte data = Wire.read();       //Receive the data
     
     if(data != key){//key changed state       
       key = data;
       if(buttonTimer){//awake          
          switch (key){
               case 246: //up
                 if(menuState > 1)menuState--;
               break;
               case 245://down              
                 if(menuState < maxRelays)menuState++;                  
               break;
               case 243: //enter
                   relays[menuState].status++;
                   if((relays[menuState].depends == 0 && relays[menuState].status > 1) || relays[menuState].status > 2)relays[menuState].status = 0;
                   if(relays[menuState].saved)EEPROM.write(menuState, relays[menuState].status);
               break;
               case 231: //escape

               break;
               case 247://no key
                   if(!relays[menuState].toggle) relays[menuState].status = false;
               break;
          }              
       }else{//wakeup         
           lcd.backlight();         
       }
       buttonTimer = millis();
     }
}
  if(millis() - buttonTimer > (LCD_TIMEOUT * 1000) && key == 247){buttonTimer = 0;lcd.noBacklight();}//screensaver or lcd.off();
}

void outputRelays(void){
  
     for(int i = 1; i <= maxRelays; i++){
       if(relays[i].status == 2){// auto
         if(digitalRead(relays[i].depends))digitalWrite(relays[i].pin, true);
         else digitalWrite(relays[i].pin, false);         
       }
       else digitalWrite(relays[i].pin, relays[i].status);
    }  
    
}

void loadSettings(void){
  
  for(int i = 1; i <= maxRelays; i++){       
      if(relays[i].saved)relays[i].status = EEPROM.read(i);
  }
  
}
