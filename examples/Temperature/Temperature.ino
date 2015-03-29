#include "Wire.h"
#include "Modulo.h"
#include <avr/io.h>

MiniDisplayModule display;

class TemperatureSensor {
public:
    TemperatureSensor(int pin) : _pin(pin), _lastUpdate(0), _sum(0), _duration(0), _temp(0) {
        // Use the internal 1.1V reference
        analogReference(INTERNAL);
        pinMode(pin, INPUT_PULLUP);
    }

    void loop() {
        // Update at most once every 100ms
        float duration = millis()-_lastUpdate;
        if (duration < 100) {
            return;
        }
        _lastUpdate = millis();

        float val = analogRead(_pin);

        // If the value is maxed out, that means no probe is connected
        if (val == 1023) {
            return;
        }

        // Convert the ADC value to temperature
        float newValue = (val*2.56/1023.0 - .5)/.01;

        // The input is pretty noisy. Keep a running average to reduce that.
        _sum += duration*newValue;
        _duration += duration;

        // Only keep the last 10 seconds
        if (_duration > 10000) {
            _sum *= 10000/_duration;
            _duration = 10000;
        }

        // Reduce noise a bit further with a little hysterisis
        if (fabs(_sum/_duration - _temp) > .25) {
            _temp = _sum/_duration;
        }
    }

    float getTemperature() {
        return _temp;
    }

private:
    int _pin;
    long _lastUpdate;
    float _sum;
    float _duration;
    float _temp;

};

TemperatureSensor temp(1);

void setup() {
    Serial.begin(9600);
    ModuloSetup();
    //controller.setPWMOutput(0, .2);
    analogWrite(0, 128);
    pinMode(0, OUTPUT);

}

void loop() {
    ModuloLoop();
    temp.loop();

    display.clear();
    display.println(temp.getTemperature());
    display.refresh();
}