#include <Arduino.h>
#include "Joystick.h"


Joystick::Joystick(int zPin, int deadzone){
    this -> zPin = zPin;
    this -> deadzone = deadzone;
}

void Joystick::registerPins(){
    pinMode(this->zPin, INPUT);
}

int Joystick::readZAxis(){
    int xValue = map(analogRead(this->zPin), 0, 1023, 0, 255);
    if(xValue >= 127 - this -> deadzone && xValue <= 127 + this-> deadzone){
        return 0;
    } else if(xValue < 127){
        xValue = (127 - xValue) * -1;
    } else {
        xValue = map(xValue, 127, 255, 0, 127);
    }
    return xValue;
}