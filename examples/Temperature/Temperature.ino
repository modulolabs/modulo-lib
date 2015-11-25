#include "Wire.h"
#include "Modulo.h"

TemperatureProbeModulo tempProbe;


void onTemperatureChanged(TemperatureProbeModulo &t) {
    Serial.print(tempProbe.isValid());
    Serial.print(" ");
    Serial.println(tempProbe.getTemperatureC());
}

void setup() {
    Serial.begin(9600);
    tempProbe.setTemperatureChangeCallback(onTemperatureChanged);
}

void loop() {
    Modulo.loop();
}
