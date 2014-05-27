#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

const int LCD_TIMEOUT = 10;//seconds

const int LCD_ADDRESS = 0x27;
const int BUTTON_ADDRESS = 0x26;

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
  }
  
  pinMode(highBeamPin, INPUT);   
  
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
          if(relays[menuState + i].status)lcd.print("ON ");
          else lcd.print("OFF");          
          
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
                   relays[menuState].status = (relays[menuState].status ? false:true);             
                   if(relays[menuState].saved)EEPROM.write(menuState, int(relays[menuState].status));
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
