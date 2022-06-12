#include "DHT.h"
#include <MQ2.h>

#include <SoftwareSerial.h>
String AP = "moshiw";        //Access Point
String PASS = "konsbudjaya";    //Wi-Fi Password
String IP = "192.168.0.99";   //IP address
String GW = "192.168.0.1";    //Gate Way
String NM = "255.255.255.0";  //Net Mask
int countTrueCommand;
int countTimeCommand;

String ALAMAT = "192.168.0.25";
String PORT = "80";
String kirim2an;

int xxx = 0;
int yyy = 0;
int i = 1;

//Sensor MQ-2
int smoke = A1;
float nilaiGas;         //inisialisasi variabel nilai gas
MQ2 mq2(smoke);
int batasGas = 2500;     //threshold
boolean stateGas = 0;
boolean lastGas = 0;

//Sensor DHT 11 Temperature
const int batasTemp = 35;
#define DHTTYPE DHT11
const int DHTPIN = A2;
DHT dht(DHTPIN, DHTTYPE);
float C;                  // temperature
boolean stateTemp = 0;
boolean lastTemp = 0;

//BUZZER
const int buzzerPin = 5;
boolean buzzer_mode = false;
int buzzerState = LOW;

const int buttonPinReset = 10;
int button_state = 0;

//push buttong
#define btnPin 11
int btnCounter = 0;
int btnState = 0;
int lastbtnState = 0;

boolean found = false;
SoftwareSerial esp8266(3, 2);  //RX,TX
String s1 = "OFF";
String s2 = "OFF";
String s3 = "OFF";
String s4 = "Terkunci";


const int ledPin = 8;  //LED Alarm
boolean LED_mode = false;
int ledState = LOW;

long previousMillis = 0;
long interval = 100;    // Interval at which LED blinks

//------------------------- SETUP
void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);
  pinMode(buttonPinReset, INPUT);
  Serial.begin(9600);
  dht.begin();
  mq2.begin();
  
  esp8266.begin(115200);
  sendCommand("AT", 1 , "OK");                                             //Check Esp Available; Tests AT startup.
  sendCommand("AT+CWMODE=3", 1, "OK");                                     //Sets the Current Wi-Fi mode; 1. Station Mode
  sendCommand("AT+CWJAP=\"" + AP + "\",\"" + PASS + "\"", 7, "OK");        //Connects to an AP
  sendCommand("AT+CIPSTA_CUR=\"" + IP + "\",\"" + GW + "\",\"" + NM + "\"", 2, "OK"); //Sets the IP Address of the ESP8266 SoftAP
  sendCommand("AT+CIPMUX=1", 1, "OK");                                     //Conﬁgures the multiple connections mode
  sendCommand("AT+CIPSERVER=1,80", 1, "OK");                               //Create TCP server

  delay (250);
  esp8266.println("AT+CIFSR");                                             //Gets Local IP Address
  Serial.println(esp8266.readStringUntil('\n'));
  printResponse();
}

//------------------------- LOOP
void loop() {
  
  bacaGas();
  bacaTemp();
  printGas();
  printTemp();
  printResponse();
  //baca_tombol();

//---------------------------------------------------Penentuan State Pembacaan Sensor
  if (nilaiGas >= batasGas && C >= batasTemp) {
    stateGas = true;
    stateTemp = true;
  }
  else if (C >= batasTemp) {
    stateTemp = true;
  }
  else if (nilaiGas >= batasGas) {
    stateGas = true;
  }
  else {
    stateGas = false;
    stateTemp = false;
  }

//------------------------------------------Penentuan State Aktuator Berdasarkan State Sensor
  if (stateGas != lastGas && stateTemp != lastTemp) {
    if (stateGas == 1 && stateTemp == 1) {
      LED_mode = true;
      buzzer_mode = true;
      kirim2an = "fdbk43";
    }
    else {
      LED_mode = false;
      buzzer_mode = false;
      kirim2an = "fdbk40";
    }
    lastGas = stateGas;
    lastTemp = stateTemp;
    kirim_info(kirim2an);
  }
  else if (stateTemp != lastTemp) {
    if (stateTemp == 1) {
      LED_mode = true;
      buzzer_mode = true;
      kirim2an = "fdbk43";
    }
    else {
      LED_mode = false;
      buzzer_mode = false;
      kirim2an = "fdbk40";
    }
    lastTemp = stateTemp;
    kirim_info(kirim2an);
  }
  else if (stateGas != lastGas) {
    if (stateGas == 1) {
      LED_mode = true;
      buzzer_mode = false;
      kirim2an = "fdbk42";
    }
    else {
      LED_mode = false;
      buzzer_mode = false;
      kirim2an = "fdbk40";
    }
    lastGas = stateGas;
    kirim_info(kirim2an);
  }

//-----------------------------------------------------Kedip LED
  if (LED_mode) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis > interval) {
      previousMillis = currentMillis;
      if (ledState == LOW)
        ledState = HIGH;
      else
        ledState = LOW;

      digitalWrite(ledPin, ledState);
    }
  }
  else {
    digitalWrite(ledPin, LOW);
  }

//-----------------------------------------------------Buzzer

  if(buzzer_mode){
    digitalWrite(buzzerPin, HIGH);
    }
  else{
    digitalWrite(buzzerPin, LOW);
    }

//-----------------------------------------------------Push Button Reset
  button_state = digitalRead(buttonPinReset);
  if (button_state == 1) {
    //stateGas = true;
    //stateTemp = true;
    lastGas = stateGas;
    lastTemp = stateTemp;
    LED_mode = false;
    buzzer_mode = false;
    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW);
    Serial.println("State Reset.");
    kirim2an = "fdbk40";
    kirim_info(kirim2an);
    delay(1000);
  }

  delay(500);

}

//------------------------- FUNGSI
//-------------------------------- SENSOR
void bacaGas() {
  nilaiGas = mq2.readLPG();
  delay(10);
}

void bacaTemp() {
  C = dht.readTemperature();
  delay(10);
}

void printGas() {
  Serial.print("Gas: ");
  Serial.println(nilaiGas);
  Serial.print("    ");
}

void printTemp() {
  Serial.print("C: ");
  Serial.println(C);
  Serial.println("    ");
}

//-------------------------------- KOMUNIKASI
//----------------------------------------------------------------------------------- SEND COMMAND
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
    Serial.print(" FAIL...RESEND ");Serial.print(i);Serial.println(" of 2");    
    if(i<2){ //mengirim kembali at command selama 5 kali 
      i++;
    goto sending;
    }
    else { //saat at command tidak terkirim setelah 5 kali percobaan
      Serial.println("command skipped"); 
      countTimeCommand = 0; //reset waktu
      i=1;
      }
  }
  found = false;
}
//----------------------------------------------------------------------------------- RESPONSE
void printResponse() {
  while (esp8266.available()) {

    Serial.println(esp8266.readStringUntil('\n'));
    if (esp8266.find("code"))     {
      String urlget = esp8266.readStringUntil('\n');

      int posisi_a = urlget.indexOf("code");
      char a = urlget.charAt(posisi_a + 1);
      char b = urlget.charAt(posisi_a + 2);
      Serial.println(urlget);
      Serial.print("nilai a = "); Serial.println(a);
      Serial.print("nilai b = "); Serial.println(b);
      int x = a - '0';
      int y = b - '0';
      monitory(x, y);
      xxx = x;
      yyy = y;
    }
  }
}

//---------------------------------------------------------------------------------- BACA TOMBOL
void baca_tombol() {
  btnState = digitalRead(btnPin);
  if (btnState != lastbtnState) {
    if (btnState == 1) {
      btnCounter++;
      Serial.println("btn pressed");
      Serial.println(xxx); Serial.println(yyy);
      monitory(xxx, yyy);
      countTrueCommand++;
    } else {
      Serial.println("btn released");
    }
  }
  lastbtnState = btnState;
}
//----------------------------------------------------------------------------------- INFO
void kirim_info(String info){
  
  sendCommand("AT+CIPSTART=0,\"TCP\",\"" + ALAMAT + "\"," + PORT, 1, "OK");
  esp8266.println("AT+CIPSEND=0,8"); delay(250);
  esp8266.println(info);Serial.println("Mengirim Data."); delay(250); countTrueCommand++;
  }
//----------------------------------------------------------------------------------- MONITORY
void monitory(int dvc2, int sts2) {
  String pt = "??????           ";
  String ct = "??????";
  if (dvc2 == 0) {
    if (sts2 == 1) {
      s1 = "ON ";
      pt = "Nyalakan Lampu Kamar";
      ct = "code01";
    }
    else if (sts2 == 0) {
      s1 = "OFF";
      pt = "Matikan Lampu Kamar";
      ct = "code00";
    }
  }
  else if (dvc2 == 1) {
    if (sts2 == 1) {
      s2 = "ON ";
      pt = "Nyalakan Lampu Teras";
      ct = "code11";
    }
    else if (sts2 == 0) {
      s2 = "OFF";
      pt = "Matikan Lampu Teras";
      ct = "code10";
    }
  }
  else if (dvc2 == 2) {
    if (sts2 == 1) {
      s3 = "ON ";
      pt = "Nyalakan Kipas      ";
      ct = "code21";
    }
    else if (sts2 == 0) {
      s3 = "OFF";
      pt = "Matikan Kipas       ";
      ct = "code20";
    }
  }
  else if (dvc2 == 3) {
    if (sts2 == 1) {
      s4 = "Terbuka ";
      pt = "                    ";
      ct = "code31";
    }
    else if (sts2 == 0) {
      s4 = "Tertutup";
      pt = "Kunci Pintu         ";
      ct = "code30";
    }
  }
  else {
    //
    //    pt = "????                  ";
    //    ct = "????";
  }
  delay(500);
  esp8266.println("AT+CIPSEND=0,190"); delay(300);
  esp8266.print("Lampu Kamar,kondisi : "); esp8266.println(s1);
  esp8266.print("Lampu Teras,kondisi : "); esp8266.println(s2);
  esp8266.print("kipas      ,kondisi : "); esp8266.println(s3);
  esp8266.print("Kunci Pintu,kondisi : "); esp8266.println(s4);
  esp8266.println(" ");
  esp8266.print("perintah terakhir   : "); esp8266.println(pt);
  esp8266.println(" ");
  esp8266.print("code terakhir     : "); esp8266.println(ct);
  delay(350);

  esp8266.println("AT+CIPCLOSE=0"); delay(350);
}
