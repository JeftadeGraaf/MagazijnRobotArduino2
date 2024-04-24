#include <Arduino.h>
#include "Src/MotorModule/Motor.h"
#include "Src/StatusLed/StatusLed.h"
#include "Src/JoystickModule/Joystick.h"
#include <Wire.h>

#define firstArduinoAddress 8
#define fallSwitch A3               ///white vcc, red connection, brown ground
bool isFalling = false;
unsigned long fallTimer = 0;

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
    z_axisMotor.registerPins();
    Wire.begin(9);
    Wire.onReceive(receiveEvent);
    statusLed.setupPins();
}


void loop()

{
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
            break;
        default:
            currentState = off;
            break;
    }
}

void receiveEvent(int bytes){
  Serial.println("ReceiveEvent");
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
  Serial.println(msg);
}

void sendMessage(int address, String msg){
    Wire.beginTransmission(address);
    Wire.write(msg.c_str());
    Wire.endTransmission();
    Serial.println("message sent");
    Serial.println(msg);
}

void handleManualInput(){
  int zValue = joystick.readZAxis();
  z_axisMotor.setManualPower(zValue);
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
  Serial.println(analogRead(fallSwitch));
  if(!isFalling){
    if(analogRead(fallSwitch) < 100){
      isFalling = true; 
      fallTimer = millis();
    }
  } else {
    if(analogRead(fallSwitch) < 100 && (millis() - fallTimer <= 1000)){
      z_axisMotor.setManualPower(-127);
    } else {
      z_axisMotor.setManualPower(0);
      sendMessage(firstArduinoAddress, "off");
    }
  }
}