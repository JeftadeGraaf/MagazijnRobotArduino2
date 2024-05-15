#include <Arduino.h>
#include "Src/MotorModule/Motor.h"
#include "Src/StatusLed/StatusLed.h"
#include "Src/JoystickModule/Joystick.h"
#include <Wire.h>

#define firstArduinoAddress 8
#define fallSwitch A3               ///white vcc, red connection, brown ground
#define zAxisBackSwitch 4
#define rotatyPinYa 2
#define rotatyPinYb 9
#define rotatyPinZa 3
#define rotatyPinZb 7
#define yAxisSwitchUp 12
#define yAxisSwitchDown 14
#define allowedYMovement 200
bool isFalling = false;
bool tYSwitch = false;
bool bYSwitch = false;
bool callibrate = true;
bool zAxisIsOut = true;
bool callibrationDone = true;

unsigned long lastRequestTime = 0;
unsigned long lastReportTime = 0;

int possitionY = 0;
int possitionZ = 0;
int startY = 0;

Motor z_axisMotor = Motor(11, 13, 8, A1);
Joystick joystick = Joystick(A2, 30);
StatusLed statusLed = StatusLed(10,6,5);

enum RobotState{
  automatic,
  manual,
  off,
  callibrating
};

RobotState currentState = manual;

void setup()
{
  pinMode(rotatyPinYa, INPUT);
  pinMode(rotatyPinYb, INPUT);
  attachInterrupt(digitalPinToInterrupt(rotatyPinYa), readRotartyY, RISING);
  attachInterrupt(digitalPinToInterrupt(rotatyPinZa), readRotartyZ, RISING);
  pinMode(yAxisSwitchUp, INPUT_PULLUP);
  pinMode(yAxisSwitchDown, INPUT_PULLUP);
  Serial.begin(9600);
  pinMode(fallSwitch, INPUT);
  pinMode(zAxisBackSwitch, INPUT_PULLUP);
  zAxisIsOut = digitalRead(zAxisBackSwitch);
  bYSwitch = digitalRead(yAxisSwitchDown);
  tYSwitch = digitalRead(yAxisSwitchUp);
  z_axisMotor.registerPins();
  Wire.begin(9);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  statusLed.setupPins();
  lastRequestTime = millis();
}


void loop()
{
  if (millis() - lastRequestTime > 1300) {
    turnRobotOff();
  }

  if (millis() - lastReportTime > 65) {
    sendMessage(firstArduinoAddress, "py" + String(possitionY));
    lastReportTime = millis();
  }
  
  handleEndOfAxisDetection();

  switch (currentState){
        case automatic:
            checkForFalling();
            break;
        case manual:
            checkForFalling();
            if(!isFalling){
              handleManualInput();
            }
            break;
        case off:
            return;
            break;
        case callibrating:
            callibrateMotor();
            break;
        default:
            currentState = off;
            return;
            break;
  }
}

void receiveEvent(int bytes){
  String msg = "";
  while (Wire.available() > 0) {
    msg = msg + char(Wire.read());
  }
  if(msg == "off"){
    turnRobotOff();
  } else if (msg == "man"){
    switchToManualMode();
  } else if (msg == "aut"){
    switchToAutomaticMode();
  } else if (msg == "cal") {
    switchToCallibrateMode();
  } else if (msg == "dcal") {
    callibrationDone = true;
  }
}

void requestEvent(){
  Wire.write("u");
  lastRequestTime = millis();
}

void sendMessage(int address, String msg){
    Wire.beginTransmission(address);
    Wire.write(msg.c_str());
    Wire.endTransmission();
}

void handleManualInput(){
  int zValue = joystick.readZAxis();
  if((zValue == 0 || !zAxisIsOut && zValue > 0) || zAxisIsOut){
    z_axisMotor.setManualPower(zValue);
  } else {
    z_axisMotor.setManualPower(0);
  }  
}

void turnRobotOff(){
  currentState = off;
  statusLed.changeColor(255,0,0);
  z_axisMotor.setManualPower(0);
}

void switchToManualMode(){
  currentState = manual;
  statusLed.changeColor(255,15,0);
}

void switchToAutomaticMode(){
  currentState = automatic;
  statusLed.changeColor(0,255,0);
}

void switchToCallibrateMode() {
  currentState = callibrating;
  callibrationDone = false;
  statusLed.changeColor(0,0,255);

}

void checkForFalling(){
  if(!isFalling){
    if(analogRead(fallSwitch) < 100){
      isFalling = true; 
    }
  } else {
    if(zAxisIsOut){
      z_axisMotor.setManualPower(-127);
    } else {
      z_axisMotor.setManualPower(0);
      isFalling = false;
      turnRobotOff();
      Serial.println("turning off");
      sendMessage(firstArduinoAddress, "off");
    }
  }
}

void handleEndOfAxisDetection(){
  if(digitalRead(zAxisBackSwitch) != zAxisIsOut){
    if(zAxisIsOut){
      sendMessage(firstArduinoAddress, "mz0");
    } else {
      sendMessage(firstArduinoAddress, "mz1");
    }
  }
  if (digitalRead(yAxisSwitchUp) != tYSwitch) {
    if (!tYSwitch) {
      sendMessage(firstArduinoAddress, "my0h");
    } else {
      sendMessage(firstArduinoAddress, "my1h");
    }
  }
  if (digitalRead(yAxisSwitchDown) != bYSwitch) {
    if (!bYSwitch) {
      sendMessage(firstArduinoAddress, "my0l");
    } else {
      sendMessage(firstArduinoAddress, "my1l");
    }
  }
  
  
  zAxisIsOut = digitalRead(zAxisBackSwitch);
  tYSwitch = digitalRead(yAxisSwitchUp);
  bYSwitch = digitalRead(yAxisSwitchDown);
}

void readRotartyY(){
  if (digitalRead(rotatyPinYb)) {
    possitionY++;
  } else {
    possitionY--;
  }
}

void readRotartyZ(){
  if (digitalRead(rotatyPinZb)) {
    possitionZ++;
  } else {
    possitionZ--;
  }
}

void callibrateMotor() {
  if (digitalRead(zAxisBackSwitch)) {
    z_axisMotor.setManualPower(-255);
  } else {
    z_axisMotor.setManualPower(0);
    possitionZ = 0;
    sendMessage(firstArduinoAddress, "mz0");
  }
  if (!digitalRead(yAxisSwitchDown)) {
    sendMessage(firstArduinoAddress, "my1l");
    possitionY = 0;
  }  

  if (!digitalRead(yAxisSwitchDown) && !digitalRead(zAxisBackSwitch) && callibrationDone) {
    switchToManualMode();
  }
  
}