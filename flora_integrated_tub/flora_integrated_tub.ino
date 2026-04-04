///////////////////////////////////////////////////////////////////////////////
// Title: Flora Integrated Tub
// Revishion: 0.3
// 
// 3/31/2026 Version 0.2
// Added moisture sensor
// Fixed bug with menure interface not displatying correct menue
//
// 4/4/2026 Version 0.3
// Added support for DTH11 temp / humidity sensor
// Updated menu to include status for temp and humidity
// Removed code for PH sensor
// Removed PH sensor from menue
// Current co de does not compile in tinkerCad circuit simulator
//
// 4/4/2026 Version 0.4
// changed sensor suport from DTH11 to DTH22
///////////////////////////////////////////////////////////////////////////////




 // include the library code:
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <DHT.h>
#define DHTPIN 10
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);


int jStickXPin        = A0;
int jStickYPin        = A1;
int pResistor         = A2; // Photoresistor at Arduino analog pin A2
int moistureSensorPin = A3; // Moisture Sensor at Arduino analog pin A2

int JStickButtonPin   = 6;

int xVal;                 // variable for storing joystick x values
int yVal;                 // variable for storing joystick y values
int xVal2;                // variable for storing joystick x values
int yVal2;                // variable for storing joystick y values
int buttonState;          // variable for storing joystick push button state

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int WTCH_DOG_SCROLL_MENUE_MAX = 10;
const int WTCH_DOG_CNFG_MENUE_MAX = 10;



const int MAX_MENUE=5;
const int MIN_MENUE=0;

const String menueArray[]= {
  "LIGHT MAX",     // 0
  "LIGHT MIN",     // 1
  "TEMP MAX",      // 2
  "TEMP MIN",      // 3
  "MOISTURE MAX",  // 4
  "MOISTURE MIN",  // 5
  "HUMIDITY MAX",  // 6
  "HUMIDITY MIN"   // 7  
};

// These arrays will be used to store the current value, Max threshold, 
// Min threshold for the Moisture, PH and Light sensor readings
// The sensor index is 
// Light Sensor       = 0
// Temp Sensor        = 1
// Moisture Sensor    = 2
// Humidity Sensor    = 3
float currValArray[]={0,0,0,0}; 



// These arrays will be used to store the max and Minum allowabel threshold levels
// The sensor index is 
//  LIGHT MAX     // 0 Defualt 1200
//  LIGHT MIN     // 1 Default 
//  TEMP MAX      // 2
//  TEMP MIN      // 3
//  MOISTURE MAX  // 4
//  MOISTURE MIN  // 5
//  HUMIDITY MAX  // 6
//  HUMIDITY MIN  // 7
const int SENSOR_MAX_SETTING_ARRAY[]={2000,0,10,0,200,0,10,0};
int limitThrsArray[]={1000,500,10,10,100,10,0,0}; // Sets Defualts

float hum;  //Stores  humidity value
float temp; //Stores temperature value

// Function prototypes
int displayStatus();
int checkControls();
int scrollMenue();
int printSubConfigMenue(int setVal ,int maxVal, int minVal, String menueTitle);
int printMenue(int currMenu, int nextMenu);



///////////////////////////////////////////////////////////////////////////////
// Hardware Setup 
///////////////////////////////////////////////////////////////////////////////
void setup() {  
  pinMode(jStickXPin, INPUT);
  pinMode(jStickYPin, INPUT);
  pinMode(JStickButtonPin, INPUT_PULLUP);  
  
  dht.begin();  
  // set up the LCD's number of columns and rows:  
  lcd.begin(16, 2);
  //pinMode(switchPin, INPUT_PULLUP);
  Serial.begin(9600); // initialize the serial monitor
 

}



///////////////////////////////////////////////////////////////////////////////
// Main Loop
/////////////////////////////////////////////////////////////////////////////// 
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
// Function Name : checkSensors
// Inputs        : None
// Returns       : Nothing
//
// Description : Function reads the analog data from the sensors interfaced
//               to the aruduino board. 
///////////////////////////////////////////////////////////////////////////////
int checkSensors(){

// Light Sensor       = 0
// Temp Sensor        = 1
// Moisture Sensor    = 2
// Humidity Sensor    = 3
  
  // Light sensor    
  currValArray[0] = analogRead(pResistor);
  // Temp Sensor
  currValArray[1] = dht.readTemperature();  
  // Moisture Sensor   
  int tmpRd = analogRead(moistureSensorPin);
  currValArray[2] = map(tmpRd, 0, 1023, 255, 0);
  
  // Humidity Sensor
  currValArray[3] = dht.readHumidity();


    Serial.print("checkSensors: pResistor:  ");  
    Serial.print(currValArray[0]); 
    Serial.print("\n");
    Serial.print("checkSensors: moistureSensorPin:  ");  
    Serial.print(currValArray[2]); 
    Serial.print("\n");

}  


///////////////////////////////////////////////////////////////////////////////
// Function Name : checkControls
// Inputs        : None
// Returns       : Nothing
//
// Description : 
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
// Function Name : displayStatus
// Inputs        : None
// Returns       : Nothing
//
// Description : 
//               
///////////////////////////////////////////////////////////////////////////////
int displayStatus(){

    int userInput=0;
   String displayString=""; 
   String statusString="";    
   const String statStringArray[]= {
    "Light Exp:",    // 0
    "temperature:",  // 1
    "Soil Mstr:",    // 2
    "Humidity:"      // 3	
    };    

//  LIGHT MAX     // 0
//  LIGHT MIN     // 1
//  Temp MAX      // 2
//  Temp MIN      // 3
//  MOISTURE MAX  // 4
//  MOISTURE MIN  // 5
//  HUMIDITY MAX  // 4
//  HUMIDITY MIN  // 5

    while(userInput==0){  
      for (int i=0; i<4; i++) {
        displayString=statStringArray[i]+currValArray[i];
        // Check MAX
        if(currValArray[i] > limitThrsArray[(i*2)] ) {
          statusString="STATUS: ALARM HI";
        // CHECK MIN  
        } else if(currValArray[i] < limitThrsArray[(i*2)+1] ) {
          statusString="STATUS: ALARM LO";          
        } else {
          statusString="STATUS: GOOD";
        }
        lcd.setCursor(0, 0);
        lcd.print("                ");   // This is done to 0 out the 2nd row
        lcd.setCursor(0, 0);        
        lcd.print(statusString);
        lcd.setCursor(0, 1);        
        lcd.print("                ");   // This is done to 0 out the 2nd row
        lcd.setCursor(0, 1);     
        lcd.print(displayString);          

        Serial.print("displayStatus: displayString= ");  
        Serial.print(displayString);
        Serial.print("--"); 
        Serial.print(i);        
        Serial.print("\n");  

        for(int j=0; j<250;j++){
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
// Function Name : scrollMenue
// Inputs        : None
// Returns       : Nothing
//
// Description : 
//               
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
// Function Name : printSubConfigMenue
//
// Inputs        : int setVal
//                 int maxVal
//                 int minVal
//                 String menueTitle
//
// Returns       : int setVal
//
// Description : 
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
            setVal=setVal+5;        
          }
        } else if(yVal < 100) { // DOWN
          localWatchDog=0;        
          if(setVal>minVal){
            Serial.print("printSubConfigMenue: setVal ");  
            Serial.print(setVal);
            Serial.print("\n"); 
            setVal=setVal-5;      
          }
        }

        lcd.setCursor(0, 0);
        lcd.print("                ");   // This is done to 0 out the 2nd row   
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
// Function Name : printMenue
// Inputs        : int currMenu
//               : int nextMenu
//               
// Returns       : int localCurrMenu
//
// Description : 
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
    lcd.print("                ");   // This is done to 0 out the row       
    lcd.setCursor(0, 0);    
    lcd.print(">"+menueArray[localCurrMenu]);
    lcd.setCursor(0, 1);
    lcd.print("                ");   // This is done to 0 out the row           
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