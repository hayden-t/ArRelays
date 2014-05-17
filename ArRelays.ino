//#include <Wire.h>
#include <LiquidCrystal.h>
//#include <EEPROM.h>


typedef struct {
  char name[15];//max characters for name
  boolean toggle;//if false, = momentary
  boolean status;//on or off
  boolean saved;//if status saved to eeprom
  boolean hibeam;//dependant on high beam active
} relay;

const int maxRelays = 13;//must equal number of specified relays

relay relays[maxRelays + 1] = {
  {""},//not used
  {"ROOF LIGHT BAR", true, false, true, true},
  {"BULL BAR LIGHT", true, false, true, true},
  {"LED SPOTTIES", true, false, true, true},
  {"9\" HID LIGHT", true, false, true, true},
  {"7\" HID LIGHT", true, false, true, true},
  {"LEFT LED BAR", true, false, true, false},
  {"RIGHT LED Bar", true, false, true, false},
  {"REAR LED BAR", true, false, true, false},
  {"AUX BATTERY", true, false, false, false},
  {"WINCH MASTER", true, false, false, false},
  {"WINCH IN", false, false, false, false},
  {"WINCH OUT", false, false, false, false},
  {"REVERSE CAMERA", true, false, false, false}      
};


//init LCD
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
int lcdLines = 4;

int menuState = 0;

int stateA0 = HIGH;
int stateA1 = HIGH;
int stateA2 = HIGH;
int stateA3 = HIGH;
unsigned long buttonTimer = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  Serial.print("Controller Starting Up...");
  
  //Wire.begin();
  lcd.begin(20, lcdLines);
  
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  digitalWrite(A0, stateA0);
  digitalWrite(A1, stateA1);
  digitalWrite(A2, stateA2);
  digitalWrite(A3, stateA3);
  

}

void loop() {
  // put your main code here, to run repeatedly:
    
    checkButtons();
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


          
      }else if(!menuState)lcd.print("Back");
      
   } 
}
void checkButtons(void){
 //check buttons
 
//down button 
  int reading = analogRead(A0);
  if((reading < 512) && (stateA0 == HIGH)){
    stateA0 = LOW;   
    //if(!openMenu){
      //if(viewEvent < eventCount())viewEvent++;
    //}else{
      if(menuState < maxRelays)menuState++;
    //}   
    lcd.clear();
    buttonTimer = millis();
    //digitalWrite(ledPin, LOW);
    //Serial.println("Button1");
  }else if((reading > 512) && (stateA0 == LOW)){
    stateA0 = HIGH;
  }
  
//up button  
  reading = analogRead(A1);
  if((reading < 512) && (stateA1 == HIGH)){
    stateA1 = LOW;    
   // if(!openMenu){
   //   if(viewEvent > 1)viewEvent--;
   // }else{
      if(menuState > 0)menuState--;
    //}    
      lcd.clear();
      buttonTimer = millis();
      //digitalWrite(ledPin, LOW);
    //Serial.println("Button2");
  }else if((reading > 512) && (stateA1 == LOW)){
    stateA1 = HIGH;
  }
  
//enter button 
  reading = analogRead(A2);
  if((reading < 512) && (stateA2 == HIGH)){
    stateA2 = LOW;   
    //if(!openMenu)openMenu = true;
    //else toggleAlarm(menuState);    
    lcd.clear();
    buttonTimer = millis();
    //digitalWrite(ledPin, LOW);
    //Serial.println("Button3");
  }else if((reading > 512) && (stateA2 == LOW)){
    stateA2 = HIGH;
  }
  
  //mode button 
  reading = analogRead(A3);
  if((reading < 512) && (stateA3 == HIGH)){
    stateA3 = LOW;   
   // changeMode();
    buttonTimer = millis();
  }else if((reading > 512) && (stateA3 == LOW)){
    stateA3 = HIGH;
  }
  
  //if(((millis() - buttonTimer) > 10000) && (viewEvent != 1 || openMenu))home();
}
