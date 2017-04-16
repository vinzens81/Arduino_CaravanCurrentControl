#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

int RELAIS1 = 4;
int RELAIS2 = 5;
int RELAIS3 = 6;
int RELAIS4 = 7;
int sleeptimeer = 500;
int Sensor1 = 0;
int Sensor2 = 1;
int valSensor1 = 0;
int valSensor2 = 0;
int offsetSensor = 102;

const int CurrSens1NumReadings = 10;
int CurrSens1Readings[CurrSens1NumReadings];
int CurrSens1ReadIndex = 0;
int CurrSens1Total = 0;
int CurrSens1Average = 0;
int OldCurrSens1Average = 0;
int OldCurrSens1AverageDiviation = 0;
int CurrSens1InputPin = A0;
long sensor2mA = 0;

const int CurrSens2NumReadings = 10;
int CurrSens2Readings[CurrSens2NumReadings];
int CurrSens2ReadIndex = 0;
int CurrSens2Total = 0;
int CurrSens2Average = 0;
int OldCurrSens2Average = 0;
int OldCurrSens2AverageDiviation = 0;
int CurrSens2InputPin = A1;
long sensor1mA = 0;

const int VoltSens1NumReadings = 10;
int VoltSens1Readings[VoltSens1NumReadings];
int VoltSens1ReadIndex = 0;
int VoltSens1Total = 0;
int VoltSens1Average = 0;
int OldVoltSens1Average = 0;
int OldVoltSens1AverageDiviation = 0;
int VoltSens1InputPin = A2;
float sensor1V = 0;

const int VoltSens2NumReadings = 10;
int VoltSens2Readings[VoltSens2NumReadings];
int VoltSens2ReadIndex = 0;
int VoltSens2Total = 0;
int VoltSens2Average = 0;
int OldVoltSens2Average = 0;
int OldVoltSens2AverageDiviation = 0;
int VoltSens2InputPin = A3;
float sensor2V = 0;

bool motorRunning = false;
long motorOffDelay = 1200000;
long motorRunningDelay = 300000;
int lowCurrent = 5800;
int voltRunningMotor = 12;

void setup()
{
  pinMode(RELAIS1, OUTPUT);
  pinMode(RELAIS2, OUTPUT);
  pinMode(RELAIS3, OUTPUT);
  pinMode(RELAIS4, OUTPUT);
  Serial.begin(9600);


  //lcd.begin();

  // Turn on the blacklight and print a message.
  //lcd.backlight();
  //lcd.print("Hello, world!");
  //lcd.clear();
  Serial.println("Hallo");
}
void loop()
{
  // Check if motor Running
  for ( int i; i < 10; i++) {
    readAnalogPortsVolt1();
    delay(100);
  }

  if (sensor1V > voltRunningMotor) {
    Serial.println("We assume the motor is running...");
    motorRunning = true;
  } else {
    digitalWrite(RELAIS1, LOW);
    motorRunning = false;
    Serial.println("Motor Not Running");
  }

  if ( motorRunning == true ) {
    digitalWrite(RELAIS1, HIGH);

    for ( int i; i < 10; i++) {
      // Get the Average right... Starting far to low.
      readAnalogPortsCurrent1();
      Serial.print(sensor1mA); Serial.println(" mA Sensor 1");
      delay(100);
    }
    for (;;) {
      readAnalogPortsCurrent1();
      Serial.print(sensor1mA); Serial.println(" mA Sensor 1");
      if (sensor1mA < lowCurrent) {
        Serial.println("low on current break the innter loop");
        digitalWrite(RELAIS1, LOW);
        break;
      }
      delay(500);
    }
    digitalWrite(RELAIS1, LOW);
    Serial.print("Wait "); Serial.print(motorRunningDelay / 1000); Serial.println(" sec for next loop");
    delay(motorRunningDelay);
  }
  else {
    Serial.println("Motor not Running no need to do anything... Sleep for a while...");
    delay(motorOffDelay);
  }
}


void readAnalogPortsCurrent1() {
  CurrSens1Total = CurrSens1Total - CurrSens1Readings[CurrSens1ReadIndex];
  CurrSens1Readings[CurrSens1ReadIndex] = analogRead(CurrSens1InputPin);
  CurrSens1Total = CurrSens1Total + CurrSens1Readings[CurrSens1ReadIndex];
  CurrSens1ReadIndex = CurrSens1ReadIndex + 1;
  if (CurrSens1ReadIndex >= CurrSens1NumReadings) {
    CurrSens1ReadIndex = 0;
  }
  OldCurrSens1Average = CurrSens1Average;
  CurrSens1Average = CurrSens1Total / CurrSens1NumReadings;
  OldCurrSens1AverageDiviation = OldCurrSens1Average - CurrSens1Average;
  if (OldCurrSens1AverageDiviation < 0) {
    OldCurrSens1AverageDiviation = OldCurrSens1AverageDiviation * -1;
  }
  sensor1mA = ((((long)CurrSens1Average * 5000 / 1024) - 498 ) * 1000 / 133);
}
void readAnalogPortsCurrent2() {
  CurrSens2Total = CurrSens2Total - CurrSens2Readings[CurrSens2ReadIndex];
  CurrSens2Readings[CurrSens2ReadIndex] = analogRead(CurrSens2InputPin);
  CurrSens2Total = CurrSens2Total + CurrSens2Readings[CurrSens2ReadIndex];
  CurrSens2ReadIndex = CurrSens2ReadIndex + 1;
  if (CurrSens2ReadIndex >= CurrSens2NumReadings) {
    CurrSens2ReadIndex = 0;
  }
  OldCurrSens2Average = CurrSens2Average;
  CurrSens2Average = CurrSens2Total / CurrSens2NumReadings;
  OldCurrSens2AverageDiviation = OldCurrSens2Average - CurrSens2Average;
  if (OldCurrSens2AverageDiviation < 0) {
    OldCurrSens2AverageDiviation = OldCurrSens2AverageDiviation * -1;
  }
  sensor2mA = ((((long)CurrSens2Average * 5000 / 1024) - 498 ) * 1000 / 133);
}

void readAnalogPortsVolt2() {
  VoltSens2Total = VoltSens2Total - VoltSens2Readings[VoltSens2ReadIndex];
  VoltSens2Readings[VoltSens2ReadIndex] = analogRead(VoltSens2InputPin);
  VoltSens2Total = VoltSens2Total + VoltSens2Readings[VoltSens2ReadIndex];
  VoltSens2ReadIndex = VoltSens2ReadIndex + 1;
  if (VoltSens2ReadIndex >= VoltSens2NumReadings) {
    VoltSens2ReadIndex = 0;
  }
  OldVoltSens2Average = VoltSens2Average;
  VoltSens2Average = VoltSens2Total / VoltSens2NumReadings;
  OldVoltSens2AverageDiviation = OldVoltSens2Average - VoltSens2Average;
  if (OldVoltSens2AverageDiviation < 0) {
    OldVoltSens2AverageDiviation = OldVoltSens2AverageDiviation * -1;
  }
  sensor2V = (5.0 / 1024 * VoltSens2Average) + 10.3;
}
void readAnalogPortsVolt1() {
  VoltSens1Total = VoltSens1Total - VoltSens1Readings[VoltSens1ReadIndex];
  VoltSens1Readings[VoltSens1ReadIndex] = analogRead(VoltSens1InputPin);
  VoltSens1Total = VoltSens1Total + VoltSens1Readings[VoltSens1ReadIndex];
  VoltSens1ReadIndex = VoltSens1ReadIndex + 1;
  if (VoltSens1ReadIndex >= VoltSens1NumReadings) {
    VoltSens1ReadIndex = 0;
  }
  OldVoltSens1Average = VoltSens1Average;
  VoltSens1Average = VoltSens1Total / VoltSens1NumReadings;
  OldVoltSens1AverageDiviation = OldVoltSens1Average - VoltSens1Average;
  if (OldVoltSens1AverageDiviation < 0) {
    OldVoltSens1AverageDiviation = OldVoltSens1AverageDiviation * -1;
  }
  sensor1V = (5.0 / 1024 * VoltSens1Average) + 10.3;
}
