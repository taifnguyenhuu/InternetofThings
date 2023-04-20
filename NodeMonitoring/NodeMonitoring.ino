#include <EEPROM.h> 
#include <OneWire.h> 
#include <DallasTemperature.h> 
#include <SoftwareSerial.h>
#include "OneButton.h" 
#include "LoRa_E32.h" 
int START = 8; 
int STOP = 9;
OneButton BTNSTART(START, true); 
OneButton BTNSTOP(STOP, true);                          

int LED_START = 3; 
int LED_STOP  = 2;
int LED_TT = 4;
int valueDelay = 1000;
//------------------
LoRa_E32 e32ttl(6, 7);
#define pHSensor A0
#define ECSensor A1
#define DOSensor A2
#define TDSSensor A3
#define TSSSensor A4
#define ONE_WIRE_BUS 5 
#define W002Sensor 12


float PHValue = 7.55 ; 
float ECValue; 
float DOValue;
float TDSValue; 
float TSSValue; 
float calibration_value = 0.7; 
float TempValue;
unsigned long int avgval;
int buffer_arr[10], temp;
String Result = ""; 
byte W002Value; 
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature TempSensor(&oneWire);
void setup()
{
  Serial.begin(9600); 
  while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB
  }
  e32ttl.begin();
  TempSensor.begin(); 
  pinMode(ECSensor, INPUT);
  pinMode(DOSensor, INPUT);
  pinMode(TDSSensor, INPUT);
  pinMode(TSSSensor, INPUT);
  pinMode(LED_START, OUTPUT);                                                   
  pinMode(LED_STOP,OUTPUT);
  pinMode(LED_TT,OUTPUT);         
  BTNSTART.attachClick(START);  
  BTNSTOP.attachClick(STOP);         
  digitalWrite(LED_START, LOW);
  digitalWrite(LED_STOP, HIGH);
  digitalWrite(LED_TT, LOW);
  delay(100);
}
void press_START() 
{  
 TT_LED_START = !TT_LED_START;  
 digitalWrite(LED_START,TT_LED_START);                                                                                                
} 
void press_STOP() 
{  
  if (TT_LED_START == 0) {
    //Serial.println("STOP");
      TT_LED_START = !TT_LED_START;
      digitalWrite(LED_START,TT_LED_START);
      digitalWrite(LED_STOP, LOW); 
      digitalWrite(LED_TT, HIGH);        
  }                                                                                           
}
void status() {
if (TT_LED_START == 0) {
    Serial.println("START"); 
    digitalWrite(LED_START, LOW);
    digitalWrite(LED_STOP, HIGH);
    digitalWrite(LED_TT, LOW);
    SendData();                   
  }else {
    Serial.println("STOP");
    digitalWrite(LED_STOP, LOW);
    digitalWrite(LED_TT, HIGH); 
    ResponseStatus rs = e32ttl.sendFixedMessage(0, 2, 0x04, "0.0;0.0;0.0;0.0;0.0;0.0;"); 
    delay(1000);
  }      
}*/


//--------------

void ReadSensorDO() {
  int readValDO = analogRead(DOSensor);
  DOValue = (5./1023) * readValDO;
  Serial.print("DO = ");
  Serial.println(DOValue);
  delay(100);
}

void ReadSensorEC() {
  int readValEC = analogRead(ECSensor);
  ECValue = (5./1023) * readValEC;
  Serial.print("EC = ");
  Serial.println(ECValue);
  delay(100);
}

void ReadSensorPH() {
  for (int i = 0; i < 10; i++)
  {
    buffer_arr[i] = analogRead(pHSensor);
    delay(30);
  }
  for (int i = 0; i < 9; i++)
    for (int j = i + 1; j < 10; j++)
      if (buffer_arr[i] > buffer_arr[j])
      {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
  avgval = 0;
  for (int i = 2; i < 8; i++)
    avgval += buffer_arr[i];
  float volt = (float)avgval * 5.0 / 1024 / 6;
  PHValue = 2. * volt + calibration_value;
  Serial.print("pH = ");
  Serial.println(PHValue);
  delay(100);
}

void ReadSensorTDS() {
  int readValTDS = analogRead(TDSSensor);
  TDSValue = (5./1023) * readValTDS;
  Serial.print("TDS = ");
  Serial.println(TDSValue);
  delay(100);
}

void ReadSensorTSS() {
  int readValTSS = analogRead(TSSSensor);
  TSSValue = (5./1023) * readValTSS;
  Serial.print("TDS = ");
  Serial.println(TSSValue);
  delay(100);
}

void ReadSensorTemp() {
  TempSensor.requestTemperatures();
  TempValue = TempSensor.getTempCByIndex(0);
  Serial.print("Temperature: ");
  Serial.println(TempValue);
  delay(100);
}
//-------------------
void SendData() {
  Result = String(PHValue) + ";" + String("27.5") + ";" + String(ECValue) + ";" + String(DOValue) + ";" + String(TDSValue) + ";" + String(TSSValue)+ ";";
  Serial.println(Result); 
  ResponseStatus rs = e32ttl.sendFixedMessage(0, 2, 0x04, Result);
  Serial.println(rs.getResponseDescription());
  delay(10000);
}
void loop() {
  // put your main code here, to run repeatedly
  BTNSTART.tick(); 
  BTNSTOP.tick(); 
  status();
  ReadSensorPH();
  ReadSensorTemp();
  ReadSensorEC();
  ReadSensorDO();
  ReadSensorTDS();
  ReadSensorTSS();
  SendData();                       
}  
