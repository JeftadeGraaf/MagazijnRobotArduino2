#ifndef JOYSTICK_H
#define JOYSTICK_H

class Joystick
{

public:

    Joystick(int zPin, int deadzone);
    void registerPins();
    int readZAxis();


private:
    int zPin;
    int deadzone;
};

#endif // JOYSTICK_H