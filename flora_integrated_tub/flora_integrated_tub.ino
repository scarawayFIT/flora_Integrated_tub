///////////////////////////////////////////////////////////////////////////////
// Title: Flora Integrated Tub
// Revishion: 1.2
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
// 4/16/2026 Version 1.1
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
//
// 4/17/2026 Version 1.2
// 
// Replaced many constants with #define
//
// Fixed bug that menu would not scroll to the next menu selected when the scroll 
// menu was first entered.
//
// Added Sensor min/max values and min/max Threshold values to the sensor classes.
// Now the min/max possible values for each sensor can be set in the classes during 
// initiation. This will prevent the user from setting a threshold outside 
// the min and max while in the configuration menu
//
// The sensor min/max threshold values can be used to set the default 
// threshold values for each sensor. When the user updates the threshold values 
// on the configuration menu these values will be updated. 
//
// refactored the printSubConfigMenu function 
//
//   updated the functions watch dog timer based on the 
//   millis() function rather than delay. 
//
//   Updated the way the function displays the values to prevent flickering.
//
//   Added debounce logic to the push button so the menu is not randomly 
//   entered and exited when the button is pressed during the scroll menu. 
//
// Known defect : There is a known defect when exiting a config menu by 
// pressing the bush button the user will be take back to the scroll menu 
// in a different location than where was entered.  
//
// 4/17/2026 Version 1.3
// 
// fixed defect where when exiting a config menu by 
// pressing the bush button the user will be taken back to the scroll menu 
// in a different location than where it was entered.  
//
// 4/17/2026 Version 1.4
//
// Updated the status display to use a check mark next to status to represetn
// the status is within spec and a bell to show when a status is out of spec
//
// 4/25/2026 Version 1.5
//
// Removed unused variables
//
// Added comments to class declarations of sensors to make it more clear what
// each attribute does and is used for
// updated default values in classes. 
//
// updated menu to use the left on the joystick as a way to return to 
// previous menus. When in the sub configuration menu setting the max or min values
// The user can now press left on the joystick to return to the 
// scroll menu. When in the Scroll menu pressing left of the joystick will now 
// return to the status menu
//
// Updated config menu to display the units of the sensors
// Updated config menu to display the sensor value in either Float Or Integrated
// Updated config menu to change floating point values by 0.1
//
///////////////////////////////////////////////////////////////////////////////


#ifndef FIT_H
#define FIT_H

#include <Arduino.h>



class sensorStatusClass {
public:

    String name;
    String unitStauts;
    bool   isInt;
    float  currValue;
    float  maxThreshod;
    float  minThreshod;	
    float  sensorMax;
    float  sensorMin;	


  public:
    // Constructor: runs when the object is created
    sensorStatusClass(
        String name,
        String unitStauts,
        bool   isInt,   
        float  currValue,
        float  maxThreshod,
        float  minThreshod,
        float  sensorMax,
        float  sensorMin) 
		{

       name          = name;
       unitStauts    = unitStauts;
       isInt         = isInt;
       currValue     = currValue;
       maxThreshod   = maxThreshod;
       minThreshod   = minThreshod;
	   sensorMax     = sensorMax;
	   sensorMin     = sensorMin;
    }
};

#endif



 // include the library code:
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <DHT.h>

#define DHTPIN               10
#define DHTTYPE DHT22
#define LIGHTSNSORIDX        0
#define TEMPSNSORIDX         1
#define MOISTURESNSORIDX     2
#define HUMIDNSORIDX         3
#define DIRECTION_INTERVAL   250 
#define STATUS_INTERVAL      2000
#define DIRECTION_INTERVAL   250 
#define STATUS_INTERVAL      2000
#define WTCH_DOG_SCROLL_MENU 9000
#define WTCH_DOG_CNFG_MENUE  3000
#define JSTICK_UP            700
#define JSTICK_DOWN          300
#define JSTICK_LEFT          200
#define MAX_MENU             8
#define MIN_MENU             0

//#define DEBUG_MODE

DHT dht(DHTPIN, DHTTYPE);

byte Bell[8] = {
0b00100,
0b01110,
0b01110,
0b01110,
0b11111,
0b00000,
0b00100,
0b00000
};


byte Check[8] = {
0b00000,
0b00001,
0b00011,
0b10110,
0b11100,
0b01000,
0b00000,
0b00000
};


int jStickXPin        = A0;
int jStickYPin        = A1;
int pResistor         = A2; 
int moistureSensorPin = A3; 
int ledIndicatorPin   = 13;
int JStickButtonPin   = 6;

int xVal;                 // variable for storing joystick x values
int yVal;                 // variable for storing joystick y values
int buttonState;          // variable for storing joystick push button state


const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

unsigned long previousMillisCheckDir = 0;  
unsigned long previousMillisWaitMenu = 0;    
unsigned long currentMillis = millis();

///////////////////////////////////////////////////////////////////////////////
// Intialize the custom classes for the sensors data
// Use these to configure the intial values of the sensors 
//
///////////////////////////////////////////////////////////////////////////////
// Humidity Sensor 
sensorStatusClass humidSnsrData(
humidSnsrData.name="Humidity",  // String dipslayed in the staus menu
humidSnsrData.unitStauts="%",   // Character to be displayed after the Status
humidSnsrData.isInt=false,      // Used to interprit how the data should be displayed Is this an INT or a FLOAT
humidSnsrData.currValue=0.0,    // Sets the current intial value
humidSnsrData.maxThreshod=75.0, // Set the threshold max of the Humidity sensor
humidSnsrData.minThreshod=30.0, // Set the threshold min of the Humidity sensor
humidSnsrData.sensorMax=90.0,   // Set the limit the user can set the min threshold of the Humiditty Sensor
humidSnsrData.sensorMin=10      // Set the limit the user can set the max threshold of the Humiditty Sensor	
);

// Temperature Sensor
sensorStatusClass tempSnsrData(
tempSnsrData.name="Temp",      
tempSnsrData.unitStauts="C",   // Character to be displayed after the Status
tempSnsrData.isInt=false,      // Used to interprit how the data should be displayed Is this an INT or a FLOAT
tempSnsrData.currValue=0.0,    // Sets the current intial value
tempSnsrData.maxThreshod=27.7, // 82F  | Set the threshold max of the Temperature sensor
tempSnsrData.minThreshod=7.2,  // 45F  | Set the threshold min of the Temperature sensor
tempSnsrData.sensorMax=38,     // 104F | Set the limit the user can set the min threshold of the Temperature Sensor
tempSnsrData.sensorMin=0       // 32F  |Set the limit the user can set the max threshold of the Temperature Sensor	 
);

// Moisture Sensor
sensorStatusClass moistureSnsrData(
moistureSnsrData.name="Moisture", // String dipslayed in the staus menu
moistureSnsrData.unitStauts="",   // Character to be displayed after the Status
moistureSnsrData.isInt=true,      // Used to interprit how the data should be displayed Is this an INT or a FLOAT
moistureSnsrData.currValue=0,     // Sets the current intial value
moistureSnsrData.maxThreshod=100, // Set the threshold max of the Moisture sensor
moistureSnsrData.minThreshod=10,  // Set the threshold min of the Moisture sensor
moistureSnsrData.sensorMax=1000,  // Set the limit the user can set the min threshold of the Moisture Sensor
moistureSnsrData.sensorMin=0	  // Set the limit the user can set the max threshold of the Moisture Sensor	
);

// Light Sensor
sensorStatusClass lightSnsrData(
lightSnsrData.name="Light Intesity", // String dipslayed in the staus menu
lightSnsrData.unitStauts="",         // Character to be displayed after the Status
lightSnsrData.isInt=true,            // Used to interprit how the data should be displayed Is this an INT or a FLOAT
lightSnsrData.currValue=0,           // Sets the current intial value
lightSnsrData.maxThreshod=100,       // Set the threshold max of the Light sensor
lightSnsrData.minThreshod=700,       // Set the threshold min of the Light sensor
lightSnsrData.sensorMax=1000,        // Set the limit the user can set the min threshold of the Light Sensor
lightSnsrData.sensorMin=0	         // Set the limit the user can set the max threshold of the Light Sensor	
);


//#define LIGHTSNSORIDX        0
//#define TEMPSNSORIDX         1
//#define MOISTURESNSORIDX     2
//#define HUMIDNSORIDX         3

// |-----------------------|-----------------|
// |idx| Class idx const   | Sensor          |
// |---|-------------------|-----------------|
// | 0 | LIGHTSNSORIDX     | light senesor   |
// | 1 | TEMPSNSORIDX      | Temp sensor     |
// | 2 | MOISTURESNSORIDX  | moisture snesor |
// | 3 | HUMIDNSORIDX      | humidity sensor |
// ------------------------|-----------------|
sensorStatusClass snsrStatusArr[]={lightSnsrData,tempSnsrData,moistureSnsrData,humidSnsrData};

// This array is used to set the string that will be 
// displaued in the scroll menu
const String MENU_ARRAY[]= {
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

// |---------------------------------------|
// | Menu          |idx| Sensor Setting    |
// | --------------|---|-------------------|
// | LIGHT MAX     | 0 | LIGHTSNSORIDX     |
// | LIGHT MIN     | 1 | LIGHTSNSORIDX     |
// | Temp MAX      | 2 | TEMPSNSORIDX      |
// | Temp MIN      | 3 | TEMPSNSORIDX      |
// | MOISTURE MAX  | 4 | MOISTURESNSORIDX  |
// | MOISTURE MIN  | 5 | MOISTURESNSORIDX  |
// | HUMIDITY MAX  | 6 | HUMIDNSORIDX      |
// | HUMIDITY MIN  | 7 | HUMIDNSORIDX      |
// | RTN STATUS    | 8 | N/A               |
// |---------------------------------------|

const int menuLookUpTable[]={
TEMPSNSORIDX,
TEMPSNSORIDX,
HUMIDNSORIDX,
HUMIDNSORIDX,
MOISTURESNSORIDX,
MOISTURESNSORIDX,
LIGHTSNSORIDX,
LIGHTSNSORIDX
};




// Function prototypes
int displayStatus();
int checkControls();
int scrollMenu();
int printSubConfigMen(int setVal ,int maxVal, int minVal, String menueTitle, bool isInt, String unitStauts );
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
 
 
  // Strings were not saving from the initalization of these classes
  // Moved them here and it seemed to work 
  snsrStatusArr[HUMIDNSORIDX].name="Humid";
  snsrStatusArr[HUMIDNSORIDX].unitStauts="%";
    
  snsrStatusArr[TEMPSNSORIDX].name="Temp";
  snsrStatusArr[TEMPSNSORIDX].unitStauts="C";
  
  snsrStatusArr[MOISTURESNSORIDX].name="Moist";
  snsrStatusArr[MOISTURESNSORIDX].unitStauts="";
  
  snsrStatusArr[LIGHTSNSORIDX].name="Light";
  snsrStatusArr[LIGHTSNSORIDX].unitStauts="";
  // create a new character
  // These are characters that will be used
  // to comunicate status of the sensors 
  lcd.createChar(0, Check); // GOOD
  lcd.createChar(1, Bell);  // BAD
}



///////////////////////////////////////////////////////////////////////////////
// Main Loop
/////////////////////////////////////////////////////////////////////////////// 
void loop() {
	
 // Collects sensor data
 checkSensors();

 // Display the status 
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



    //String name;
    //String unitStauts;
    //String lowStatus;	
    //String highStatus;		
    bool   isInt;
    bool   changeStatus=false;	
	bool   alrmStatus=false;
    float  currValue;
    float  maxThreshod;
    float  minThreshod;	
    int    currBTN = 0;
    int    prvBTN = 0;
	int    statusidxA=0;
	int    statusidxB=0;	

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
            statusidxA=1;			
        } else if((snsrStatusArr[i].currValue  < snsrStatusArr[i].minThreshod)) {			
           // Under Min 
           statusidxA=1;
        } else {
           // Nominal
           statusidxA=0;
        }


	
	   	if(snsrStatusArr[i+1].isInt){
          statusStringB=snsrStatusArr[i+1].name+":"+int(snsrStatusArr[i+1].currValue);
        } else {
          statusStringB=snsrStatusArr[i+1].name+":"+snsrStatusArr[i+1].currValue;
        }
        statusStringB=statusStringB+" "+snsrStatusArr[i+1].unitStauts;


        if((snsrStatusArr[i+1].currValue  > snsrStatusArr[i+1].maxThreshod) ) {
            // Over Max
           statusidxB=1;			
        } else if((snsrStatusArr[i+1].currValue  < snsrStatusArr[i+1].minThreshod)) {
           // Under Min 
           statusidxB=1;		   
        } else {
           // Nominal
           statusidxB=0;  
        }

        #ifdef DEBUG_MODE	  		
          Serial.print("snsrStatusArr[i].currValue ");  
          Serial.print(snsrStatusArr[i].currValue);
          Serial.print(" | snsrStatusArr[i].maxThreshod ");  
          Serial.print(snsrStatusArr[i].maxThreshod);  
          Serial.print(" | snsrStatusArr[i].minThreshod ");  		
          Serial.print(snsrStatusArr[i].minThreshod);        
          Serial.print(statusidxA);  
        
          Serial.print("\n");  		
        #endif
		
        lcd.setCursor(0, 0);
        lcd.print("                ");   // This is done to 0 out the 2nd row
        lcd.setCursor(0, 0);        
        lcd.write(byte(statusidxA));
        lcd.setCursor(1, 0);         
        lcd.print(statusStringA);        
		
        lcd.setCursor(0, 1);        
        lcd.print("                ");   // This is done to 0 out the 2nd row
        lcd.setCursor(0, 1);     
        lcd.write(byte(statusidxB)); 
        lcd.setCursor(1,1);         
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
    

   int   currMenu      = 0; 
   int   prevMenu      = 0; 
   int   nextMenu      = 0;  
   bool  menueChange   = false; 
   bool  enterSubMenue = false;   
   bool  checkDir      = false;
   bool  watchDogExp   = false;
   int   currBTN       = 1;
   int   prvBTN        = 1;
   int   tmpIdx        = 0;
   float tmpSetVal     = 0;

   currentMillis=millis();
   previousMillisWaitMenu=currentMillis;
   previousMillisCheckDir=currentMillis;
   printMenue(prevMenu,0);    

  currBTN=digitalRead(JStickButtonPin);
  prvBTN=1;
  while((prvBTN!=currBTN) && (currBTN !=1)){  
    #ifdef DEBUG_MODE	    
	   Serial.print("scrollMenu: DBOUNCE \n");    
    #endif	
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
    #ifdef DEBUG_MODE	  		
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
    #endif
 
    if((yVal > JSTICK_UP) && checkDir) { // UP
      nextMenu=1;
      menueChange=true; 
      checkDir=false;
      Serial.print("scrollMenu: UP\n"); 	  
	  previousMillisCheckDir=millis();	  
    } else if((yVal < JSTICK_DOWN) && checkDir) { // DOWN
      nextMenu=2; 
      menueChange=true;   
      checkDir=false;
      #ifdef DEBUG_MODE	  		
         Serial.print("scrollMenu: DOWN\n"); 
      #endif
	  previousMillisCheckDir=millis();
    } else if((xVal < JSTICK_LEFT) && (yVal < JSTICK_UP && yVal > JSTICK_DOWN) && checkDir) { // Left
      menueChange=true;   
      checkDir=false;	  
      //#ifdef DEBUG_MODE	  		
         Serial.print("scrollMenu: LEFT\n"); 
         Serial.print("scrollMenu: return to status\n");   
      //#endif
      break;	  
    }else if(prvBTN!=currBTN){
       #ifdef DEBUG_MODE	  		
          Serial.print("scrollMenu: Button Pressed! \n"); 
          Serial.print("scrollMenu: "); 	  
          Serial.print(" | prevMenu ");  		
          Serial.print(prevMenu);	
          Serial.print(" | currMenu ");  		
          Serial.print(currMenu);			
          Serial.print("\n");
       #endif
	  // Digital debounce button press
      while((prvBTN!=currBTN) && (currBTN !=1)){  
        Serial.print("scrollMenu: DBOUNCE \n");    
        currBTN=digitalRead(JStickButtonPin);  
      }	 
	  
      if(currMenu==MAX_MENU){	  
          #ifdef DEBUG_MODE	  
            Serial.print("scrollMenu: return to status\n");   
		  #endif
		  break;
      } else {		  
        #ifdef DEBUG_MODE	  
          Serial.print("scrollMenu: Enter printSubConfigMen\n ");
		#endif
        // determine if this is a min or max threshold menu
        tmpIdx=menuLookUpTable[currMenu];
		if((currMenu % 2)==0){
		  tmpSetVal=snsrStatusArr[tmpIdx].maxThreshod;
		} else {
		  tmpSetVal=snsrStatusArr[tmpIdx].minThreshod;
		}			

        #ifdef DEBUG_MODE
          Serial.print("currMenu: ");
          Serial.print(" ");
	      Serial.print(currMenu);		  
          Serial.print(" ");		  
	      Serial.print("menuLookUpTable[currMenu]: ");
          Serial.print(" ");		  		  
          Serial.print(menuLookUpTable[currMenu]);			  
          Serial.print(" ");		  		  
          Serial.print("tmpSetVal");
          Serial.print(" ");		  		  
          Serial.print(tmpSetVal);
          Serial.print(" ");		  		  		  
          Serial.print("\n");		  		  
	      Serial.print("menuLookUpTable: ");
          Serial.print(menuLookUpTable[0]);	
          Serial.print(" ");
          Serial.print(menuLookUpTable[1]);	
          Serial.print(" ");
          Serial.print(menuLookUpTable[2]);
          Serial.print(" ");
          Serial.print(menuLookUpTable[3]);
          Serial.print(" ");
          Serial.print(menuLookUpTable[4]);		  
          Serial.print(" ");
          Serial.print(menuLookUpTable[5]);
          Serial.print(" ");
          Serial.print(menuLookUpTable[6]);		  
          Serial.print("\n");	
	      Serial.print("tmpIdx: ");
          Serial.print(tmpIdx);
          Serial.print(" | ");		  
	      Serial.print("name: ");
          Serial.print(snsrStatusArr[tmpIdx].name);
          Serial.print(" | ");
          Serial.print("unitStauts: ");
          Serial.print(snsrStatusArr[tmpIdx].unitStauts);
          Serial.print(" | ");
          Serial.print("isInt: ");
          Serial.print(snsrStatusArr[tmpIdx].isInt);
          Serial.print(" | ");
          Serial.print("currValue: ");
          Serial.print(snsrStatusArr[tmpIdx].currValue);
          Serial.print(" | ");
          Serial.print("maxThreshod: ");
          Serial.print(snsrStatusArr[tmpIdx].maxThreshod);
          Serial.print(" | ");
          Serial.print("minThreshod: ");
          Serial.print(snsrStatusArr[tmpIdx].minThreshod);
          Serial.print(" | ");
          Serial.print("sensorMax: ");
          Serial.print(snsrStatusArr[tmpIdx].sensorMax);
          Serial.print(" | ");
          Serial.print("sensorMin: ");
          Serial.print(snsrStatusArr[tmpIdx].sensorMin);
          Serial.print("\n");
	    #endif
	
        //int printSubConfigMen(int setVal ,int maxVal, int minVal, String menueTitle){        
		tmpSetVal=printSubConfigMen(tmpSetVal,
                                      snsrStatusArr[tmpIdx].sensorMax,
                                      snsrStatusArr[tmpIdx].sensorMin,
                                      MENU_ARRAY[currMenu],
									  snsrStatusArr[tmpIdx].isInt,
									  snsrStatusArr[tmpIdx].unitStauts
                                      );   

        // determine if this is a min or max threshold menu
		// then save the updated value to the apropiate class
		if((currMenu % 2)==0){
		  snsrStatusArr[tmpIdx].maxThreshod=tmpSetVal;
		} else {
		  snsrStatusArr[tmpIdx].minThreshod=tmpSetVal;
		}	


	  						  
        // Go to the sub menue to configure the thresholds. 
	    //int printMenue(int currMenu, int nextMenu) {
	    printMenue(currMenu,0); 
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
}

///////////////////////////////////////////////////////////////////////////////
// Function Name : printSubConfigMen
//
// Inputs int    setVal     : The current value
//        int    maxVal     : The max allowed value
//        int    minVal     : The min allowed value
//        String menueTitle : The string to be displayed
//        bool   isInt      : determine if this is an int or a float
//        string unitStauts : display chacter to show the units of the value
//
// Returns : int setVal : the new value to be set
//
// Description : 
//               
///////////////////////////////////////////////////////////////////////////////
int printSubConfigMen(float setVal ,int maxVal, int minVal, 
                      String menueTitle, bool isInt , String unitStauts){

    int   localWatchDog =0;
    bool  exitMenu      = false;
	bool  checkDir      = false;
    int   currBTN       = 1;
    int   prvBTN        = 1;	
	float incDecAmnt    = 1;
    String valueString =""; 
	if(isInt == true){
      incDecAmnt = 1;
	} else {
      incDecAmnt = 0.1;
	}

    #ifdef DEBUG_MODE     
      Serial.print("printSubConfigMen: menueTitle ");  
      Serial.print(menueTitle);
      Serial.print("\n"); 
      Serial.print("printSubConfigMen: setVal ");  
      Serial.print(setVal);
      Serial.print("\n"); 
      Serial.print("printSubConfigMen: maxVal ");  
      Serial.print(maxVal);
      Serial.print("\n"); 
      Serial.print("printSubConfigMen: minVal ");  
      Serial.print(maxVal);
      Serial.print("\n");     
    #endif;
    if(isInt == true){
    	valueString=int(setVal)+unitStauts;
    } else {
    	valueString=setVal+unitStauts;
    }	
	
    lcd.clear();      
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print(menueTitle);
    lcd.setCursor(0, 1);
    lcd.print(valueString);      	


    currentMillis = millis();
    previousMillisWaitMenu=currentMillis;
    previousMillisCheckDir=currentMillis;	
     while(exitMenu==false){
        currBTN=digitalRead(JStickButtonPin);
        prvBTN=1;
	    // Digital debounce button press
        while((prvBTN!=currBTN) && (currBTN !=1)){  
          currBTN=digitalRead(JStickButtonPin);  
		  exitMenu=true;
        }	 

        if (currentMillis - previousMillisCheckDir >= DIRECTION_INTERVAL) {
          previousMillisCheckDir = currentMillis;  // Remember the time
          checkDir = !checkDir;                    // Toggle
        }
		
        prvBTN  =currBTN;		
        xVal    = analogRead(jStickXPin);
        yVal    = analogRead(jStickYPin);
	
        if(yVal > JSTICK_UP) { // UP
          localWatchDog=0;
          if(setVal<maxVal){
            #ifdef DEBUG_MODE
              Serial.print("printSubConfigMen: setVal ");  
              Serial.print(setVal);
              Serial.print("\n"); 
            #endif;
            setVal=setVal+incDecAmnt;
            previousMillisWaitMenu= millis();  // Reset watch dog timer			
			if(isInt == true){
				valueString=int(setVal)+unitStauts;
	        } else {
				valueString=setVal+unitStauts;
	        }

            lcd.setCursor(0, 0);
            lcd.print("                ");   // This is done to 0 out the 2nd row   
            lcd.setCursor(0, 0);
            lcd.print(menueTitle);
            lcd.setCursor(0, 1);
            lcd.print("                ");   // This is done to 0 out the 2nd row   
            lcd.setCursor(0, 1);      
            lcd.print(valueString);      
          }
        } else if(yVal < JSTICK_DOWN) { // DOWN
          localWatchDog=0;        
          if(setVal>minVal){
            #ifdef DEBUG_MODE
              Serial.print("printSubConfigMen: setVal ");  
              Serial.print(setVal);
              Serial.print("\n"); 
            #endif;
            setVal=setVal-incDecAmnt;      
            previousMillisWaitMenu= millis(); // Reset watch dog timer			
			if(isInt == true){
				valueString=int(setVal)+unitStauts;
	        } else {
				valueString=setVal+unitStauts;
	        }			
            lcd.setCursor(0, 0);
            lcd.print("                ");   // This is done to 0 out the 2nd row   
            lcd.setCursor(0, 0);
            lcd.print(menueTitle);
            lcd.setCursor(0, 1);
            lcd.print("                ");   // This is done to 0 out the 2nd row   
            lcd.setCursor(0, 1);      
            lcd.print(valueString);    				
          }
        } else if((xVal < JSTICK_LEFT) && (yVal < JSTICK_UP && yVal > JSTICK_DOWN) && checkDir) { // Left
          checkDir=false;	  
	      previousMillisCheckDir=millis();			  
          //#ifdef DEBUG_MODE	  		
             Serial.print("printSubConfigMen: LEFT\n"); 
             Serial.print("printSubConfigMen: return to status\n");   
          //#endif
		  exitMenu=true;
		}  

        currentMillis = millis();		
         if (currentMillis - previousMillisWaitMenu >= WTCH_DOG_CNFG_MENUE) {
           previousMillisWaitMenu = currentMillis;  // Remember the time   
           exitMenu  = true;
         }	
	 	 delay(100); // put this here to prevent screen flickering

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
int arryNextIndex=0;


    #ifdef DEBUG_MODE
      Serial.print("printMenue: currMenu ");  
      Serial.print(currMenu);  
      Serial.print(" | nextMenu ");  	
      Serial.print(nextMenu);  
      Serial.print(" | localCurrMenu ");  	
      Serial.print(localCurrMenu);  
      Serial.print(" | arryNextIndex ");  	
      Serial.print(arryNextIndex);  	
      Serial.print("\n");
    #endif

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
      if(localCurrMenu ==  (MAX_MENU)){
        localCurrMenu=0;
      } else {
        localCurrMenu=localCurrMenu+1;
      }      
    }

    if(localCurrMenu==(MAX_MENU)){
      arryNextIndex=0; 
    } else {
       arryNextIndex=localCurrMenu+1;
    }	

    // This was 
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("                ");   // This is done to 0 out the row       
    lcd.setCursor(0, 0);    
    lcd.print(">"+MENU_ARRAY[localCurrMenu]);
    lcd.setCursor(0, 1);
    lcd.print("                ");   // This is done to 0 out the row           
    lcd.setCursor(0, 1);
    lcd.print(" "+MENU_ARRAY[arryNextIndex]);

  return localCurrMenu;
}

