#include "Wire.h"
#include "Modulo.h"

ControllerModule controller;

void setup() {
    Serial.begin(9600);
}

void loop() {
    Serial.println(controller.readTemperatureProbe(0));
}