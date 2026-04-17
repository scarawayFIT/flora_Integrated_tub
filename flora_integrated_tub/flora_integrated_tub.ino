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
//
// 4/11/2026 Version 0.5
// Added support for LED alarm indicator
// 
// refactored code to use classes for the sensor status 
// classes will be used to set the Max and Min threshold values
// and what is displayed in LCD for each sensor. 
//
// 4/11/2026 Version 0.6
// 
// Updated menu navigation to use edge detection of the push button to enter 
// the configuration menu from the status screen.
//
// updated the time out logice for the watch dogs to use the millis() 
// function rather than using delay(). 
//
// Known Defects : current the menu cannot scroll up. Scrolling down works and 
// wraps around but the user cannot scroll up. 
//
// 4/11/2026 Version 1.0
// 
// Fixed bug where menu would not scroll up if at the first menu option
// the wraping from the ealiest menu to the end of the menu was not working
// release as verion 1.0 the joystick and menu is wokring
//
// 4/11/2026 Version 1.1
// Updated menu to dipslay 2 sensor status on the LCD at one time 
// using each of the 2 rows
//
// updated to run the LED indicator on when any of the current 5 sensors
// are outside of their operating ranges. 
//
// added RTN STATUS to the scroll menu. This will allow the user to return 
// to the status screen of the UI without the need to wait for the timout
// to return to the status the user will set the scroll menu to RTN STATUS
// then press the joystick button
//
///////////////////////////////////////////////////////////////////////////////


#ifndef FIT_H
#define FIT_H

#include <Arduino.h>



class sensorStatusClass {
public:

    String name;
    String unitStauts;
    String lowStatus;	
    String highStatus;		
    bool   isInt;
    float  currValue;
    float  maxThreshod;
    float  minThreshod;	


  public:
    // Constructor: runs when the object is created
    sensorStatusClass(
         String name,
        String unitStauts,
        String lowStatus,	
        String highStatus,	
        bool   isInt,   
        float  currValue,
        float  maxThreshod,
        float  minThreshod	) {
			
			
       name          = name;
       unitStauts    = unitStauts;
       lowStatus	 = lowStatus;
       highStatus	 = highStatus;	
       isInt         = isInt;
       currValue     = currValue;
       maxThreshod   = maxThreshod;
       minThreshod   = minThreshod;
    }
};




#endif



 // include the library code:
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <DHT.h>

#define DHTPIN 10
#define DHTTYPE DHT22
#define LIGHTSNSORIDX    0
#define TEMPSNSORIDX     1
#define MOISTURESNSORIDX 2
#define HUMIDNSORIDX     3
//#define DEBUG_MODE
DHT dht(DHTPIN, DHTTYPE);

const int JSTICK_UP =700;
const int JSTICK_DOWN =300;
#define DIRECTION_INTERVAL   250 
#define STATUS_INTERVAL      2000
#define WTCH_DOG_SCROLL_MENU 9000


int jStickXPin        = A0;
int jStickYPin        = A1;
int pResistor         = A2; // Photoresistor at Arduino analog pin A2
int moistureSensorPin = A3; // Moisture Sensor at Arduino analog pin A2
int ledIndicatorPin   = 13;
int JStickButtonPin   = 6;

int xVal;                 // variable for storing joystick x values
int yVal;                 // variable for storing joystick y values
int xVal2;                // variable for storing joystick x values
int yVal2;                // variable for storing joystick y values
int buttonState;          // variable for storing joystick push button state
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
//const int rs = 12, en = 11, d4 = 6, d5 = 5, d6 = 4, d7 = 3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


const int WTCH_DOG_CNFG_MENUE_MAX = 10;

volatile bool enterMenu=false;

const int MAX_MENU=8;
const int MIN_MENU=0;

const String menueArray[]= {
  "TEMP MAX",     // 0
  "TEMP MIN",     // 1
  "HUMIDITY MAX", // 2
  "HUMIDITY MIN", // 3    
  "MOISTURE MAX", // 4
  "MOISTURE MIN", // 5
  "LIGHT MAX",    // 6
  "LIGHT MIN",    // 7
  "RTN STATUS"    // 8
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

  unsigned long previousMillisCheckDir = 0;  
  unsigned long previousMillisWaitMenu = 0;    
  unsigned long currentMillis = millis();

// Intialize the custom classes for the sensors data
sensorStatusClass humidSnsrData(
humidSnsrData.name='Humidity',
humidSnsrData.unitStauts="%",
humidSnsrData.lowStatus="to low",
humidSnsrData.highStatus="to high",	
humidSnsrData.isInt=false,
humidSnsrData.currValue=0.0,
humidSnsrData.maxThreshod=10.0,
humidSnsrData.minThreshod=20.0
);


sensorStatusClass tempSnsrData(
tempSnsrData.name="Temp",
tempSnsrData.unitStauts="C",
tempSnsrData.lowStatus="to Low",
tempSnsrData.highStatus="to high",
tempSnsrData.isInt=false,
tempSnsrData.currValue=0.0,
tempSnsrData.maxThreshod=10.0,
tempSnsrData.minThreshod=20.0
);


sensorStatusClass moistureSnsrData(
moistureSnsrData.name="Moisture",
moistureSnsrData.unitStauts="",
moistureSnsrData.lowStatus="to Low",
moistureSnsrData.highStatus="to high",
moistureSnsrData.isInt=true,
moistureSnsrData.currValue=0,
moistureSnsrData.maxThreshod=100,
moistureSnsrData.minThreshod=10
);

sensorStatusClass lightSnsrData(
lightSnsrData.name="Light Intesity",
lightSnsrData.unitStauts="",
lightSnsrData.lowStatus="to Low",
lightSnsrData.highStatus="to high",
lightSnsrData.isInt=true,
lightSnsrData.currValue=0,
lightSnsrData.maxThreshod=1000,
lightSnsrData.minThreshod=500
);

sensorStatusClass snsrStatusArr[]={lightSnsrData,tempSnsrData,moistureSnsrData,humidSnsrData};

//snsrStatusArr[LIGHTSNSORIDX]    =lightSnsrData;
//snsrStatusArr[TEMPSNSORIDX]     =tempSnsrData;
//snsrStatusArr[MOISTURESNSORIDX] =moistureSnsrData;
//snsrStatusArr[HUMIDNSORIDX]     =humidSnsrData;




// Function prototypes
int displayStatus();
int checkControls();
int scrollMenu();
int printSubConfigMenue(int setVal ,int maxVal, int minVal, String menueTitle);
int printMenue(int currMenu, int nextMenu);



///////////////////////////////////////////////////////////////////////////////
// Hardware Setup 
///////////////////////////////////////////////////////////////////////////////
void setup() {  
  pinMode(jStickXPin, INPUT);
  pinMode(jStickYPin, INPUT);
  pinMode(JStickButtonPin,INPUT_PULLUP);  
  pinMode(ledIndicatorPin, OUTPUT);  
  //attachInterrupt(digitalPinToInterrupt(JStickButtonPin), enterMenuIsr, RISING);  
  dht.begin();  
  // set up the LCD's number of columns and rows:  
  lcd.begin(16, 2);
  //pinMode(switchPin, INPUT_PULLUP);
  Serial.begin(9600); // initialize the serial monitor
  digitalWrite(ledIndicatorPin,LOW);
 
  snsrStatusArr[HUMIDNSORIDX].name="Humid";
  snsrStatusArr[HUMIDNSORIDX].unitStauts="%";
  snsrStatusArr[HUMIDNSORIDX].lowStatus="to low";
  snsrStatusArr[HUMIDNSORIDX].highStatus="to high";	
  
  snsrStatusArr[TEMPSNSORIDX].name="Temp";
  snsrStatusArr[TEMPSNSORIDX].unitStauts="C";
  snsrStatusArr[TEMPSNSORIDX].lowStatus="to Low";
  snsrStatusArr[TEMPSNSORIDX].highStatus="to high";
  
  
  
  snsrStatusArr[MOISTURESNSORIDX].name="Mosit";
  snsrStatusArr[MOISTURESNSORIDX].unitStauts="";
  snsrStatusArr[MOISTURESNSORIDX].lowStatus="to Low";
  snsrStatusArr[MOISTURESNSORIDX].highStatus="to high";
  
  snsrStatusArr[LIGHTSNSORIDX].name="Light";
  snsrStatusArr[LIGHTSNSORIDX].unitStauts="";
  snsrStatusArr[LIGHTSNSORIDX].lowStatus="to Low";
  snsrStatusArr[LIGHTSNSORIDX].highStatus="to high"; 

}



///////////////////////////////////////////////////////////////////////////////
// Main Loop
/////////////////////////////////////////////////////////////////////////////// 
void loop() {



 //
 // TODO NEED TO ADD SENSOR DATA COLLECITON
 //
 checkSensors();
 // check for user input and print menue
 displayStatus();


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
  // Light sensor    
  snsrStatusArr[LIGHTSNSORIDX].currValue = analogRead(pResistor);
  // Temp Sensor
  snsrStatusArr[TEMPSNSORIDX].currValue = dht.readTemperature();  
  // Moisture Sensor   
  //int tmpRd = analogRead(moistureSensorPin);
  //snsrStatusArr[MOISTURESNSORIDX].currValue = map(tmpRd, 0, 1023, 255, 0);
  snsrStatusArr[MOISTURESNSORIDX].currValue = analogRead(moistureSensorPin);
  
  // Humidity Sensor
  snsrStatusArr[HUMIDNSORIDX].currValue = dht.readHumidity();


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



   String statusStringA="";    
   String statusStringB="";       



    String name;
    String unitStauts;
    String lowStatus;	
    String highStatus;		
    bool   isInt;
    bool   changeStatus=false;	
	bool   alrmStatus=false;
    float  currValue;
    float  maxThreshod;
    float  minThreshod;	
    int currBTN = 0;
    int prvBTN = 0;

    // Loop through the sensor values
	  // set alarm status if the sensor reading is
	  // outside of the threshold
        for (int i=0; i<4; i++) {       
          if(
		     (snsrStatusArr[i].currValue  > snsrStatusArr[i].maxThreshod) || 
		     (snsrStatusArr[i].currValue  < snsrStatusArr[i].minThreshod)
			 ) {
             alrmStatus = true;
          }
        }






         
	    if(alrmStatus){
           digitalWrite(ledIndicatorPin,HIGH);      
	    } else {
           digitalWrite(ledIndicatorPin,LOW);      		 
        }
 
      for (int i=0; i<4; i=i+2) {
	   	if(snsrStatusArr[i].isInt){
          statusStringA=snsrStatusArr[i].name+":"+int(snsrStatusArr[i].currValue);
        } else {
          statusStringA=snsrStatusArr[i].name+":"+snsrStatusArr[i].currValue;
        }
        statusStringA=statusStringA+" "+snsrStatusArr[i].unitStauts;

        if((snsrStatusArr[i].currValue  > snsrStatusArr[i].maxThreshod) ) {
            // Over Max
		    statusStringA="!"+statusStringA;
        } else if((snsrStatusArr[i].currValue  < snsrStatusArr[i].minThreshod)) {
           // Under Min 
		   statusStringA="!"+statusStringA;
        } else {
           // Nominal
		   statusStringA=" "+statusStringA;		  
        }

	   	if(snsrStatusArr[i+1].isInt){
          statusStringB=snsrStatusArr[i+1].name+":"+int(snsrStatusArr[i+1].currValue);
        } else {
          statusStringB=snsrStatusArr[i+1].name+":"+snsrStatusArr[i+1].currValue;
        }
        statusStringB=statusStringB+" "+snsrStatusArr[i+1].unitStauts;


        if((snsrStatusArr[i+1].currValue  > snsrStatusArr[i+1].maxThreshod) ) {
            // Over Max
		    statusStringB="!"+statusStringB;
        } else if((snsrStatusArr[i+1].currValue  < snsrStatusArr[i+1].minThreshod)) {
           // Under Min 
		   statusStringB="!"+statusStringB;
        } else {
           // Nominal
		   statusStringB=" "+statusStringB;		  
        }

        lcd.setCursor(0, 0);
        lcd.print("                ");   // This is done to 0 out the 2nd row
        lcd.setCursor(0, 0);        
        lcd.print(statusStringA);
        lcd.setCursor(0, 1);        
        lcd.print("                ");   // This is done to 0 out the 2nd row
        lcd.setCursor(0, 1);     
        lcd.print(statusStringB); 



   
        changeStatus  = false;	
        currentMillis = millis();
        previousMillisWaitMenu=currentMillis;
        while(changeStatus==false){  
         currBTN = 1;
         prvBTN = 1;
         currBTN=digitalRead(JStickButtonPin);
		 currentMillis = millis();			
         if (currentMillis - previousMillisWaitMenu >= STATUS_INTERVAL) {
           previousMillisWaitMenu = currentMillis;  // Remember the time         
           changeStatus = !changeStatus;            // Toggle        
         }	
	 	 
         if(prvBTN!=currBTN){
          Serial.print("displayStatus: Button Pressed! "); 
          scrollMenu();
          break;			
		 }
         prvBTN=currBTN;
        }   
	}	
    return 1;
}
//displayStatus


///////////////////////////////////////////////////////////////////////////////
// Function Name : scrollMenu
// Inputs        : None
// Returns       : Nothing
//
// Description : 
//               
///////////////////////////////////////////////////////////////////////////////
int scrollMenu(){
  // reset the watch dog timer to 0 when we enter the scroll menue
  // when the watch dog reaches the max we will go back to the status 
  // display function. 
    

   int  currMenu=0; 
   int  prevMenu=0; 
   int  nextMenu=0;  
   bool menueChange = false; 
   bool enterSubMenue = false;   
   bool checkDir = false;
   bool watchDogExp=false;
   int  currBTN = 1;
   int  prvBTN = 1;


   currentMillis=millis();
   previousMillisWaitMenu=currentMillis;
   previousMillisCheckDir=currentMillis;
   printMenue(prevMenu,0);    

  currBTN=digitalRead(JStickButtonPin);
  prvBTN=1;
  while((prvBTN!=currBTN) && (currBTN !=1)){  
    Serial.print("scrollMenu: DBOUNCE \n");    
    currBTN=digitalRead(JStickButtonPin);  
  }
  while(watchDogExp==false){  
    currentMillis=millis();
    menueChange=false;	
    if ((currentMillis - previousMillisWaitMenu) >= WTCH_DOG_SCROLL_MENU) {
      previousMillisWaitMenu = currentMillis;  // Remember the time         
      watchDogExp = !watchDogExp;            // Toggle        
    }	  

    if (currentMillis - previousMillisCheckDir >= DIRECTION_INTERVAL) {
      previousMillisCheckDir = currentMillis;  // Remember the time
      checkDir = !checkDir;            // Toggle
    }
  
   currBTN=digitalRead(JStickButtonPin);
    // read the x, y and joystick switch values
  
    xVal = analogRead(jStickXPin);
    yVal = analogRead(jStickYPin);    
    /* 
    Serial.print("scrollMenu: xVal ");  
    Serial.print(xVal);  
    Serial.print(" | yVal ");  	
    Serial.print(yVal);  	
    Serial.print(" | currBTN ");  		
    Serial.print(currBTN);	
    Serial.print(" | prvBTN ");  		
    Serial.print(prvBTN);		
    Serial.print(" | currentMillis ");  		
    Serial.print(currentMillis);		
    Serial.print(" | previousMillisCheckDir ");  
    Serial.print(previousMillisCheckDir);		
    Serial.print(" | previousMillisWaitMenu ");  
    Serial.print(previousMillisWaitMenu);		
    Serial.print(" | checkDir ");  		
    Serial.print(checkDir);	
    Serial.print(" | menueChange ");  		
    Serial.print(menueChange);	
    Serial.print(" | prevMenu ");  		
    Serial.print(prevMenu);	
    Serial.print(" | currMenu ");  		
    Serial.print(currMenu);		
    Serial.print("\n");  		
*/

    //  LIGHT MAX     // 0
    //  LIGHT MIN     // 1
    //  Temp MAX      // 2
    //  Temp MIN      // 3
    //  MOISTURE MAX  // 4
    //  MOISTURE MIN  // 4
    //  HUMIDITY MAX  // 5
    //  HUMIDITY MIN  // 6
    //  RTN STATUS    // 7	
 
    if((yVal > JSTICK_UP) && checkDir) { // UP
      nextMenu=1;
      menueChange=true; 
      checkDir=false;
      Serial.print("UP\n"); 	  
	  previousMillisCheckDir=millis();	  
    } else if((yVal < JSTICK_DOWN) && checkDir) { // DOWN
      nextMenu=2; 
      menueChange=true;   
      checkDir=false;
      Serial.print("DOWN\n"); 
	  previousMillisCheckDir=millis();
    }else if(prvBTN!=currBTN){
      Serial.print("scrollMenu: Button Pressed! \n"); 
      Serial.print("scrollMenu: "); 	  
      Serial.print(" | prevMenu ");  		
      Serial.print(prevMenu);	
      Serial.print(" | currMenu ");  		
      Serial.print(currMenu);			
      Serial.print("\n"); 	 
      while((prvBTN!=currBTN) && (currBTN !=1)){  
        Serial.print("scrollMenu: DBOUNCE \n");    
        currBTN=digitalRead(JStickButtonPin);  
      }	 	  
      if(prevMenu==MAX_MENU){	  
		  break;
      } else {		  
        Serial.print("scrollMenu: Button Pressed! "); 
        //int printSubConfigMenue(int setVal ,int maxVal, int minVal, String menueTitle){
        limitThrsArray[prevMenu]=printSubConfigMenue(
                                 limitThrsArray[prevMenu],
                                 SENSOR_MAX_SETTING_ARRAY[prevMenu],
                                 0,
                                 menueArray[prevMenu]
                                );    
	  						  
        // Go to the sub menue to configure the thresholds. 
	    //int printMenue(int currMenu, int nextMenu) {
	    printMenue(prevMenu,0); 
	    enterMenu=false;
        previousMillisWaitMenu=millis();	  
      }
    }	
	
	if(menueChange == true){
      prevMenu=currMenu;
      currMenu=printMenue(currMenu,nextMenu);  
      previousMillisWaitMenu=millis();

    };
    prvBTN=currBTN;
  }
  return;
  // when we leave this function and go back to the status
  // display function we will reatach the interrutp
  //attachInterrupt(digitalPinToInterrupt(JStickButtonPin), scrollMenu, FALLING);  
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
    #ifdef DEBUG_MODE     
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
    #endif;
    lcd.clear();      
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print(menueTitle);
    lcd.setCursor(0, 1);
    lcd.print(setVal);      
    delay(200);    
    while(localButtonState==1){
       localButtonState = digitalRead(JStickButtonPin);

        xVal = analogRead(jStickXPin);
        yVal = analogRead(jStickYPin);
        if(yVal > 800) { // UP
          localWatchDog=0;
          if(setVal<maxVal){
            #ifdef DEBUG_MODE
            Serial.print("printSubConfigMenue: setVal ");  
            Serial.print(setVal);
            Serial.print("\n"); 
            #endif;
            setVal=setVal+5;        
          }
        } else if(yVal < 100) { // DOWN
          localWatchDog=0;        
          if(setVal>minVal){
            #ifdef DEBUG_MODE
            Serial.print("printSubConfigMenue: setVal ");  
            Serial.print(setVal);
            Serial.print("\n"); 
            #endif;
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
        #ifdef DEBUG_MODE        
        Serial.print("printSubConfigMenue: localWatchDog ");  
        Serial.print(localWatchDog);
        Serial.print("\n");             
        #endif;
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


if(arryNextIndex==(MAX_MENU+1)){
  arryNextIndex=0; 
}
    Serial.print("printMenue: currMenu ");  
    Serial.print(currMenu);  
    Serial.print(" | nextMenu ");  	
    Serial.print(nextMenu);  
    Serial.print(" | localCurrMenu ");  	
    Serial.print(localCurrMenu);  
    Serial.print(" | arryNextIndex ");  	
    Serial.print(arryNextIndex);  	
    Serial.print("\n");
	
    lcd.clear();
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
    if(nextMenu == 1 ) { // Menue scroll up
      // If we are on the first menue option and we scroll up
      // We will need to wrap back the last menue 
      // other wise we will go up one to the previous menue option 
      if(localCurrMenu ==  MIN_MENU){
        localCurrMenu=MAX_MENU;
      } else {
        localCurrMenu=localCurrMenu-1;
      }   
    } else if (nextMenu == 2) { // Menue scroll down 
      // If we are on the last menue option and we scroll down
      // We will need to wrap back the first menue 
      // other wise we will go up one to the next menue option 
      if(localCurrMenu ==  MAX_MENU){
        localCurrMenu=0;
      } else {
        localCurrMenu=localCurrMenu+1;
      }      
    }

  return localCurrMenu;
}



//void enterMenuIsr() {
//
//   enterMenu=true;   
//
//}