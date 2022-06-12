/**
  ******************************************************************************
  * @file    smarthome_voicerecog.ino
  * @author  Rizal Ardana
  * @brief   This file provides a demostration on 
              how to control local network smarthome system
*/
#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"

/**        
  Connection
  Arduino    VoiceRecognitionModule
   2   ------->     TX
   3   ------->     RX
*/
VR myVR(2,3);    // 2:RX 3:TX

uint8_t records[7]; // total command
uint8_t buf[64];
int switchcount = 0;
int tunda2an = 0;

//..................................................................
#define satu A0
#define dua  A1
#define tiga A2
#define trig A3

int satuP = 0;
int duaP = 0;
int tigaP = 0;
int trigP =0;


#define switchnow  8
#define roomlightnow  9 
#define terracelightnow 10
#define thefannow   11
#define lockdoornow 12


//..................................................................
#define switchTrig        (0)
#define roomlight         (1) 
#define terracelight      (2)
#define thefan            (3) 
#define lockdoor          (4)
/**
  @brief   Print signature, if the character is invisible, 
           print hexible value instead.
  @param   buf     --> command length
           len     --> number of parameters
*/
void printSignature(uint8_t *buf, int len)
{
  int i;
  for(i=0; i<len; i++){
    if(buf[i]>0x19 && buf[i]<0x7F){
      Serial.write(buf[i]);
    }
    else{
      Serial.print("[");
      Serial.print(buf[i], HEX);
      Serial.print("]");
    }
  }
}

/**
  @brief   Print signature, if the character is invisible, 
           print hexible value instead.
  @param   buf  -->  VR module return value when voice is recognized.
             buf[0]  -->  Group mode(FF: None Group, 0x8n: User, 0x0n:System
             buf[1]  -->  number of record which is recognized. 
             buf[2]  -->  Recognizer index(position) value of the recognized record.
             buf[3]  -->  Signature length
             buf[4]~buf[n] --> Signature
*/
void printVR(uint8_t *buf)
{
//  Serial.println("VR Index\tGroup\tRecordNum\tSignature");
//
//  Serial.print(buf[2], DEC);
//  Serial.print("\t\t");
//
//  if(buf[0] == 0xFF){
//    Serial.print("NONE");
//  }
//  else if(buf[0]&0x80){
//    Serial.print("UG ");
//    Serial.print(buf[0]&(~0x80), DEC);
//  }
//  else{
//    Serial.print("SG ");
//    Serial.print(buf[0], DEC);
//  }
//  Serial.print("\t");
//
//  Serial.print(buf[1], DEC);
//  Serial.print("\t\t");
//  if(buf[3]>0){
//    printSignature(buf+4, buf[3]);
//  }
//  else{
//    Serial.print("NONE");
//  }
//  Serial.println("\r\n");
}

void setup()
{
  /** initialize */
  myVR.begin(9600);
  
  
  Serial.begin(115200);
  Serial.println("Local Network Smarthome System Voice Recignition Sample");
  
  pinMode(satu, OUTPUT);
  pinMode(dua, OUTPUT);
  pinMode(tiga, OUTPUT);
  pinMode(trig, OUTPUT);

  pinMode(switchnow, INPUT);
  pinMode(roomlightnow, INPUT);
  pinMode(terracelightnow, INPUT);
  pinMode(thefannow, INPUT);
  pinMode(lockdoornow, INPUT);

digitalWrite(satu,LOW);
digitalWrite(dua,LOW);
digitalWrite(tiga,LOW);
digitalWrite(trig,LOW);

//satuP = digitalWrite(satu,LOW);
//duaP = digitalWrite(dua,LOW);
//tigaP = digitalWrite(tiga,LOW);
//trigP = digitalWrite(trig,LOW);
  
//  int switchTrigP = 
      
  if(myVR.clear() <= 0){
    Serial.println("Recognizer cleared.");
  }else{
    Serial.println("Not find VoiceRecognitionModule.");
    Serial.println("Please check connection and restart Arduino.");
    while(1);
  }
  myVR.println("load 0 1 2 3 4");
  if(myVR.load((uint8_t)switchTrig) >= 0){
    Serial.println("0 loaded");
  }
  
  if(myVR.load((uint8_t)roomlight) >= 0){
    Serial.println("1 loaded");
  }
  
  if(myVR.load((uint8_t)terracelight) >= 0){
    Serial.println("2 loaded");
  }
  
  if(myVR.load((uint8_t)thefan) >= 0){
    Serial.println("3 loaded");
  }
  
  if(myVR.load((uint8_t)lockdoor) >= 0){
    Serial.println("4 loaded");
  }
  
//  if(myVR.load((uint8_t)turnofftiga) >= 0){
//    Serial.println("turn off tiga loaded");
//  }
//  if(myVR.load((uint8_t)lockthedoor) >= 0){
//    Serial.println("lock the door loaded");
//  }
}

void loop()
{
  if (trig == LOW){
    trigP = 0;
    }
    else if (trig == HIGH){
      trigP = 1;
      }
  
 int switchsts = digitalRead(switchnow);
 int roomlightsts = digitalRead(roomlightnow); 
 int terracelightsts = digitalRead(terracelightnow);
 int thefansts = digitalRead(thefannow);
 int lockdoorsts = digitalRead(lockdoornow);
//  Serial.println(digitalRead(trig), DEC);
  tunda2an++;
          if(tunda2an == 100){        
          tunda2an = 0; 
          Serial.println("switch off");
          digitalWrite(trig,LOW);
          }
  int ret;
  ret = myVR.recognize(buf, 50);
  if(ret>0){
    switch(buf[1]){
      case switchTrig:
        /** switch */
        tunda2an=0;
        Serial.println("switch triggered");
        digitalWrite(trig,HIGH);
      break;
      case roomlight:
        /** room light*/
        if(switchsts==1){
          if(roomlightsts==0){
          Serial.println("nyalain lampu");
        digitalWrite(satu, 0);
        digitalWrite(dua, 0);
        digitalWrite(tiga, 1);
        tunda2an = 60;        
            }
          else if(roomlightsts==1){
        Serial.println("matiin lampu");
        digitalWrite(satu, 0);
        digitalWrite(dua, 0);
        digitalWrite(tiga, 0);
        tunda2an = 60;          
            }
          }
        break;
        case terracelight:
        /** terrace light */
        if(switchsts==1){
          if(terracelightsts==0){
       Serial.println("nyalain teras");
        digitalWrite(satu, 0);
        digitalWrite(dua, 1);
        digitalWrite(tiga, 1);
        tunda2an = 60;        
            }
        else if(terracelightsts==0){
        Serial.println("matiin teras");
        digitalWrite(satu, 0);
        digitalWrite(dua, 1);
        digitalWrite(tiga, 0);
        tunda2an = 60;            
            }
          }
        break;
        
      case thefan:
        /** the fan*/
        if(switchsts==1){
          if(thefansts==0){
        Serial.println("nyalain kipas");
        digitalWrite(satu, 1);
        digitalWrite(dua, 0);
        digitalWrite(tiga, 1);
        tunda2an = 60;          
            }
          else if(thefansts==1){
        Serial.println("matiin kipas");
        digitalWrite(satu, 1);
        digitalWrite(dua, 0);
        digitalWrite(tiga, 0);
        tunda2an = 60;
            }
          }
        break;
        case lockdoor:
        /** lock the door */
        Serial.println("kunci pintu");
        digitalWrite(satu, 1);
        digitalWrite(dua, 1);
        digitalWrite(tiga, 0);
        digitalWrite(trig, 1);
        tunda2an = 60;
        break;
      default:
        Serial.println("Record function undefined");
        break;
    }
    /** voice recognized */
    printVR(buf);
  }
}
