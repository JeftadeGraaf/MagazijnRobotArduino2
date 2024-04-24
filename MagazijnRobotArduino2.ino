#include <Arduino.h>
#include "Src/MotorModule/Motor.h"
#include "Src/StatusLed/StatusLed.h"
#include "Src/JoystickModule/Joystick.h"
#include <Wire.h>

#define firstArduinoAddress 8
#define fallSwitch A3               ///white vcc, red connection, brown ground
#define zAxisBackSwitch 4
bool zAxisIsOut;
bool isFalling = false;

Motor z_axisMotor = Motor(11, 13, 8, A1);
Joystick joystick = Joystick(A2, 30);
StatusLed statusLed = StatusLed(10,6,5);

enum RobotState{
  automatic,
  manual,
  off
};

RobotState currentState = manual;

void setup()
{
  Serial.begin(9600);
  pinMode(fallSwitch, INPUT);
  pinMode(zAxisBackSwitch, INPUT_PULLUP);
  zAxisIsOut = digitalRead(zAxisBackSwitch);
  z_axisMotor.registerPins();
  Wire.begin(9);
  Wire.onReceive(receiveEvent);
  statusLed.setupPins();
}


void loop()
{
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
  }
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

void checkForFalling(){
  if(!isFalling){
    if(analogRead(fallSwitch) < 100){
      isFalling = true; 
    }
  } else {
    if(analogRead(fallSwitch) < 100 && zAxisIsOut){
      z_axisMotor.setManualPower(-127);
    } else {
      z_axisMotor.setManualPower(0);
      isFalling = false;
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
  zAxisIsOut = digitalRead(zAxisBackSwitch);
}