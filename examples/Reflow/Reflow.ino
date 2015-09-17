
#include "Modulo.h"
#include "Wire.h"

DisplayModulo display;
KnobModulo knob;
ThermocoupleModulo thermocouple;

void setup() {
    // put your setup code here, to run once:
    knob.setColor(0,0,0);
}

enum Phase {
    IdlePhase,
    PreheatPhase,
    SoakPhase,
    ReflowPhase,
    PeakPhase,
    CoolingPhase
};

Phase phase = IdlePhase;

float preheatTemp = 130;
float soakDuration = 45;
float liquidusTemp = 180;
float peakTemp = 215;
float peakDuration = 30;
float cooldownTemp = 50;

float preheatHue = .2;
float reflowSolidHue = .1;
float reflowLiquidHue = 0;
float coolingHue = -.3;

long startTime = 0;
float phaseStartTime = 0;
int targetTemp = 0;

char * phaseName = "Idle";
bool heatingElementOn = false;

void start() {
    startTime = millis()/1000;
    phase = PreheatPhase;
    targetTemp = preheatTemp;
    knob.setHSV(preheatHue, 1, 1);
    phaseName = "Pre-Heat";
}

void stop() {
    phase = IdlePhase;
    phaseName = "Idle";
    targetTemp = 0;
}
void update() {

    int graphMinTemp = 0;
    int graphMaxTemp = 250;
    int graphMinY = 30;
    int graphMaxY = 10;

    float currentTime = millis()/1000.0-startTime;
    float currentTemp = thermocouple.getTemperatureC();
    //currentTemp = knob.getPosition()*10;

    switch (phase) {
        case PreheatPhase:
            // When we get to the preheatTemp, switch to Soak
            if (currentTemp >= preheatTemp) {
                phase = SoakPhase;
                phaseName = "Soak";
                phaseStartTime = currentTime;
                targetTemp = preheatTemp;
            }
            break;
        case SoakPhase:
            // Once the soak duration has expired, switch to Reflow
            if (currentTime >= phaseStartTime+soakDuration) {
                phase = ReflowPhase;
                phaseName = "Reflow";
                targetTemp = peakTemp;
            }
            break;
        case ReflowPhase :
            // Upon reaching the peak temperature, switch to Peak
            if (currentTemp >= peakTemp) {
                phase = PeakPhase;
                phaseName = "Peak";
                targetTemp = peakTemp;
                phaseStartTime = currentTime;
            }
            break;
        case PeakPhase :
            // After the peak duration has expired, cool down
            if (currentTime >= phaseStartTime+peakDuration) {
                phaseName = "Cooling";
                targetTemp = cooldownTemp;
                phase = CoolingPhase;
            }
            break;
        case CoolingPhase:
            // Cool down until the cooldown temperature is reached
            if (currentTemp <= cooldownTemp) {
                phase = IdlePhase;
                phaseName = "Idle";
            }
            break;
        case IdlePhase:
            break;
    }

    if (phase == CoolingPhase || phase == IdlePhase) {
        heatingElementOn = false;
    } else {
        heatingElementOn = (currentTemp < targetTemp);
    }

    digitalWrite(0, heatingElementOn);

    if (phase == IdlePhase) {
        knob.setColor(0,0,0);
    } else if (currentTemp >= liquidusTemp) {
        knob.setHSV(reflowLiquidHue, 1, 1);
    } else if (currentTemp >= preheatTemp) {
        knob.setHSV(reflowSolidHue, 1, 1);
    } else {
        knob.setHSV(preheatHue, 1, 1);
    }

    //display.fillRect(0, 0, display.width(), graphMaxY, 0); // Clear the title area
    //display.fillRect(0, graphMinY, display.width(), display.height(), 0); // Clear the bottom info area

    display.clear();
    display.setTextSize(2);
    display.setCursor((display.width()-strlen(phaseName)*12)/2,0);
    display.println(phaseName);

    display.setTextSize(1);

    display.println();

    switch (phase) {
        case PreheatPhase:
        case ReflowPhase:
            display.print("Heating to: ");
            display.println(targetTemp);
            break;
        case CoolingPhase:
            display.print("Cooling to: ");
            display.println(targetTemp);
            break;
        case SoakPhase:
            display.print("Holding for ");
            display.print(soakDuration-(currentTime-phaseStartTime));
            display.println(" sec");
            break;
        case PeakPhase:
            display.print("Holding for ");
            display.print(peakDuration-(currentTime-phaseStartTime));
            display.println(" sec");
            break;
        default:
            display.println("Waiting...");
            break;
    }

    display.print("Heating Element: ");
    display.println(heatingElementOn ? "On" : "Off");
    display.print("Temperature: ");
    if (currentTemp == thermocouple.InvalidTemperature) {
        display.println("---");
    } else {
        display.println(currentTemp);
    }

    display.println();

    if (phase == IdlePhase) {
        display.println("     Start");
    } else {
        display.println("     Stop");
    }
    //display.fillRect(0, 60, display.width()*(currentTime/totalDuration), 3, 1);
    display.refresh();
}

bool button0WasPressed = false;
bool button1WasPressed = false;
bool button2WasPressed = false;

void loop() {
    Modulo.loop();

    bool button0IsPressed = display.getButton(0);
    bool button1IsPressed = display.getButton(1);
    bool button2IsPressed = display.getButton(2);

    switch (phase) {
        case IdlePhase:
            if (button1IsPressed && !button1WasPressed) {
                start();
            }
            break;
        default:
            if (button1IsPressed && !button1WasPressed) {
                stop();
            }
            break;
    }

    button0WasPressed = button0IsPressed;
    button1WasPressed = button1IsPressed;
    button2WasPressed = button2IsPressed;

    update();
}
