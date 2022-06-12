#include <SoftwareSerial.h>
//String AP = "TP-LINK_AP_DE76";
//String PASS = "69407789";

String AP = "moshiw";
String PASS = "konsbudjaya";

//String AP = "Wisma RG";
//String PASS = "Toptenan";
String IP = "192.168.1.25";
String GW = "192.168.1.1";
String NM = "255.255.255.0";

#define ledin A5
#define led 7
#define btnPin 4

int btnCounter = 0;
int btnState = 0;
int lastbtnState = 0;
int x = 9;
int y = 9;

String ALAMAT = "0.0.0.0";
String KAMAR = "192.168.1.26";
String TERAS = "192.168.1.27";
String MONITORY = "192.168.1.99";

String PORT = "80";

int countTrueCommand;
int countTimeCommand;
boolean found = false;
SoftwareSerial esp8266(3, 2);
String kirim = "code00";
int DATA =  0;
int state =  0;
int i=1;
//..................................................................
int satu = A0;
int dua  = A1;
int tiga = A2;
int trig = A3;

int trignow = 0;
int lastTrig = 0;

int buzzer = A5;
int switchnow = 8;
int roomlightnow = 9; 
int terracelightnow = 10;
int thefannow = 11; 
int lockdoornow = 12;
int tunda2an = 0;
int buzzer_mode=0;

int www = 9;
int xxx = 9;
int yyy = 9;
//..................................................................

//-------------------------------------------------------------------------------- SETUP
void setup() {
  pinMode(btnPin, INPUT);
  pinMode(satu, INPUT);
  pinMode(dua, INPUT);
  pinMode(tiga, INPUT);
  pinMode(trig, INPUT);

  pinMode(switchnow, OUTPUT);
  pinMode(roomlightnow, OUTPUT);
  pinMode(terracelightnow, OUTPUT);
  pinMode(thefannow, OUTPUT);
  pinMode(lockdoornow, OUTPUT);

  digitalWrite(switchnow,0);
  digitalWrite(roomlightnow,0);
  digitalWrite(terracelightnow,0);
  digitalWrite(thefannow,0);
  digitalWrite(lockdoornow,0);

  pinMode(ledin, OUTPUT);
  pinMode(led, OUTPUT);

  // put your setup code here, to run once:
  Serial.begin(9600);
  esp8266.begin(115200);
  sendCommand("AT", 1, "OK");
  sendCommand("AT+CWMODE=3", 1, "OK");
  sendCommand("AT+CWJAP=\"" + AP + "\",\"" + PASS + "\"", 7, "OK");
  sendCommand("AT+CIPSTA_CUR=\"" + IP + "\",\"" + GW + "\",\"" + NM + "\"", 2, "OK");
  sendCommand("AT+CIPMUX=1", 1, "OK");
  sendCommand("AT+CIPSERVER=1,80", 1, "OK");                               //Create TCP server
  esp8266.println("AT+CIFSR");
  delay(300);
  printResponse();

}
//-------------------------------------------------------------------------------------- LOOP
void loop() {
  //  String hiyaaa=String(DATA);
//  baca_tombol();
  bacaInput();
  //    Serial.print(btnState);
  printResponse();
  if(buzzer_mode == 1){
    digitalWrite(buzzer,1);
    delay(500);
    digitalWrite(buzzer,0);
    delay(500);
    
  }else if (buzzer_mode==0){
    digitalWrite(buzzer,0);
    }
    else if (buzzer_mode==2){
    digitalWrite(buzzer,1);
    delay(100);
    digitalWrite(buzzer,0);
    delay(100);    
    }

  tunda2an++;
//  Serial.println(trignow);
  if(tunda2an == 100){         
          digitalWrite(trig,LOW);
  }
}
//--------------------------------------------------------------------------------------- SEND COMMAND
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
//----------------------------------------------------------------------------------- PRINT RESPONSE
void printResponse() {
    while (esp8266.available()) {

    String urlmasuk = esp8266.readStringUntil('\n');
    Serial.println(urlmasuk);
    if (esp8266.find("fdbk"))     {
      String urlget = esp8266.readStringUntil('\n');
      int posisi_a = urlget.indexOf("fdbk");
      char a = urlget.charAt(posisi_a + 1);
      char b = urlget.charAt(posisi_a + 2);
//      Serial.println(urlget);
      Serial.print("dapat kode : ");Serial.print(a);Serial.println(b);
//      Serial.print("nilai a = "); Serial.println(a);
//      Serial.print("nilai b = "); Serial.println(b);
      x = a - '0';
      y = b - '0';
      initiate(x, y);
    /*  ALAMAT = MONITORY;
     sendCommand("AT+CIPSTART=0,\"TCP\",\"" + ALAMAT + "\"," + PORT, 1, "OK");
     esp8266.println("AT+CIPSEND=0,8"); delay(250);
     esp8266.println(kirim); delay(250); countTrueCommand++;
    */
    }
  }
}
//---------------------------------------------------------------------------------- BACA INPUT
void bacaInput() {
//    digitalRead(trig);
//      if (trig == 1) {
//         digitalWrite(switchnow,1);
//         digitalRead(satu);
//         digitalRead(dua);
//         digitalRead(tiga);
//         kiriman(satu, dua, tiga);
//      }
//      else if (trig== 0){
//        digitalWrite(switchnow,0);
//      }
  trignow = digitalRead(trig);
  if (trignow != lastTrig) {
    if (trignow == 1) {
        Serial.println("switch detected");
        digitalWrite(switchnow,HIGH);
        }
    
    else {
        Serial.println("switch mati");
        digitalWrite(switchnow,0);
        www = digitalRead(satu);
        xxx = digitalRead(dua);
        yyy = digitalRead(tiga);
       delay(50);    
        kiriman(www, xxx, yyy);
    }
    lastTrig = trignow;
//   delay(50);
  }
      
 }
//--------------------------------------------------------------------------------- KIRIMAN
void kiriman(int p, int q, int r) {
  if (p == 0) {
    if (q == 0) {
      if (r == 0) {
        //              ...
        Serial.println("matiin lampu kamar");
        ALAMAT = KAMAR;
        kirim = "code00";
      }
      else if (r == 1) {
        //              ...
        Serial.println("nyalain lampu kamar");
        ALAMAT = KAMAR;
        kirim = "code01";
      }
    }
    else if (q == 1) {
      if (r == 0) {
        //              ...
        Serial.println("matiin lampu teras");
        ALAMAT = TERAS;
        kirim = "code10";
      }
      else if (r == 1) {
        //              ...
        Serial.println("nyalain lampu teras");
        ALAMAT = TERAS;
        kirim = "code11";
      }
    }
  }
  if (p == 1) {
    if (q == 0) {
      if (r == 0) {
        //              ...
        Serial.println("matiin kipas");
        ALAMAT = KAMAR;
        kirim = "code20";
      }
      else if (r == 1) {
        //              ...
        Serial.println("nyalain kipas");
        ALAMAT = KAMAR;
        kirim = "code21";
      }
    }
    else if (q == 1) {
      if (r == 0) {
        //              ...
        Serial.println("ngunci pintu");
        ALAMAT = TERAS;
        kirim = "code30";
      }
      else if (r == 1) {
        //              ...
      }
    }
  }
  Serial.print ("Kirim Paket : "); Serial.print(kirim); Serial.println ("Dengan Alamat : "); Serial.println(ALAMAT);
  sendCommand("AT+CIPSTART=0,\"TCP\",\"" + ALAMAT + "\"," + PORT, 1, "OK");
  //  esp8266.println("AT+CIPSTART=0,\"TCP\",\"" + HOST + "\"," + PORT);delay(350);
  //  sendCommand("AT+CIPSEND=0,8", 1, "OK");
  esp8266.println("AT+CIPSEND=0,8"); delay(250);
  esp8266.println(kirim); delay(250); countTrueCommand++;
  
  
}
//---------------------------------------------------------------------------------- INITIATE
void initiate(int device, int state) {
  Serial.print("Found feedback = ");
       if (device == 0) {
         Serial.print("Room Light");
         device = roomlightnow;
       }
       else if (device == 1) {
         Serial.print("Terrace Light");
         device = terracelightnow;
       }
       else if (device == 2) {
         Serial.print("Fan");
          device = thefannow;
       }
       else if (device == 3) {
          Serial.print("Door");
          device = lockdoornow;
       }
       else if (device == 4) {
          Serial.print("buzzer");
          device = buzzer;
       }
       else {
          Serial.println("unknown");
       }
  Serial.print(" for command = ");
       if (state == 0) {
          digitalWrite(device, 0);
//          buzzer_kedip(0);
          buzzer_mode = 0;
          Serial.println("OFF");
       }
       else if (state == 1) {
          digitalWrite(device, 1);
          Serial.println("ON");
       }
       else if (state == 2) {
          buzzer_mode=1;
          Serial.println("danger");
       }
       else if (state == 3) {
//          digitalWrite(device, 1);
          buzzer_mode=2;
          Serial.println("fire");
       }
        else {
          Serial.println("unknown");
        }
}
//---------------------------------------------------------------------------------- BUZZER KEDIP
//void buzzer_kedip(int mode){
//  if(mode == 1){
//    digitalWrite(buzzer,1);
//    delay(100);
//    digitalWrite(buzzer,0);
//    
//  }else {
//    digitalWrite(buzzer,0);
//    }
//}

//---------------------------------------------------------------------------------- BACA TOMBOL
void baca_tombol() {
  btnState = digitalRead(btnPin);
  if (btnState != lastbtnState) {
    if (btnState == 1) {
      btnCounter++;
      //      Serial.println("on");
      //      Serial.print("Lamp BUTTON PUSH: ");
      //      Serial.println(btnCounter);
      Serial.print ("paket yg akan dikirim : "); Serial.println(kirim);
      sendCommand("AT+CIPSTART=0,\"TCP\",\"" + ALAMAT + "\"," + PORT, 1, "OK");
      //  esp8266.println("AT+CIPSTART=0,\"TCP\",\"" + HOST + "\"," + PORT);delay(350);
      //  sendCommand("AT+CIPSEND=0,8", 1, "OK");
      esp8266.println("AT+CIPSEND=0,8"); delay(250);
      esp8266.println(kirim); delay(250); countTrueCommand++;
    } else {
      //      Serial.println("off");
    }
    //    delay(50);
  }
  lastbtnState = btnState;
//  if (btnCounter % 2 == 0) {
//    kirim = "code01";
//  } else {
//    kirim = "code00";
//  }
}
//-------------------------------------------------------------
