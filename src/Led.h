#ifndef LED_H
#define LED_H

#include <Arduino.h>

class Led {
public:
    Led(int blueLed, int yellowLed);
    void begin() const;
    void wifiConnecting() const;
    void loop(const String& state);

private:
    int _blueLed;
    int _yellowLed;
    unsigned long _previousMillis;
    bool _ledState;
};

#endif // LED_H
