#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include <functional>

class Button {
public:
    Button(int pin, const std::function<void(int)> &callback);
    void begin() const;
    void loop();

private:
    int _pin;
    std::function<void(int)> _callback;
    unsigned long _lastPressTime;
    bool _isHolding;
    bool _lastState;
};

#endif // BUTTON_H
