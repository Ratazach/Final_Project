#include <SoftwareSerial.h>
String AP = "moshiw2";        //Access Point
String PASS = "123454321";    //Wi-Fi Password
String IP = "192.168.0.26";   //IP address
String GW = "192.168.0.1";    //Gate Way
String NM = "255.255.255.0";  //Net Mask
int countTrueCommand;
int countTimeCommand;
int pinKipas = 6;
int pinLampuKamar = 5;

//push buttong
#define btnPin 11
int btnCounter = 0;
int btnState = 0;
int lastbtnState = 0;

boolean found = false;
SoftwareSerial esp8266(2, 3);  //RX,TX

int x = 9;
int y = 9;
int xxx = 0;
int yyy = 0;
int i = 1;

//------------------------------------------------------------------------
void setup() {
  pinMode(pinLampuKamar,OUTPUT);
  pinMode(pinKipas, OUTPUT);
  Serial.begin(9600);
  esp8266.begin(115200);
  sendCommand("AT", 1 , "OK");                                             //Check Esp Available; Tests AT startup.
  sendCommand("AT+CWMODE=1", 1, "OK");                                     //Sets the Current Wi-Fi mode; 1. Station Mode
  sendCommand("AT+CWJAP=\"" + AP + "\",\"" + PASS + "\"", 7, "OK");        //Connects to an AP
  sendCommand("AT+CIPSTA_CUR=\""+IP+"\",\""+GW+"\",\""+NM+"\"", 2, "OK");  //Sets the IP Address of the ESP8266 SoftAP
  sendCommand("AT+CIPMUX=1", 1, "OK");                                     //Conﬁgures the multiple connections mode
  sendCommand("AT+CIPSERVER=1,80", 1, "OK");                               //Create TCP server
  
delay (250);
  esp8266.println("AT+CIFSR");                                             //Gets Local IP Address
  Serial.println(esp8266.readStringUntil('\n'));
  printResponse();
}


//-------------------------------------------------------------------
void loop() {
  printResponse();
  baca_tombol();
  
  delay(500);
}
//-------------------------------------------------------------------

//-------------------------------------------------------------------
void sendCommand(String perintah, int maxTime, char readReplay[]) {
  //fungsi komunikasi serial yang berisi at command, waktu pemeriksaan, dan balasan
  Serial.print("AT Command => ");
  Serial.print(perintah); //mengirim perintah at command
  while (countTimeCommand < (maxTime * 1)) //selama waktu pemeriksaan akan mengirim at command 
  {
      sending:
    esp8266.println(perintah);//perintah at command yang dikirim
    if (esp8266.find(readReplay)) 
    //memeriksa balasan apakah sesuai dengan permintaan
    {
      found = true;
      delay(200);
      break;
    }

    countTimeCommand++;//waktu pemeriksaan bertambah tiap detik
  }
  if (found == true) //saat balasan sesuai
  {
    Serial.println(" COMMAND SUCCESS");
    i=1;
    countTimeCommand = 0;//reset waktu
  }
  if (found == false) //saat balasan tidak sesuai
  {
//    Serial.print("");Serial.print(i);Serial.println(" of 2");    
    if(i<2){ //mengirim kembali at command selama 5 kali 
      i++;
    goto sending;
    }
    else { //saat at command tidak terkirim setelah 5 kali percobaan
//      Serial.println("command skipped"); 
      Serial.println(" COMMAND SUCCESS.");
      countTimeCommand = 0; //reset waktu
      i=1;
      }
  }
  found = false;
}
//---------------------------------------------------------------------
void printResponse() {
  while (esp8266.available()) {
    
           Serial.println(esp8266.readStringUntil('\n')); 
      if (esp8266.find("code"))     {
    String urlget = esp8266.readStringUntil('\n');

    int posisi_a = urlget.indexOf("code");
    char a = urlget.charAt(posisi_a+1);
    char b = urlget.charAt(posisi_a + 2);
//    Serial.println(urlget);
//    Serial.print("nilai a = ");Serial.println(a);
//    Serial.print("nilai b = ");Serial.println(b);
    Serial.print("dapat kode : ");Serial.print(a);Serial.println(b);
    x = a - '0';
    y = b - '0';
    initiate(x,y);    
    }
  }
}

void initiate(int device, int state){
  if(device==0){
    if (state==1){
      order(1);
      }
    else if (state==0){
      order(2);
      }
  }
  
  else if(device==2){
    if (state==1){
      order(3);
      }
    else if (state==0){
      order(4);
      }
  }
  else{
    order(0);
    }
      feedback(device,state);
  }
//-------------------------------------------------------------
  void feedback(int dvc, int sts){
 Serial.println(dvc);Serial.println(sts);
 esp8266.println("AT+CIPSEND=0,8");delay(300);
 esp8266.print("fdbk");esp8266.print(dvc);esp8266.println(sts);delay(350);
 esp8266.println("AT+CIPCLOSE=0");delay(350);
 xxx=dvc;
 yyy=sts;
  }
//-------------------------------------------------------------
void order(int perintah){
  switch (perintah) {
  case 1:                     //Menyalakan Lampu Kamar
    digitalWrite(pinLampuKamar, HIGH);
  delay(200);
    break;   
  case 2:                     //Mematikan Lampu Kamar
    digitalWrite(pinLampuKamar, LOW);
  delay(200);
    break;
  case 3:                     //Menyalakan Kipas Kamar
    digitalWrite(pinKipas, HIGH);
  delay(200);
    break;
  case 4:                     //Mematikan Kipas Kamar
    digitalWrite(pinKipas, LOW);
  delay(200);
    break;
  default:
  break;
      }
  }
//---------------------------------------------------------------------------------- BACA TOMBOL
void baca_tombol() {
  btnState = digitalRead(btnPin);
  if (btnState != lastbtnState) {
    if (btnState == 1) {
      btnCounter++;
      Serial.println("btn pressed");
       Serial.println(xxx);Serial.println(yyy);
      esp8266.println("AT+CIPSEND=0,8");delay(300);
      esp8266.print("fdbk");esp8266.print(xxx);esp8266.println(yyy);delay(350);
      esp8266.println("AT+CIPCLOSE=0");delay(350);  
     countTrueCommand++;
    } else {
      Serial.println("btn released");
    }
}
  lastbtnState = btnState;
}
//---------------------------------------------------------------------------------
