#include <SoftwareSerial.h>
//String AP = "Wisma RG";        //Access Point
//String PASS = "Toptenan";    //Wi-Fi Password

String AP = "moshiw";
String PASS = "konsbudjaya";
String IP = "192.168.0.27";   //IP address
String GW = "192.168.0.1";    //Gate Way
String NM = "255.255.255.0";  //Net Mask

String ALAMAT = "192.168.0.25";
String PORT = "80";

int countTrueCommand;
int countTimeCommand;
int pinLampuTeras = 8;
int pinServo = 7;
int perintah;
int data = 0;
int i;
const int ledpin = 11;

//push buttong
#define btnPin 9
int btnCounter = 0;
int btnState = 0;
int lastbtnState = 0;

//Fingerprint
#include <Adafruit_Fingerprint.h>
volatile int finger_status = -1;
SoftwareSerial mySerial(3, 2); // TX/RX on fingerprint sensor
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

#include <Servo.h>
Servo myServo;

boolean found = false;
SoftwareSerial esp8266(5, 4);  //RX,TX

int xxx = 0;
int yyy = 0;

//------------------------------------------------------------------------
void setup() {
Serial.begin(9600);
  myServo.attach(pinServo);
  myServo.write(90);
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  Serial.println("Waiting for valid finger...");
   
  
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
  bacaFingerprintServo();
  
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
    
//           Serial.println(esp8266.readStringUntil('\n')); 
      if (esp8266.find("code"))     {
    String urlget = esp8266.readStringUntil('\n');

    int posisi_a = urlget.indexOf("code");
    char a = urlget.charAt(posisi_a+1);
    char b = urlget.charAt(posisi_a + 2);
    Serial.print("dapat kode : ");Serial.print(a);Serial.println(b);
//    Serial.println(urlget);
//    Serial.print("nilai a = ");Serial.println(a);
//    Serial.print("nilai b = ");Serial.println(b);
    int x = a - '0';
    int y = b - '0';
    initiate(x,y);    
    }
  }
}

//---------------------------------------------------------------------     FINGERPRINT
void bacaFingerprintServo(){
  finger_status = getFingerprintIDez();
  if (finger_status!=-1 and finger_status!=-2){
    Serial.println("Match");
//    delay(100);
    data++;
    if (data > 1){data=0;}
    if(data==1){
//      Serial.println (data); 
      delay(100);
      myServo.write(0);
      Serial.println ("buka");
      yyy = 1;
      delay(1000);
      }
   if(data == 0){
//        Serial.println (data); 
        delay(100);
        myServo.write(90);
        Serial.println ("tutup");
        yyy = 0;
        delay(1000);
        }
    
  } else{
    if (finger_status==-2){
      for (int ii=0;ii<5;ii++){
        Serial.print("Not Match");
        delay(1000);
      }
    }
  }
  delay(50);
  
}

void initiate(int device, int state){
  if(device==1){
    if (state==1){
      order(1);
      }
    else if (state==0){
      order(2);
      }
  }
  
  else if(device==3){
    if (state==0){
      order(3);
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
  case 1:                     //Menyalakan Lampu Teras
    LampuTerasNyala();
    break;   
  case 2:                     //Mematikan Lampu Teras
    LampuTerasMati();
    break;
  case 3:                     //Mengunci Pintu
    myServo.write(90);
    Serial.println ("tutup");
    delay(100);
    break;

  default:
    break;
      }
  }

//--------- ------------------------------------------------------------------------- BACA TOMBOL
void baca_tombol() {
  btnState = digitalRead(btnPin);
  if (btnState != lastbtnState) {
    if (btnState == 1) {
      btnCounter++;
      Serial.println("btn pressed");
      finger.begin(57600);
      digitalWrite(ledpin,HIGH);
//       Serial.println(xxx);Serial.println(yyy);
//      esp8266.println("AT+CIPSEND=0,8");delay(300);
//      esp8266.print("fdbk");esp8266.print(xxx);esp8266.println(yyy);delay(350);
//      esp8266.println("AT+CIPCLOSE=0");delay(350);  
//     countTrueCommand++;

    } else {
      Serial.println("btn released");
       esp8266.begin(115200);
       sendCommand("AT+CIPSERVER=1,80", 1, "OK");                               //Create TCP server
      sendCommand("AT+CIPSTART=0,\"TCP\",\"" + ALAMAT + "\"," + PORT, 1, "OK");
      esp8266.println("AT+CIPSEND=0,8");delay(300);
      esp8266.print("fdbk");esp8266.print(3);esp8266.println(yyy);delay(350);
      esp8266.println("AT+CIPCLOSE=0");delay(350);  
//       esp8266.listen();
//
//  while (esp8266.available() > 0) {
//
//    Serial.println(esp8266.readStringUntil('\n')); 
//      if (esp8266.find("code"))     {
//    String urlget = esp8266.readStringUntil('\n');
//
//    int posisi_a = urlget.indexOf("code");
//    char a = urlget.charAt(posisi_a+1);
//    char b = urlget.charAt(posisi_a + 2);
//    Serial.println(urlget);
//    Serial.print("nilai a = ");Serial.println(a);
//    Serial.print("nilai b = ");Serial.println(b);
//    int x = a - '0';
//    int y = b - '0';
//    initiate(x,y);  
//      }
//  }
      digitalWrite(ledpin,LOW);
    }
}
  lastbtnState = btnState;
}

//---------------------------------------------------------------------------------
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p!=2){
//    Serial.println(p);
  }
  if (p != FINGERPRINT_OK)  return -1;
  
  p = finger.image2Tz();
  if (p!=2){
//    Serial.println(p);
  }
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -2;
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID; 
}


void LampuTerasNyala(){
  digitalWrite(pinLampuTeras, HIGH);
  delay(200);
  }

void LampuTerasMati(){
  digitalWrite(pinLampuTeras,LOW);
  delay(200);
  }
