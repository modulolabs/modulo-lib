/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// Pin 11 has the LED on Teensy 2.0
// Pin 6  has the LED on Teensy++ 2.0
// Pin 13 has the LED on Teensy 3.0
// give it a name:
int redPin = 10;
int greenPin = 11;
int bluePin = 13;


void SetColor(float r, float g, float b) {
  r = max(min(r,1),0);
  g = max(min(g,1),0);
  b = max(min(b,1),0);
  analogWrite(redPin, 255-r*255);
  analogWrite(greenPin, 255-g*255);
  analogWrite(bluePin, 255-b*255); 
}

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(0, INPUT_PULLUP);
  
  SetColor(1,0,0);
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
    if (USBDevice.configured()) {
        SetColor(0,0,1);
    }
}
