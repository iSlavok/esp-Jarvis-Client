#include "Led.h"

Led::Led(const int blueLed, const int yellowLed)
        : _blueLed(blueLed), _yellowLed(yellowLed), _previousMillis(0), _ledState(false) {}

void Led::begin() const {
    pinMode(_blueLed, OUTPUT);
    pinMode(_yellowLed, OUTPUT);
    digitalWrite(_blueLed, LOW);
    digitalWrite(_yellowLed, LOW);
}

void Led::wifiConnecting() const {
    digitalWrite(_blueLed, HIGH);
    digitalWrite(_yellowLed, LOW);
    delay(150);
    digitalWrite(_blueLed, LOW);
    digitalWrite(_yellowLed, HIGH);
    delay(150);
}

void Led::loop(const String& state) {
    const unsigned long currentMillis = millis();

    if (state == "waiting") {
        digitalWrite(_blueLed, HIGH);
        digitalWrite(_yellowLed, LOW);
    } else if (state == "recording") {
        digitalWrite(_blueLed, LOW);
        digitalWrite(_yellowLed, HIGH);
    } else if (state == "responding") {
        if (currentMillis - _previousMillis >= 500) {
            _previousMillis = currentMillis;
            _ledState = !_ledState;
            digitalWrite(_blueLed, _ledState ? HIGH : LOW);
        }
        digitalWrite(_yellowLed, LOW);
    } else if (state == "speaking") {
        if (currentMillis - _previousMillis >= 500) {
            _previousMillis = currentMillis;
            _ledState = !_ledState;
            digitalWrite(_yellowLed, _ledState ? HIGH : LOW);
        }
        digitalWrite(_blueLed, LOW);
    } else {
        digitalWrite(_blueLed, LOW);
        digitalWrite(_yellowLed, LOW);
    }
}
