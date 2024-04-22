#include <Arduino.h>
#include "Src/MotorModule/Motor.h"
#include "Src/StatusLed/StatusLed.h"
#include "Src/JoystickModule/Joystick.h"
#include <Wire.h>

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
    z_axisMotor.registerPins();
    Wire.begin(9);
    Wire.onReceive(receiveEvent);
    statusLed.setupPins();
}

void loop()
{
  switch (currentState){
        case automatic:
            break;
        case manual:
            handleManualInput();
            break;
        case off:
            break;
        default:
            currentState = off;
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