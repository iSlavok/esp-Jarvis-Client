#include "Button.h"

Button::Button(const int pin, const std::function<void(int)> &callback)
        : _pin(pin), _callback(callback), _lastPressTime(0), _isHolding(false), _lastState(HIGH) {}

void Button::begin() const {
    pinMode(_pin, INPUT_PULLUP);
}

void Button::loop() {
    const bool currentState = digitalRead(_pin);
    const unsigned long currentMillis = millis();

    if (_lastState == HIGH && currentState == LOW) {
        _lastPressTime = currentMillis;
        _isHolding = false;
        _callback(0);
    } else if (_lastState == LOW && currentState == LOW) {
        if (!_isHolding && currentMillis - _lastPressTime >= 1000) {
            _isHolding = true;
            _callback(1);
        }
    } else if (_lastState == LOW && currentState == HIGH) {
        if (_isHolding) {
            _callback(-1);
        }
        _isHolding = false;
    }

    _lastState = currentState;
}
