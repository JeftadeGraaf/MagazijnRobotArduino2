#include <Arduino.h>
#include "StatusLed.h"


StatusLed::StatusLed(int pinR, int pinG, int pinB){
    this-> pinR = pinR;
    this-> pinG = pinG;
    this-> pinB = pinB;
}

void StatusLed::setupPins(){
    pinMode(this->pinR, OUTPUT);
    pinMode(this->pinG, OUTPUT);
    pinMode(this->pinB, OUTPUT);
}

void StatusLed::changeColor(int Rvalue, int Gvalue, int Bvalue){
    analogWrite(this->pinR, Rvalue);
    analogWrite(this->pinG, Gvalue);
    analogWrite(this->pinB, Bvalue);
}