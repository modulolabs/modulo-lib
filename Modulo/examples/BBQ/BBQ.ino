#include "Wire.h"
#include "Modulo.h"
#include <PID_v1.h>

ColorDisplayModule display;
ThermocoupleModule thermocouple;
KnobModule knob;

ColorDisplayModule::Color currentTempColor(255,0,0);
ColorDisplayModule::Color targetTempColor(255,255,0);
ColorDisplayModule::Color fanSpeedColor(0,0,255);

enum Selection {
    SelectionTargetTemp,
    SelectionKp,
    SelectionKi,
    SelectionKd,
    NumSelections
};

Selection selection = SelectionTargetTemp;


//
// Variables for the PID Control Algorithm
//
const double defaultTargetTemp = 225;  // The default temperature at power on
double targetTemp = defaultTargetTemp; // The temeprature we're trying to achieve
double currentTemp = 225;              // The latest temperature from the sensor
double fanSpeed = 0;                   // The speed at which to run the fan

// Create a PID controller object and connect it to our variables.
PID myPID(&currentTemp, &fanSpeed, &targetTemp, 20, 0, 0, DIRECT);


//
// Variables for the graph
//
double graphMinTemp = 50;      // The minimum temperature to display
double graphMaxTemp = 350;     // The maximum temperature to display
double graphMinY = ColorDisplayModule::HEIGHT-10; // The display Y position of the min temperatre
double graphMaxY = 10; // The display Y position of the max temperature
double graphDuration = 15*60; // Duration of the graph in seconds

const int graphWidth = 96;    // The width of the graph in pixels
uint8_t currentTempGraph[graphWidth]; // The data for the graph
uint8_t targetTempGraph[graphWidth];  // The data for the graph
uint8_t fanSpeedGraph[graphWidth];  // The data for the graph
uint8_t graphPos = 0;          // The index in the graph for the current time

int tempToGraphY(float temp) {
    // Map temperature (in degrees) and to a value between 0 and 1
    double t = (temp-graphMinTemp)/(graphMaxTemp-graphMinTemp);

    // Map that 0 to 1 value into a Y position on the screen
    return t*graphMaxY + (1-t)*graphMinY;
}

int speedToGraphY(float speed) {
    // Map fan speed (0 to 255) to a value between 0 and 1
    double s = speed/255.0;

    // Map that 0 to 1 value into a Y position on the screen
    return s*graphMaxY + (1-s)*graphMinY;
}

void drawGraph(uint8_t *graphData) {
    // The Y position and starting X position of the
    // current line. Changes every time a new value is
    // encountered.
    int lineY = 0;
    int lineStartX = 0;

    // Walk over the graph from left to right
    for (int x=0; x < graphWidth; x++) {
        // Figure out the index into the data
        int i = (x+graphPos) % graphWidth;

        // If the value has not changed since the
        // previous position, we don't need to do anything
        if (graphData[i] == lineY) {
            continue;
        }

        // Draw the line, but only if the value was not 0
        if (lineY != 0) {
            display.drawLine(lineStartX, lineY,
                x, graphData[i]);
        }

        // Save the start position of the next line
        lineStartX = x;
        lineY = graphData[i];
    }

    // Draw the last line segment
    display.drawLine(lineStartX, lineY,
        graphWidth, lineY);
}

void setup() {
    //turn the PID on
    myPID.SetMode(AUTOMATIC);

    // Initialize graphData. Values at or below 0 won't be drawn.
    for (int i=0; i < graphWidth; i++) {
        currentTempGraph[i] = 0;
        targetTempGraph[i] = 0;
        fanSpeedGraph[i] = 0;
    }
}

void drawMainScreen() {
    // Clear and draw the screen.
    display.fillScreen(ColorDisplayModule::Black);
    display.setLineColor(ColorDisplayModule::Color(255,0,0));

    display.setCursor(0,0);
    display.setTextColor(currentTempColor);
    display.print(int(currentTemp));
    display.print("\xF7");

    display.setCursor(96-24, 0);
    display.setTextColor(targetTempColor);
    display.print(int(targetTemp));
    display.print("\xF7");

    display.setTextColor(fanSpeedColor);
    display.setCursor(0, display.HEIGHT-8);
    display.print("    Fan: ");
    display.print(int(fanSpeed*100/255));
    display.print("%");

    display.setLineColor(targetTempColor);
    drawGraph(targetTempGraph);

    display.setLineColor(fanSpeedColor);
    drawGraph(fanSpeedGraph);

    display.setLineColor(currentTempColor);
    drawGraph(currentTempGraph);
    
    display.refresh();
}

void drawSettingsScreen() {
    display.setCursor(0,0);
    display.fillScreen(ColorDisplayModule::Black);
    display.setTextColor(ColorDisplayModule::White);


    display.print(selection == SelectionKp ? "> " : "  ");
    display.print("Kp: ");
    display.println(myPID.GetKp());

    display.print(selection == SelectionKi ? "> " : "  ");
    display.print("Ki: ");
    display.println(myPID.GetKi());

    display.print(selection == SelectionKd ? "> " : "  ");
    display.print("Kd: ");
    display.println(myPID.GetKd());
    display.refresh();

}


int previousKnobPosition = 0;
bool knobWasPressed = false;

void loop() {
    ModuloLoop();

    // Read the thermocouple temperature, update the controller, and output the fan speed
    currentTemp = thermocouple.getTemperatureF();
    myPID.Compute();
    analogWrite(0, fanSpeed);

    // Upgrade graphPos, the index into graphData that corresponds to the current
    // time. We sample once a second (millis()/1000) and the index wraps around
    // when it gets bigger than graphWidth. (% graphWidth).
    float graphSampleRate = graphWidth/graphDuration;
    graphPos = int((millis()/1000)*graphWidth/graphDuration) % graphWidth;

    // Save the currentTemp and targetTemp at the current position in the graph
    currentTempGraph[graphPos] = tempToGraphY(currentTemp);
    targetTempGraph[graphPos] = tempToGraphY(targetTemp);   
    fanSpeedGraph[graphPos] = speedToGraphY(fanSpeed);  

    // When the knob is pressed, change the selection
    bool knobIsPressed = knob.getButton();
    if (!knobWasPressed and knobIsPressed) {
        selection = (Selection)((selection+1) % NumSelections);
    }
    knobWasPressed = knobIsPressed;

    // Find out how far the knob moved
    int newKnobPos = knob.getPosition();
    int knobDelta = newKnobPos-previousKnobPosition;
    previousKnobPosition = newKnobPos;

    if (selection == SelectionTargetTemp) {
        targetTemp += knobDelta;

        drawMainScreen();
    } else {
        if (knobDelta) {
            double Kp = myPID.GetKp();
            double Ki = myPID.GetKi();
            double Kd = myPID.GetKd();

            if (selection == SelectionKp) {
                Kp += .1*knobDelta;
            }
            if (selection == SelectionKd) {
                Kd += .01*knobDelta;
            }
            if (selection == SelectionKi) {
                Ki += .01*knobDelta;
            }

            myPID.SetTunings(Kp, Ki, Kd);
        }

        drawSettingsScreen();
    }

}

