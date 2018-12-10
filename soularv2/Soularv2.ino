/* Soular.ino
 November 2018
 Christopher Datsikas
 Based on Adafruit Code */

#include <Adafruit_NeoPixel.h>
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET
//#include <avr/power.h> // ENABLE THIS LINE FOR GEMMA OR TRINKET

#define NUMPIXELS 8 // Number of LEDs in strip
#define LAMPOFF 0
#define LAMPHALF 50
#define LAMPFULL 254
#define SPEED 10

// Here's how to control the LEDs from any two pins:
//#define DATAPIN    11
//#define CLOCKPIN   13
//Adafruit_DotStar strip = Adafruit_DotStar(
//NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
// The last parameter is optional -- this is the color data order of the
// DotStar strip, which has changed over time in different production runs.
// Your code just uses R,G,B colors, the library then reassigns as needed.
// Default is DOTSTAR_BRG, so change this if you have an earlier strip.

// Hardware SPI is a little faster, but must be wired to specific pins
// (Arduino Uno = pin 11 for data, 13 for clock, other boards are different).
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_RGBW + NEO_KHZ800);

int inPin = 4;         // the number of the input pin
int outPin = 13;       // the number of the output pin

int state = HIGH;      // the current state of the output pin
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin

// the follow variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers

void setup() {
  pinMode(inPin, INPUT);
  pinMode(outPin, OUTPUT);
  
  strip.begin(); // Initialize pins for output
  strip.clear();
  strip.show();  // Turn all LEDs off ASAP
}



void loop() {
  // continuously polls the capacitive sensor
  reading = digitalRead(inPin);

  // if the input just went from HIGH TO LOW and we've waited long enough
  // to ignore any noise on the circuit, toggle the output pin and remember
  // the time. Capacitive sensor is active low.
  if (reading == LOW && previous == HIGH && millis() - time > debounce) {
    if (state == HIGH) {
      state = LOW;
      fade(LAMPOFF,LAMPFULL,SPEED);
    }
    else {
      state = HIGH;
      fade(LAMPFULL, LAMPOFF, SPEED);
    }

    time = millis();    
  }

  digitalWrite(outPin, state);

  previous = reading;

}


// go from a starting brightness level to an endBrightness level in a certain amount of time
// this is awful code; TODO: rewrite
void fade(uint8_t startBrightness, uint8_t endBrightness, uint8_t wait) {
  if (startBrightness < endBrightness) {
    for (int b = startBrightness ; b < endBrightness; b++){
      for (int thisPixel = 0; thisPixel < NUMPIXELS; thisPixel++) {   // iterate through all the levers                               // if lever is pulled down, turn green
        strip.setPixelColor(thisPixel, b,b,b);   // Moderately bright green color.
      }
      strip.show();                             // Update pixel color.
      delay(wait);
    }
  }
  else if (startBrightness > endBrightness) {
    for (int b = startBrightness ; b > endBrightness; b--){
      for (int thisPixel = 0; thisPixel < strip.numPixels(); thisPixel++) {   // iterate through all the levers                               // if lever is pulled down, turn green
        strip.setPixelColor(thisPixel, b,b,b);   // Moderately bright green color.
      }
      strip.show();                             // Update pixel color.
      delay(wait);
    }
  }
}

