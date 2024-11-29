/* 
  IR Breakbeam sensor demo! - https://learn.adafruit.com/ir-breakbeam-sensors/arduino
*/
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
// haptic vibrator
#include <Wire.h>
#include "Adafruit_DRV2605.h"

Adafruit_DRV2605 drv;

// Which pin on the Arduino is connected to the NeoPixels?
#define BRUSH_LED_PIN1        5 // On Trinket or Gemma, suggest changing this to 1
#define BRUSH_LED_PIN2        6 // On Trinket or Gemma, suggest changing this to 1
#define YELLOW_LED_PIN        9 // On Trinket or Gemma, suggest changing this to 1
#define BLUE_LED_PIN        10 // On Trinket or Gemma, suggest changing this to 1
#define RED_LED_PIN       11 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUM_RING_PIXELS 12 // Popular NeoPixel ring size
#define NUM_STRIP_PIXELS 90 // Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.

Adafruit_NeoPixel pixels1(NUM_RING_PIXELS, BRUSH_LED_PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels2(NUM_RING_PIXELS, BRUSH_LED_PIN2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels3(NUM_STRIP_PIXELS, YELLOW_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels4(NUM_STRIP_PIXELS, BLUE_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels5(NUM_STRIP_PIXELS, RED_LED_PIN, NEO_GRB + NEO_KHZ800);

//#define DELAYVAL 125 // Time (in milliseconds) to pause between pixels

//#define LEDPIN 13
  // Pin 13: Arduino has an LED connected on pin 13
  // Pin 11: Teensy 2.0 has the LED on pin 11
  // Pin  6: Teensy++ 2.0 has the LED on pin 6
  // Pin 13: Teensy 3.0 has the LED on pin 13


#define YELLOW_SENSOR_PIN 4
#define BLUE_SENSOR_PIN 12
#define RED_SENSOR_PIN 7

// variables will change:
int YELLOW_sensorState = 0, BLUE_sensorState = 0, RED_sensorState = 0; // IR break sensor
int lastState=0, sensorState=0;         // state variables
// color value for choose_color function
int YELLOW = 1;
int BLUE = 2;
int RED = 3;
// actual color values
int R = 0, G = 0, B = 0;
// for serial read
char receivedChar;
boolean newData = false;

int game = 0;
int ambient = 0;

void setup() {
  // Print file name, version, date on start up
  Serial.begin(9600);
  while (!Serial);
  Serial.println("name - Pluto_paint_dog_house, version - 1.1, github");
  
  //delay(100);
   // initialize the LED pin as an output:
  pinMode(BRUSH_LED_PIN1, OUTPUT);
  pinMode(BRUSH_LED_PIN2, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  
  // initialize the sensor pin as an input:
  pinMode(YELLOW_SENSOR_PIN, INPUT);
  pinMode(BLUE_SENSOR_PIN, INPUT);
  pinMode(RED_SENSOR_PIN, INPUT);
  
  //digitalWrite(BRUSH_LED_PIN1, HIGH); // turn on the pullup
  //digitalWrite(BRUSH_LED_PIN2, HIGH); // turn on the pullup     
  digitalWrite(YELLOW_SENSOR_PIN, HIGH); // turn on the pullup
  digitalWrite(BLUE_SENSOR_PIN, HIGH); // turn on the pullup
  digitalWrite(RED_SENSOR_PIN, HIGH); // turn on the pullup
  // put your setup code here, to run once:
  //pinMode(RED_LED_PIN, OUTPUT);

  pixels1.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels2.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels3.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels4.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels5.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)

  // pixels.setPixelColor(10, pixels.Color(0, 0, 0));
  pixels1.clear(); // Set all pixel colors to 'off'
  pixels2.clear(); // Set all pixel colors to 'off'
  pixels3.clear(); // Set all pixel colors to 'off'
  pixels4.clear(); // Set all pixel colors to 'off'
  pixels5.clear(); // Set all pixel colors to 'off'

  pixels1.show();   // Send the updated pixel colors to the hardware.
  pixels2.show();   // Send the updated pixel colors to the hardware.
  pixels3.show();   // Send the updated pixel colors to the hardware.
  pixels4.show();   // Send the updated pixel colors to the hardware.
  pixels5.show();   // Send the updated pixel colors to the hardware.

  // haptic vitrator
  if (! drv.begin()) {
    while (!Serial);
    Serial.println("Could not find DRV2605");
    while (1) delay(10);
    }

  drv.selectLibrary(1);
  
  // I2C trigger by sending 'go' command 
  // default, internal trigger when sending GO command
  drv.setMode(DRV2605_MODE_INTTRIG);
  
}

uint8_t effect = 16;

void loop(){

  while (!Serial) ;
  recvOneChar(); // grab serial data
  showNewData();

  // read the state of the pushbutton value:
  YELLOW_sensorState = digitalRead(YELLOW_SENSOR_PIN);
  BLUE_sensorState = digitalRead(BLUE_SENSOR_PIN);
  RED_sensorState = digitalRead(RED_SENSOR_PIN);
  sensorState = HIGH;
  delay(100);

  // game mode
  if(game == 1){
  if (YELLOW_sensorState == LOW) {
    sensorState = LOW;
    R = 150;
    G = 150;
    B = 0;
    choose_color(YELLOW);
  }
  if (BLUE_sensorState == LOW) {
    // Serial.println("BLUE sensor");
    sensorState = LOW;
    R = 0;
    G = 0;
    B = 150;
    choose_color(BLUE);
  }
   if (RED_sensorState == LOW) {
    sensorState = LOW;
    R = 150;
    G = 0;
    B = 0;
    choose_color(RED);
  }
}
lastState = sensorState; // reset sensor state

if(ambient == 1){
    pixels1.clear(); // Set all pixel colors to 'off'
    pixels2.clear(); // Set all pixel colors to 'off'
    pixels3.clear(); // Set all pixel colors to 'off'
    pixels4.clear(); // Set all pixel colors to 'off'
    pixels5.clear(); // Set all pixel colors to 'off'

    pixels1.show();
    pixels2.show();
    pixels3.show();
    pixels4.show();
    pixels5.show();
    
  for(int i=0; i<NUM_STRIP_PIXELS; i++) { // For each pixel...
    if(i < NUM_RING_PIXELS){
      pixels1.setPixelColor(i, pixels1.Color(0, 150, 0));
      pixels2.setPixelColor(i, pixels2.Color(0, 150, 0));
    }

    pixels3.setPixelColor(i, pixels2.Color(150, 150, 0));
    pixels4.setPixelColor(i, pixels2.Color(0, 150, 0));
    pixels5.setPixelColor(i, pixels2.Color(0, 0, 150));
    pixels1.show();   // Send the updated pixel colors to the hardware.
    pixels2.show();   // Send the updated pixel colors to the hardware.
    pixels3.show();   // Send the updated pixel colors to the hardware.
    pixels4.show();   // Send the updated pixel colors to the hardware.
    pixels5.show();   // Send the updated pixel colors to the hardware.
    delay(50);
  }
    // pixels.setPixelColor(10, pixels.Color(0, 0, 0));
    pixels1.clear(); // Set all pixel colors to 'off'
    pixels2.clear(); // Set all pixel colors to 'off'
    pixels3.clear(); // Set all pixel colors to 'off'
    pixels4.clear(); // Set all pixel colors to 'off'
    pixels5.clear(); // Set all pixel colors to 'off'

    pixels1.show();
    pixels2.show();
    pixels3.show();
    pixels4.show();
    pixels5.show();
}
}

void choose_color(int color){
    if (!sensorState && lastState) {
    // Serial.println("Broken");
    for(int i=0; i<NUM_RING_PIXELS; i++) { // For each pixel...
      pixels1.setPixelColor(i, pixels1.Color(R, G, B));
      pixels2.setPixelColor(i, pixels2.Color(R, G, B));
    }
    if(color == YELLOW){
      for(int i=0; i<NUM_STRIP_PIXELS; i++) { // For each pixel...
        pixels3.setPixelColor(i, pixels3.Color(R, G, B));
      }
      pixels4.clear(); // Set all pixel colors to 'off'
      pixels5.clear(); // Set all pixel colors to 'off'
      Serial.println("Y");  
    }
    if(color == BLUE){
      for(int i=0; i<NUM_STRIP_PIXELS; i++) { // For each pixel...
        pixels4.setPixelColor(i, pixels4.Color(R, G, B));
      }
      pixels3.clear(); // Set all pixel colors to 'off'
      pixels5.clear(); // Set all pixel colors to 'off'
      Serial.println("B");
    }
    if(color == RED){
      for(int i=0; i<NUM_STRIP_PIXELS; i++) { // For each pixel...
      pixels5.setPixelColor(i, pixels5.Color(R, G, B));
      }
      pixels3.clear(); // Set all pixel colors to 'off'
      pixels4.clear(); // Set all pixel colors to 'off'
      Serial.println("R");
    }
      pixels1.show();   // Send the updated pixel colors to the hardware.
      pixels2.show();   // Send the updated pixel colors to the hardware.
      pixels3.show();   // Send the updated pixel colors to the hardware.
      pixels4.show();   // Send the updated pixel colors to the hardware.
      pixels5.show();   // Send the updated pixel colors to the hardware.

// Haptic
// if (effect == 1) {
//   Serial.println("11.2 Waveform Library Effects List");
// }

//  if (effect == 1) {
//    Serial.println(F("1 − Strong Click - 100%"));
//  }

// set the effect to play
  drv.setWaveform(0, effect);  // play effect 
  //drv.setWaveform(1, 0);       // end waveform
//drv.play()
  // play the effect!
  drv.go();

  // wait a bit
  //delay(500);

//  effect++;
//  if (effect > 117) effect = 1;
  }
}

void recvOneChar() {
    if (Serial.available() > 0) {
        receivedChar = Serial.read();
        if(receivedChar == 'G'){
        game = 1;
        ambient = 0;
        }
        if(receivedChar == 'A'){
          ambient = 1;
          game = 0;
        newData = true;
        }
    newData = true;
    }
}

void showNewData() {
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChar);
        newData = false;
    }
}
