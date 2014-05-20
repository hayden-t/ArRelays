#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

typedef struct {
  char name[16];//max characters for name
  boolean toggle;//if false, = momentary
  boolean status;//on or off
  boolean saved;//if status saved to eeprom
  boolean highbeam;//dependant on high beam active
  int pin;//which pin number relay is connected to
} relay;

const int maxRelays = 13;//must equal number of specified relays

relay relays[maxRelays + 1] = {
  {""},//not used
  {"ROOF LIGHT BAR ", true, false, true, true, 23},
  {"BULL BAR LIGHT ", true, false, true, true, 25},
  {"LED SPOTTIES   ", true, false, true, true, 27},
  {"9\" HID LIGHT  ", true, false, true, true, 29},
  {"7\" HID LIGHT  ", true, false, true, true, 31},
  {"LEFT LED BAR   ", true, false, true, false, 33},
  {"RIGHT LED Bar  ", true, false, true, false, 35},
  {"REAR LED BAR   ", true, false, true, false, 37},
  {"AUX BATTERY    ", true, false, false, false, 39},
  {"WINCH MASTER   ", true, false, false, false, 41},
  {"WINCH IN       ", false, false, false, false, 43},
  {"WINCH OUT      ", false, false, false, false, 45},
  {"REVERSE CAMERA ", true, false, false, false, 47}      
};

int highBeamPin = 53;//input pin detect high beam status
boolean highBeam = false;//high beam status

//init LCD
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
int lcdLines = 4;

int menuState = 1;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  Serial.print("Controller Starting Up...");
  
  //Wire.begin();
  lcd.begin(20, lcdLines);
  lcd.backlight();
  
   for(int i = 1; i <= maxRelays; i++){
       pinMode(relays[i].pin, OUTPUT);
  }
  
  pinMode(highBeamPin, INPUT);   
  
  loadSettings();

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
          if(relays[menuState + i].status)lcd.print("ON ");
          else lcd.print("OFF");          
          
      }
      else if(!menuState)lcd.print("Back");
      else lcd.print("                    ");
      
   } 
}
int key = 247;//no key
void checkButtons(void){
  
 Wire.requestFrom(0x26,1);
 
 if(Wire.available())     //If the request is available
    {
     byte data = Wire.read();       //Receive the data
     
     if(data != key){//key changed state
        key = data;
        //lcd.clear(); //not needed ?
        switch (key){
             case 246: //up
               if(menuState > 1)menuState--;
             break;
             case 245://down              
               if(menuState < maxRelays)menuState++;                  
             break;
             case 243: //enter
                 relays[menuState].status = true;                    
                 if(relays[menuState].saved)EEPROM.write(menuState, 1); 
             break;
             case 231: //off/escape
                   relays[menuState].status = false;
                   if(relays[menuState].saved)EEPROM.write(menuState, 0);
             break;
             case 247://no key
                 if(!relays[menuState].toggle) relays[menuState].status = false;
             break;
        }    
   }
}
  //if(((millis() - buttonTimer) > 10000) && (viewEvent != 1 || openMenu))home();
}

void outputRelays(void){
  
    highBeam = (analogRead(highBeamPin) > 205 ? true : false);//appx. 1 volt of more
  
     for(int i = 1; i <= maxRelays; i++){
       if(relays[i].highbeam){//is subject to high beam
         if(highBeam)digitalWrite(relays[i].pin, relays[i].status);//high beam on
         else digitalWrite(relays[i].pin, false);//high beam off
       }
       else digitalWrite(relays[i].pin, relays[i].status);//not subject to high beam
    }  
    
}

void loadSettings(void){
  
  for(int i = 1; i <= maxRelays; i++){       
      if(relays[i].saved)relays[i].status = (EEPROM.read(i) == 1 ? true : false);
  }

}
