#ifndef StatusLed_H
#define StatusLed_H

#include <Arduino.h>

class StatusLed
{
public:
    StatusLed(int pinR, int pinG, int pinB);
    void setupPins();
    void changeColor(int Rvalue, int Gvalue, int Bvalue);

private:
    int pinR, pinG, pinB;
};

#endif // StatusLed_H