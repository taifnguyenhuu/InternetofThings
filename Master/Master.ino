#include "Arduino.h"
#include "LoRa_E32.h"
#include <SoftwareSerial.h>
#define BLYNK_TEMPLATE_ID "TMPLQW85SGxp"
#define BLYNK_DEVICE_NAME "Lora communication network application in shrimp farming"
#define BLYNK_AUTH_TOKEN "wDQOhRfeQzbsTuxwqRZcPce2CF536gLx"
#define BLYNK_FIRMWARE_VERSION "1.1.0" 
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h> 
#include <BlynkSimpleEsp8266.h>
#define APP_DEBUG
#define WARNINGSTATUS V14
#define WARNINGLEVEL V15
#define RUNMODE V16
SoftwareSerial sim(D5, D6); 
SoftwareSerial myHMI(D3, D4); 
SoftwareSerial mySerial(D1, D2); 
LoRa_E32 e32ttl(&mySerial);
BlynkTimer timer;
char auth[] = BLYNK_AUTH_TOKEN; 
char ssid[] = "NguyenHuuTai";
char pass[] = "624680";
String Data_From_Display = "";
String Data_From_Lora = "";
String _buffer;
String number = "0969812546";
String PH = ""; 
String TEMP = "";
String EC = "";
String DO = "";
String TDS = "";
String TSS = "";
String W002 = "";
float floatPH = 0;
float floatTEMP = 0;
float floatEC = 0; 
float floatDO = 0;
float floatTDS = 0;
float floatTSS = 0;
float floatW002 = 0;
int TT_LAMP = HIGH;
int TT_PUMP = HIGH;
int TT_FAN = HIGH;
int TT_DRAIN = HIGH;
int TT_FEED = HIGH;
int TT_CLEAN = HIGH;
int _timeout;
int timerID1,timerID2;
int mq2_value = 600;
int levelWarning=500;
boolean runMode=1;
boolean warningState=0;
boolean statusEC = 0;
boolean statusDO = 0;
boolean statusTDS = 0;
boolean statusTSS = 0;
WidgetLED CONNECTION(V13);
void setup()
{
    Serial.begin(9600);
    e32ttl.begin();
    myHMI.begin(9600);
    sim.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB
    }
    pinMode(relay, OUTPUT);
    digitalWrite(relay, LOW);
    _buffer.reserve(50);
    Blynk.begin(auth, ssid, pass);  
    timerID1 = timer.setInterval(1000L,handleTimerID1);
    delay(100);
}

String splitString(String str, String delim, uint16_t pos) {
  String tmp = str;
  for (int i = 0; i < pos; i++) {
    tmp = tmp.substring(tmp.indexOf(delim) + 1);
    if (tmp.indexOf(delim) == -1
        && i != pos - 1 )
      return "";
  }
  return tmp.substring(0, tmp.indexOf(delim));
}
BLYNK_WRITE(V7) {
  int virtualStatus = param.asInt(); 
  if (virtualStatus == 0) {
    e32ttl.sendFixedMessage(0, 3, 0x04, "LAMP");    
  }else {
    e32ttl.sendFixedMessage(0, 3, 0x04, "OFFLAMP");
  }
}
BLYNK_WRITE(V8) {
  int virtualStatus = param.asInt(); 
  if (virtualStatus == 0) {
    e32ttl.sendFixedMessage(0, 3, 0x04, "PUMP");    
  }else {
    e32ttl.sendFixedMessage(0, 3, 0x04, "OFFPUMP");
  }
}
BLYNK_WRITE(V9) {
  int virtualStatus = param.asInt(); 
  if (virtualStatus == 0) {
    e32ttl.sendFixedMessage(0, 3, 0x04, "FAN");    
  }else {
    e32ttl.sendFixedMessage(0, 3, 0x04, "OFFFAN");
  }
}

BLYNK_WRITE(V10) {
  int virtualStatus = param.asInt(); 
  if (virtualStatus == 0) {
    e32ttl.sendFixedMessage(0, 3, 0x04, "DRAIN");    
  }else {
    e32ttl.sendFixedMessage(0, 3, 0x04, "OFFDRAIN");
  }
}

BLYNK_WRITE(V11) {
  int virtualStatus = param.asInt(); 
  if (virtualStatus == 0) {
    e32ttl.sendFixedMessage(0, 3, 0x04, "FEED");    
  }else {
    e32ttl.sendFixedMessage(0, 3, 0x04, "OFFFEED");
  }
}
BLYNK_WRITE(V12) {
  int virtualStatus = param.asInt(); 
  if (virtualStatus == 0) {
    e32ttl.sendFixedMessage(0, 3, 0x04, "CLEAN");    
  }else {
    e32ttl.sendFixedMessage(0, 3, 0x04, "OFFCLEAN");
  }
}

void confirmDisplay() {
  myHMI.write(0xff);
  myHMI.write(0xff);
  myHMI.write(0xff);
}

void updateDataToScreen() {
  PH = splitString(Data_From_Lora, ";", 0);
  TEMP = splitString(Data_From_Lora, ";", 1);
  EC = splitString(Data_From_Lora, ";", 2);
  DO = splitString(Data_From_Lora, ";", 3);
  TDS = splitString(Data_From_Lora, ";", 4);
  TSS = splitString(Data_From_Lora, ";", 5);
  myHMI.print("Monitoringmode.t4.txt=" + String('"') + PH + String('"'));
  confirmDisplay();
  myHMI.print("Monitoringmode.t3.txt=" + String('"') + TEMP + String('"'));
  confirmDisplay();
  myHMI.print("Monitoringmode.t0.txt=" + String('"') + EC + String('"'));
  confirmDisplay();
  myHMI.print("Monitoringmode.t5.txt=" + String('"') + DO + String('"'));
  confirmDisplay();
  myHMI.print("Monitoringmode.t1.txt=" + String('"') + TDS + String('"'));
  confirmDisplay();
  myHMI.print("Monitoringmode.t2.txt=" + String('"') + TSS + String('"'));
  confirmDisplay();
  myHMI.print("Monitoringmode.t6.txt=" + String('"') + "OK!" + String('"'));
  confirmDisplay(); 
}

void updateDataToBlynk() {
  floatPH = PH.toFloat();
  floatTEMP = TEMP.toFloat();
  floatEC = EC.toFloat();
  floatDO = DO.toFloat();
  floatTDS = TDS.toFloat();
  floatTSS = TSS.toFloat();
  //Push data to virtual interactions
  Blynk.virtualWrite(V1, floatPH);
  Blynk.virtualWrite(V0, floatTEMP);
  Blynk.virtualWrite(V2, floatEC);
  Blynk.virtualWrite(V3, floatDO);
  Blynk.virtualWrite(V4, floatTDS);
  Blynk.virtualWrite(V5, floatTSS);
  Blynk.virtualWrite(V6, "OK!");
}

void checkButtonOnScreen() {
  Data_From_Display = "";
  if (myHMI.available()) { 
    Data_From_Display = myHMI.readString(); 
    //Serial.println(Data_From_Display);
  }
  if (Data_From_Display.indexOf("LAMP") >= 0) {
    //Serial.println("Yes");
    e32ttl.sendFixedMessage(0, 3, 0x04, "LAMP");
  }
  if (Data_From_Display.indexOf("PUMP") >= 0) {
    //Serial.println("YesPUMP");
    e32ttl.sendFixedMessage(0, 3, 0x04, "PUMP");
  } 
  if (Data_From_Display.indexOf("FAN") >= 0) {
    //Serial.println("YesPUMP");
    e32ttl.sendFixedMessage(0, 3, 0x04, "FAN");
  }
  if (Data_From_Display.indexOf("CLEAN") >= 0) {
    //Serial.println("YesPUMP");
    e32ttl.sendFixedMessage(0, 3, 0x04, "CLEAN");
  }
  if (Data_From_Display.indexOf("DRAIN") >= 0) {
    //Serial.println("YesPUMP");
    e32ttl.sendFixedMessage(0, 3, 0x04, "DRAIN");
  }
    if (Data_From_Display.indexOf("FEED") >= 0) {
    //Serial.println("YesPUMP");
    e32ttl.sendFixedMessage(0, 3, 0x04, "FEED");
  } 
}

void updateButton() {
  if (Data_From_Lora.indexOf("ONLAMP") >=0 ) {
    TT_LAMP= LOW;
    Blynk.virtualWrite(V7, 1);
  }else if (Data_From_Lora.indexOf("OFFLAMP") >=0 ) {
    TT_LAMP = HIGH;
    Blynk.virtualWrite(V7, 0);
  }
  
  if (Data_From_Lora.indexOf("ONPUMP") >=0 ) {
    statusEC=1;
    TT_PUMP= LOW;
    Blynk.virtualWrite(V8, 1);
  }else if (Data_From_Lora.indexOf("OFFPUMP") >=0 ) {
    TT_PUMP = HIGH;
    Blynk.virtualWrite(V8, 0);
    statusEC=0;
  }

  if (Data_From_Lora.indexOf("ONFAN") >=0 ) {
    TT_FAN= LOW;
    Blynk.virtualWrite(V9, 1);
  }else if (Data_From_Lora.indexOf("OFFFAN") >=0 ) {
    TT_FAN = HIGH;
    Blynk.virtualWrite(V9, 0);
  }
 
  if (Data_From_Lora.indexOf("ONCLEAN") >=0 ) {
    TT_CLEAN= LOW;
    Blynk.virtualWrite(V12, 1);
  }else if (Data_From_Lora.indexOf("OFFCLEAN") >=0 ) {
    TT_CLEAN = HIGH;
    Blynk.virtualWrite(V12, 0);
  }
    if (Data_From_Lora.indexOf("ONDRAIN") >=0 ) {
    TT_DRAIN= LOW;
    Blynk.virtualWrite(V10, 1);
  }else if (Data_From_Lora.indexOf("OFFDRAIN") >=0 ) {
    TT_DRAIN = HIGH;
    Blynk.virtualWrite(V10, 0);
  } 
      if (Data_From_Lora.indexOf("ONFEED") >=0 ) {
    TT_FEED= LOW;
    Blynk.virtualWrite(V11, 1);
  }else if (Data_From_Lora.indexOf("OFFFEED") >=0 ) {
    TT_FEED = HIGH;
    Blynk.virtualWrite(V11, 0);
  } 
}

void updateColor() {
  if (TT_LAMP==0){
    myHMI.print("Controlmodeoff.b1.bco=64105");
    confirmDisplay();
    myHMI.print("Controlmodeoff.b1.txt=" + String('"') + "ON" + String('"'));
    confirmDisplay();
  }else {
    myHMI.print("Controlmodeoff.b1.bco=1664");
    confirmDisplay();
    myHMI.print("Controlmodeoff.b1.txt=" + String('"') + "OFF" + String('"'));
    confirmDisplay(); 
  }

  if (TT_PUMP==0){
    myHMI.print("Controlmodeoff.b2.bco=64105");
    confirmDisplay();
    myHMI.print("Controlmodeoff.b2.txt=" + String('"') + "ON" + String('"'));
    confirmDisplay();
  }else {
    myHMI.print("Controlmodeoff.b2.bco=1664");
    confirmDisplay();
    myHMI.print("Controlmodeoff.b2.txt=" + String('"') + "OFF" + String('"'));
    confirmDisplay(); 
  }

if (TT_FAN==0){
    myHMI.print("Controlmodeoff.b3.bco=64105");
    confirmDisplay();
    myHMI.print("Controlmodeoff.b3.txt=" + String('"') + "ON" + String('"'));
    confirmDisplay();
  }else {
    myHMI.print("Controlmodeoff.b3.bco=1664");
    confirmDisplay();
    myHMI.print("Controlmodeoff.b3.txt=" + String('"') + "OFF" + String('"'));
    confirmDisplay(); 
  }

if (TT_CLEAN==0){
    myHMI.print("Controlmodeoff.b6.bco=64105");
    confirmDisplay();
    myHMI.print("Controlmodeoff.b6.txt=" + String('"') + "ON" + String('"'));
    confirmDisplay();
  }else {
    myHMI.print("Controlmodeoff.b6.bco=1664");
    confirmDisplay();
    myHMI.print("Controlmodeoff.b6.txt=" + String('"') + "OFF" + String('"'));
    confirmDisplay(); 
  }

if (TT_DRAIN==0){
    myHMI.print("Controlmodeoff.b4.bco=64105");
    confirmDisplay();
    myHMI.print("Controlmodeoff.b4.txt=" + String('"') + "ON" + String('"'));
    confirmDisplay();
  }else {
    myHMI.print("Controlmodeoff.b4.bco=1664");
    confirmDisplay();
    myHMI.print("Controlmodeoff.b4.txt=" + String('"') + "OFF" + String('"'));
    confirmDisplay(); 
  }

  if (TT_FEED==0){
    myHMI.print("Controlmodeoff.b5.bco=64105");
    confirmDisplay();
    myHMI.print("Controlmodeoff.b5.txt=" + String('"') + "ON" + String('"'));
    confirmDisplay();
  }else {
    myHMI.print("Controlmodeoff.b5.bco=1664");
    confirmDisplay();
    myHMI.print("Controlmodeoff.b5.txt=" + String('"') + "OFF" + String('"'));
    confirmDisplay(); 
  }
}

void receiveData() {
   if (e32ttl.available()  > 1){
        ResponseContainer rs = e32ttl.receiveMessage();
        // First of all get the data
        Data_From_Lora = rs.data;
        //Serial.println(rs.status.getResponseDescription());
        Serial.println(Data_From_Lora);
    }
}

void SendMessageEC()
{
  Serial.println ("Sending Message");
  sim.println("AT+CMGF=1");    
  delay(1000);
  sim.println("AT+CMGS=\"" + number + "\"\r"); 
  delay(1000);
  String SMS = "Nong do EC vuot nguong cho phep!";
  sim.println(SMS);
  delay(100);
  sim.println((char)26);
  delay(1000);
  _buffer = _readSerial();
}

void SendMessageDO()
{
  Serial.println ("Sending Message");
  sim.println("AT+CMGF=1");    
  delay(1000);
  sim.println("AT+CMGS=\"" + number + "\"\r"); 
  delay(1000);
  String SMS = "Nong do DO vuot nguong cho phep!";
  sim.println(SMS);
  delay(100);
  sim.println((char)26);
  delay(1000);
  _buffer = _readSerial();
}

void SendMessageTDS()
{
  Serial.println ("Sending Message");
  sim.println("AT+CMGF=1");    
  delay(1000);
  sim.println("AT+CMGS=\"" + number + "\"\r"); 
  delay(1000);
  String SMS = "Nong do TDS vuot nguong cho phep!";
  sim.println(SMS);
  delay(100);
  sim.println((char)26);
  delay(1000);
  _buffer = _readSerial();
}

void SendMessageTSS()
{
  Serial.println ("Sending Message");
  sim.println("AT+CMGF=1");    
  delay(1000);
  sim.println("AT+CMGS=\"" + number + "\"\r"); 
  delay(1000);
  String SMS = "Nong do TSS vuot nguong cho phep!";
  sim.println(SMS);
  delay(100);
  sim.println((char)26);
  delay(1000);
  _buffer = _readSerial();
}

String _readSerial() {
  _timeout = 0;
  while  (!sim.available() && _timeout < 12000  )
  {
    delay(13);
    _timeout++;
  }
  if (sim.available()) {
    return sim.readString();
  }
}

void callNumber() {
  sim.print (F("ATD"));
  sim.print (number);
  sim.print (F(";\r\n"));
  _buffer = _readSerial();
  Serial.println(_buffer);
}
void handleTimerID1(){
  if(led.getValue()) {
    led.off();
  } else {
    led.on();
  }
  if(runMode==1){
    if(floatEC>mucCanhbao){
      if(warningState==0){
        warningState=1;
        Blynk.logEvent("Warning", String("Cảnh báo! nồng độ EC = " + String(floatEC)+" vượt quá ngưỡng cho phép!"));
        Blynk.virtualWrite(STATUSCB,HIGH);
        callNumber();
        delay(100);
        SendMessageEC();
        delay(100);
        Serial.println("Đã bật cảnh báo!");           
        timerID2 = timer.setTimeout(5000L,handleTimerID2);  
        //Serial.println(EC);
      }            
       if (statusEC==0){
          statusEC=1;
            e32ttl.sendFixedMessage(0, 3, 0x04, "TATPUMP");   
        }   
      }
    else {
      statusEC=0;
    }   

    if(floatDO>mucCanhbao){
      if(warningState==0){
        warningState=1;
        Blynk.logEvent("warning", String("Cảnh báo! nồng độ DO = " + String(floatDO)+" vượt quá ngưỡng cho phép!"));
        Blynk.virtualWrite(TRANGTHAICB,HIGH);
        callNumber();
        delay(100);
        SendMessageDO();
        delay(100);
        Serial.println("Đã bật cảnh báo!");
        timerID2 = timer.setTimeout(5000L,handleTimerID2);
      }       
      if (statusDO==0){
          statusDO=1;
            e32ttl.sendFixedMessage(0, 3, 0x04, "TATFAN");   
        }   
      }
    else {
      statusDO=0;
    } 
    

    if(floatTDS>mucCanhbao){
      if(warningState==0){
        warningState=1;
        Blynk.logEvent("warning", String("Cảnh báo! nồng độ TDS = " + String(floatTDS)+" vượt quá ngưỡng cho phép!"));
        Blynk.virtualWrite(TRANGTHAICB,HIGH);
        callNumber();
        delay(100);
        SendMessageTDS();
        delay(100);
        Serial.println("Đã bật cảnh báo!");
        timerID2 = timer.setTimeout(5000L,handleTimerID2);
      }
    }

    if(floatTSS>mucCanhbao){
      if(warningState==0){
        warningState=1;
        Blynk.logEvent("warning", String("Cảnh báo! nồng độ TDS = " + String(floatTSS)+" vượt quá ngưỡng cho phép!"));
        Blynk.virtualWrite(TRANGTHAICB,HIGH);
        callNumber();
        delay(100);
        SendMessageTSS();
        delay(100);
        Serial.println("Đã bật cảnh báo!");
        timerID2 = timer.setTimeout(5000L,handleTimerID2);
      }
    }
 }else{
    Blynk.virtualWrite(TRANGTHAICB,LOW);
    Serial.println("Đã tắt cảnh báo!");
    warningState=0;
    statusEC==0;
    statusDO==0;
  }
}
void handleTimerID2(){
  warningState=0;
  if(floatEC<mucCanhbao){
    Blynk.virtualWrite(TRANGTHAICB,LOW);
    Serial.println("Đã tắt cảnh báo!");
  }
}
BLYNK_CONNECTED() {
  Blynk.syncVirtual(RUNMODE,MUCCANHBAO);
}
BLYNK_WRITE(MUCCANHBAO) {
  mucCanhbao = param.asInt();
  Serial.println("Mức cảnh báo: " + String(mucCanhbao));
}
BLYNK_WRITE(RUNMODE) {
  runMode = param.asInt();
}

//---------------------------------------------
// The loop function is called in an endless loop
void loop()
{
   Blynk.run();
   timer.run();
   checkButtonOnScreen();
   updateButton();
   updateColor();
   updateDataToScreen();
   updateDataToBlynk();
   receiveData();
}
