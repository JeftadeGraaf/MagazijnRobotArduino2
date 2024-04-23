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
    int zValue = map(analogRead(this->zPin), 0, 1023, 0, 255);
    if(zValue >= 127 - this -> deadzone && zValue <= 127 + this-> deadzone){
        return 0;
    } else if(zValue < 127){
        zValue = (127 - zValue) * -1;
    } else {
        zValue = map(zValue, 127, 255, 0, 127);
    }
    return zValue * -1;
}