#include "Managers/HardwareManager.h"

HardwareManager::HardwareManager(uint8_t ledPin, uint8_t buttonPin)
    : _ledPin(ledPin)
    , _buttonPin(buttonPin)
    , _ledState(false)
    , _lastButtonState(false)
    , _lastButtonCheck(0)
    , _bootTime(0)
    , _buttonCallback(nullptr) {}

void HardwareManager::begin() {
    pinMode(_ledPin, OUTPUT);
    digitalWrite(_ledPin, LOW);
    _ledState = false;
    
    pinMode(_buttonPin, INPUT_PULLUP);
    _lastButtonState = isButtonPressed();
    
    _bootTime = millis();
    _lastButtonCheck = millis();
    
    Serial.printf("[HW] LED on pin %d, Button on pin %d\n", _ledPin, _buttonPin);
}

void HardwareManager::setLed(bool on) {
    _ledState = on;
    digitalWrite(_ledPin, on ? HIGH : LOW);
    Serial.printf("[HW] LED %s\n", on ? "ON" : "OFF");
}

bool HardwareManager::toggleLed() {
    setLed(!_ledState);
    return _ledState;
}

bool HardwareManager::isButtonPressed() const {
    // BOOT button is active LOW (inverted logic)
    return digitalRead(_buttonPin) == LOW;
}

bool HardwareManager::update() {
    if (millis() - _lastButtonCheck < BUTTON_DEBOUNCE_MS) {
        return false;
    }
    
    _lastButtonCheck = millis();
    bool currentState = isButtonPressed();
    
    if (currentState != _lastButtonState) {
        _lastButtonState = currentState;
        Serial.printf("[HW] Button %s\n", currentState ? "PRESSED" : "RELEASED");
        
        if (_buttonCallback) {
            _buttonCallback(currentState);
        }
        return true;
    }
    
    return false;
}
