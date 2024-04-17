#include <Arduino.h>
#include "Src/MotorModule/Motor.h"

Motor z_axisMotor = Motor(3, 12);

void setup()
{
    Serial.begin(9600);
    z_axisMotor.registerPins();
}

void loop()
{
}