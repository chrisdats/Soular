/* Soularv2.ino
  December 2018
  Christopher Datsikas
  Based on Adafruit Code
  
  
  TODO: Implement two modes
  Onbard LED on triggered when V0 pressed on app
  */

#include <Adafruit_NeoPixel.h>
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET
//#include <avr/power.h> // ENABLE THIS LINE FOR GEMMA OR TRINKET

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "e9b91c10830f40d089089d3cfa4bc34e";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Verizon-SM-G950U-A968";
char pass[] = "qihanhan";

#define NUMPIXELS 1 // Number of LEDs in strip
#define PIN 4
#define SPEED 10
#define LAMPOFF 0
#define LAMPHALF 50
#define LAMPFULL 254

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
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_RGBW + NEO_KHZ800);

int buttonInPin = 0;         // the number of the input pin
int ledPin = 16;             // the number of the output pin

int powerState = LOW; // if device is on or off
int lightMode = HIGH;  // if device is in sunlight or sunrise mode
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin

// the follow variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long myTime = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers

void setup() {
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  pinMode(buttonInPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  strip.begin(); // Initialize pins for output
  strip.clear();
  strip.show();  // Turn all LEDs off ASAP
}


void loop() {
  // continuously polls the app
  Blynk.run();

  // continuously polls the capacitive sensor
  reading = digitalRead(buttonInPin);

  // if the input just went from HIGH TO LOW and we've waited long enough
  // to ignore any noise on the circuit, toggle the output pin and remember
  // the time. Capacitive sensor is active low.
  if (reading == LOW && previous == HIGH && millis() - myTime > debounce) {
    toggleOnOff();
    myTime = millis();
  }
  previous = reading;
}

void toggleOnOff() {
  //if device is on turn it off
  if (powerState == HIGH) {
    powerState = LOW;
    fade(LAMPFULL, LAMPOFF, SPEED);
  }
  //if device is off turn it on
  else {
    powerState = HIGH;
    fade(LAMPOFF, LAMPFULL, SPEED);
  }
  Serial.print("Power State set to");
  Serial.println(powerState);
  Blynk.virtualWrite(V0, powerState); //sending to Blynk
    digitalWrite(ledPin, !powerState);    // onboard Led active low
}

// turn on and off the device
BLYNK_WRITE(V0)
{
  int pinValue = param.asInt();
  Serial.print("V1 Slider value is: ");
  Serial.println(pinValue);
  if (pinValue == 1) {    // if Button sends 1
    fade(LAMPOFF, LAMPFULL, SPEED);
    powerState = HIGH;
    Serial.println("Turning device on");
  }
  else if (pinValue == 0) {
    fade(LAMPFULL, LAMPOFF, SPEED);
    powerState = LOW;
    Serial.println("Turning device off");
  }
}


// go from a starting brightness level to an endBrightness level in a certain amount of time
// this is awful code; TODO: rewrite
void fade(uint8_t startBrightness, uint8_t endBrightness, uint8_t wait) {
  if (startBrightness < endBrightness) {
    for (int b = startBrightness ; b < endBrightness; b++) {
      for (int thisPixel = 0; thisPixel < NUMPIXELS; thisPixel++) {   // iterate through all the levers                               // if lever is pulled down, turn green
        strip.setPixelColor(thisPixel, b, b, b); // Moderately bright green color.
      }
      strip.show();                             // Update pixel color.
      delay(wait);
    }
  }
  else if (startBrightness > endBrightness) {
    for (int b = startBrightness ; b > endBrightness; b--) {
      for (int thisPixel = 0; thisPixel < strip.numPixels(); thisPixel++) {   // iterate through all the levers                               // if lever is pulled down, turn green
        strip.setPixelColor(thisPixel, b, b, b); // Moderately bright green color.
      }
      strip.show();                             // Update pixel color.
      delay(wait);
    }
  }
}
