///////////////////////////////////////////////////////////////////////////////
// Title UI Menue
// Revishion 2.1
// 
//
///////////////////////////////////////////////////////////////////////////////
 // include the library code:
#include <LiquidCrystal.h>


int jStickXPin = A0;
int jStickYPin = A1;
int JStickButtonPin = 6;
int pResistor = A2; // Photoresistor at Arduino analog pin A0
int xVal; // variable for storing joystick x values
int yVal; // variable for storing joystick y values
int xVal2; // variable for storing joystick x values
int yVal2; // variable for storing joystick y values
int buttonState;

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int WTCH_DOG_SCROLL_MENUE_MAX = 10;
const int WTCH_DOG_CNFG_MENUE_MAX = 10;



const int MAX_MENUE=5;
const int MIN_MENUE=0;

const String menueArray[]= {
  "PH MAX",        // 0
  "PH MIN",        // 1
  "MOISTURE MAX",  // 2
  "MOISTURE MIN",  // 3
  "LIGHT MAX",     // 4
  "LIGHT MIN"      // 5
};

// These arrays will be used to store the current value, Max threshold, 
// Min threshold for the Moisture, PH and Light sensor readings
// The sensor index is 
// Light Sensor       = 0
// Ph Sensor = 1
// Moisture Sensor    = 2
int currValArray[]={0,0,0}; 



// These arrays will be used to store the max and Minum allowabel threshold levels
// The sensor index is 
//  PH MAX        // 0
//  PH MIN        // 1
//  MOISTURE MAX  // 2
//  MOISTURE MIN  // 3
//  LIGHT MAX     // 4
//  LIGHT MIN     // 5
const int SENSOR_MAX_SETTING_ARRAY[]={112,112,10,10,24,24};
int limitThrsArray[]={0,0,0,0,0,0};



void setup() {
  pinMode(jStickXPin, INPUT);
  pinMode(jStickYPin, INPUT);
  pinMode(JStickButtonPin, INPUT_PULLUP);  
 // set up the LCD's number of columns and rows:  
  lcd.begin(16, 2);
  //pinMode(switchPin, INPUT_PULLUP);
  Serial.begin(9600); // initialize the serial monitor
}
 
void loop() {

  int userInput=0;

  //
  // TODO NEED TO ADD SENSOR DATA COLLECITON
  //
  checkSensors();
  // check for user input and print menue
  userInput=displayStatus();
  if(userInput==1){
    scrollMenue();
    lcd.clear();
  }


}



///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
int checkSensors(){
  // Light sensor
    currValArray[0] = analogRead(pResistor);
    Serial.print("checkSensors: pResistor:  ");  
    Serial.print(currValArray[0]); 
    Serial.print("\n");
}  

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
int checkControls(){
  // read the x, y and joystick switch values
  //Serial.print("checkControls : entered\n");  
  xVal = analogRead(jStickXPin);
  yVal = analogRead(jStickYPin);
  delay(10);
  xVal2 = analogRead(jStickXPin);
  yVal2 = analogRead(jStickYPin);
  if((abs(xVal - xVal2) > 100) || (abs(yVal - yVal2) > 100)){
    Serial.print("checkControls: xvals | ");  
    Serial.print(xVal);
    Serial.print(" | ");    
    Serial.print(xVal2);  
    Serial.print("\n");

    Serial.print("checkControls: yvals | ");  
    Serial.print(yVal);
    Serial.print(" | ");    
    Serial.print(yVal2);  
    Serial.print("\n");
    return 1;
  }
    return 0;  
}
///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
int displayStatus(){

    int userInput=0;
   String displayString=""; 
   const String statStringArray[]= {
    "Light Exp:",     // 0
    "Soil Ph:",      // 1
    "Soil Mstr:"  // 2
    };    

    while(userInput==0){
      lcd.setCursor(0, 0);
      lcd.print("STATUS");
    
      for (int i=0; i<3; i++) {
        lcd.setCursor(0, 1);        
        lcd.print("                ");   // This is done to 0 out the 2nd row
        lcd.setCursor(0, 1);
        displayString=statStringArray[i]+currValArray[i];
        Serial.print("displayStatus: displayString= ");  
        Serial.print(displayString);
        Serial.print("--"); 
        Serial.print(i);        
        Serial.print("\n");         
        lcd.print(displayString);          
        for(int j=0; j<150;j++){
          userInput=checkControls();
          if(userInput==1){
            return 1;
          }    
        }

      }
    return 1;      
    }
    return 1;
}//displayStatus
///////////////////////////////////////////////////////////////////////////////
// Scroll Menue
///////////////////////////////////////////////////////////////////////////////
int scrollMenue(){
  //  
  int wtchDogMenueScroll = 0;
  int currMenu=0; 
  int prevMenu=0; 
  int nextMenu=0;  
  while (wtchDogMenueScroll <= WTCH_DOG_SCROLL_MENUE_MAX){

  // read the x, y and joystick switch values
  xVal = analogRead(jStickXPin);
  yVal = analogRead(jStickYPin);    
  buttonState = digitalRead(JStickButtonPin);
  bool menueChange = false; 
  bool enterSubMenue = false;   
  if(buttonState==0){
    Serial.println("scrollMenue: JOY STICK PRESSED BUTTON\n");      
    enterSubMenue=true;

  }else if(xVal < 400 ) { // LEFT
    Serial.println("scrollMenue: JOY STICK LEFT\n");  
  } else if(xVal > 800) { // RIGHT
    Serial.println("scrollMenue: JOY STICK RIGHT\n");  
  } else if(yVal > 800) { // UP
    Serial.println("scrollMenue: JOY STICK UP\n");  
    nextMenu=1;
    menueChange=true;   
  } else if(yVal < 100) { // DOWN
    Serial.println("scrollMenue: JOY STICK DOWN\n");  
    nextMenu=2; 
    menueChange=true;       
  }

  // If we scroll through the menue 
  if(enterSubMenue == true){
    Serial.print("scrollMenue: enterSubMenue prevMenu= ");  
    Serial.print(prevMenu);
    Serial.print("\n");


    //int printSubConfigMenue(int setVal ,int maxVal, int minVal, String menueTitle){
    limitThrsArray[prevMenu]=printSubConfigMenue(
                             limitThrsArray[prevMenu],
                             SENSOR_MAX_SETTING_ARRAY[prevMenu],
                             0,
                             menueArray[prevMenu]
                            );    
    printMenue(prevMenu,0);    
  } else if(menueChange == true){
    prevMenu=currMenu;
    currMenu=printMenue(currMenu,nextMenu);
    Serial.print("scrollMenue: menueChange return currMenu= ");  
    Serial.print(currMenu);
    Serial.print("\n");     
  }
  delay(250);
  if(menueChange || enterSubMenue){
    wtchDogMenueScroll=0;
  }else{
    wtchDogMenueScroll++;
  }

  }

}
///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
int printSubConfigMenue(int setVal ,int maxVal, int minVal, String menueTitle){
    int localWatchDog=0;
    int localButtonState=1;
    Serial.print("printSubConfigMenue: menueTitle ");  
    Serial.print(menueTitle);
    Serial.print("\n"); 
    Serial.print("printSubConfigMenue: setVal ");  
    Serial.print(setVal);
    Serial.print("\n"); 
    Serial.print("printSubConfigMenue: maxVal ");  
    Serial.print(maxVal);
    Serial.print("\n"); 
    Serial.print("printSubConfigMenue: minVal ");  
    Serial.print(maxVal);
    Serial.print("\n");     
    lcd.clear();      
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print(menueTitle);
    lcd.setCursor(0, 1);
    lcd.print(setVal);      
    delay(50);    
    while(localButtonState==1){
       localButtonState = digitalRead(JStickButtonPin);

        xVal = analogRead(jStickXPin);
        yVal = analogRead(jStickYPin);
        if(yVal > 800) { // UP
          localWatchDog=0;
          if(setVal<maxVal){
            Serial.print("printSubConfigMenue: setVal ");  
            Serial.print(setVal);
            Serial.print("\n"); 
            setVal++;        
          }
        } else if(yVal < 100) { // DOWN
          localWatchDog=0;        
          if(setVal>minVal){
            Serial.print("printSubConfigMenue: setVal ");  
            Serial.print(setVal);
            Serial.print("\n"); 
            setVal--;      
          }
        }

        lcd.setCursor(0, 0);
        lcd.print(menueTitle);
        lcd.setCursor(0, 1);
        lcd.print("                ");   // This is done to 0 out the 2nd row   
        lcd.setCursor(0, 1);      
        lcd.print(setVal);      
        delay(150);
        Serial.print("printSubConfigMenue: localWatchDog ");  
        Serial.print(localWatchDog);
        Serial.print("\n");             
        localWatchDog++;
        if(localWatchDog > WTCH_DOG_CNFG_MENUE_MAX){
         break;
        }
    };
    return setVal;

}



///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
int printMenue(int currMenu, int nextMenu) {

int localCurrMenu=currMenu;
int arryNextIndex=localCurrMenu+1;


if(arryNextIndex==(MAX_MENUE+1)){
  arryNextIndex=0; 
}
    lcd.clear();
    Serial.print("printMenue: localCurrMenu ");  
    Serial.print(localCurrMenu);
    Serial.print("\n");        
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print(">"+menueArray[localCurrMenu]);
    lcd.setCursor(0, 1);
    lcd.print(" "+menueArray[arryNextIndex]);
 


    // Check if the menue scrolls up or down 
    // 1 = UP, 2 = DOWN
    if(nextMenu == 0 ) { // Menue scroll up
      // If we are on the first menue option and we scroll up
      // We will need to wrap back the last menue 
      // other wise we will go up one to the previous menue option 
      if(localCurrMenu ==  MIN_MENUE){
        localCurrMenu=MAX_MENUE;
      } else {
        localCurrMenu=localCurrMenu-1;
      }   
    } else if (nextMenu == 2) { // Menue scroll down 
      // If we are on the last menue option and we scroll down
      // We will need to wrap back the first menue 
      // other wise we will go up one to the next menue option 
      if(localCurrMenu ==  MAX_MENUE){
        localCurrMenu=0;
      } else {
        localCurrMenu=localCurrMenu+1;
      }      
    }

  return localCurrMenu;
}