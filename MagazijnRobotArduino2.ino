#include <Arduino.h>
#include "Src/MotorModule/Motor.h"
#include "Src/StatusLed/StatusLed.h"
#include <Wire.h>

Motor z_axisMotor = Motor(3, 12);
StatusLed statusLed = StatusLed(11,6,5);
int x = 0;

void setup()
{
    Serial.begin(9600);
    z_axisMotor.registerPins();
    Wire.begin(9);
    Wire.onReceive(receiveEvent);
    statusLed.setupPins();
}

void loop()
{
}

void receiveEvent(int bytes){
  String msg = "";
  while (Wire.available() > 0) {
    msg = msg + char(Wire.read());
  }

  if(msg == "off"){
    statusLed.changeColor(255,0,0);
  } else if (msg == "man"){
    statusLed.changeColor(255,140,0);
  } else if (msg == "aut"){
    statusLed.changeColor(0,255,0);
  }
}