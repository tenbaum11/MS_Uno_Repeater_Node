#include <SPI.h>
#include <MySensor.h>  
#include <RHDatagram.h>
#include <RH_ASK.h>
#include <RCSwitch.h>


#define CID_LIGHT 0
#define CID_DOGBOWL 0
#define CID_EtekA   12
#define CID_EtekB   13
#define CID_EtekC   14
#define CID_EtekD   15
#define CID_EtekE   16

const byte RX433_PIN = 2; 
const byte TX433_PIN = 4;  
const byte  LED1_PIN = 5;
const byte TXPWR_PIN = 6;  

MySensor gw;
MyMessage msg(CID_LIGHT, V_LIGHT_LEVEL);
MyMessage msgDogbowlTrip(CID_DOGBOWL, V_TRIPPED);
unsigned long SLEEP_TIME = 10000; // Sleep time between reads (in milliseconds)


// 433 MODULE VARS
RH_ASK RHdriver(1200,2, 88);  //(speed, RXpin, TXpin, ptt;IN, inverted)
RHDatagram RHmanager(RHdriver, 2);

int lastLightLevel;
int lastDogbowlVal;


//char * RFmsg;
uint8_t buf[22];
int W=0, X=0, Y=0, Z=0;


// ^^^^ RC VARIABLES^^^^
RCSwitch RCsend = RCSwitch();
unsigned int pulsetime = 140;  // RCremote pulse out

byte etekState = 0;
byte etekID = 4;


// *****************************************
// ETEK CODES
// *****************************************

const int NUMBER_OF_etekELEMENTS = 20;
typedef struct {
   char etekCodeArr[16];
   byte etekStateArr;
   byte etekIDArr;
} etekCODES;

const etekCODES etekArr [NUMBER_OF_etekELEMENTS] PROGMEM = {
  {"F0FF0FFF0101",1,0}, {"F0FF0FFF1001",1,1}, {"F0FF0FF10001",1,2}, {"F0FFFF1F0001",1,3}, {"F0FFF1FF0001",1,4},
  {"F0FFFFFF0101",1,0}, {"F0FFFFFF1001",1,1}, {"F0FFFFF10001",1,2}, {"F0FF0F1F0001",1,3}, {"F0FF01FF0001",1,4},
  {"F0FF0FFF0110",0,0}, {"F0FF0FFF1010",0,1}, {"F0FF0FF10010",0,2}, {"F0FFFF1F0010",0,3}, {"F0FFF1FF0010",0,4},
  {"F0FFFFFF0110",0,0}, {"F0FFFFFF1010",0,1}, {"F0FFFFF10010",0,2}, {"F0FFFF1F0010",0,3}, {"F0FF01FF0010",0,4},
};




void setup()  
{ 
  gw.begin(incomingMessage, 209, true);

  // Send the sketch version information to the gateway and Controller
  gw.sendSketchInfo("Light Sensor", "1.0");

  // Register all sensors to gateway (they will be created as child devices)
  gw.present(CID_LIGHT, S_LIGHT_LEVEL);
  gw.present(CID_DOGBOWL, S_DOOR);
  
  
  if (!RHmanager.init())  // start RH Driver
    Serial.println(F("RH init failed"));  /* DEFAULTS 434MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36  */

  RCsend.enableTransmit(TX433_PIN);
  RCsend.setProtocol(1);
  RCsend.setPulseLength(pulsetime);
  RCsend.setRepeatTransmit(6); 
//  RCrecv.enableReceive(0);   // ARDUINO PIN 2 - interrupt 0 


  Serial.println(F("setup complete"));
  
}



void loop()      
{     
  //int dogbowlFULL = 88;
  int DogbowlVal;
  if (RHmanager.available()){
      uint8_t len = sizeof(buf);
      uint8_t from;
      if (RHmanager.recvfrom(buf, &len, &from))
      {
        sscanf ((char*)buf, "%d%*c%d%*c%d%*c%d%", &W, &X, &Y, &Z);        
      }
  }
  
  DogbowlVal = W;
  if(DogbowlVal!=lastDogbowlVal)
  {
    int DogbowlTrip;
    Serial.print(F("RF DATA: ")); Serial.println(DogbowlVal);
    gw.send(msgDogbowlTrip.set(DogbowlVal==1 ? DogbowlTrip=1 : DogbowlTrip=0));
    lastDogbowlVal=DogbowlVal;
  }
  

  int lightLevel = 88;
  //Serial.println(lightLevel);
  if (lightLevel != lastLightLevel) {
      //gw.send(msg.set(lightLevel));
      lastLightLevel = lightLevel;
  }
  //gw.wait(SLEEP_TIME);
}

// @@@@@@@ END LOOP @@@@@@@@@@@@

void incomingMessage(const MyMessage &message)
{
    int CID = (int)message.sensor;
  if (CID>=CID_EtekA && CID<=CID_EtekE) {
    
    etekState = atoi( message.data );
    //Serial.println( F("Etek msg received") );         
    if ((etekState<0)||(etekState>1)) {
      Serial.println( F("Etek data invalid (should be 0/1)") );
      return;
    }
    
    int etekID = CID - CID_EtekA;
    Serial.print( F("EtekID: ") );   Serial.println( etekID );  
    Serial.print( F("EtekState: ") );   Serial.println( etekState );    
    RC_SEND( etekID, etekState );
   
  }
  
}





