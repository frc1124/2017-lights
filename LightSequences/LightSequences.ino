//lights setup
#include <FastLED.h>
#define START_LED 200
#define NUM_LEDS 300
#define DATA_PIN 12
#define CLOCK_PIN 11
#define NUM_LEDS_FRONT_LEFT 25
#define NUM_LEDS_FRONT_RIGHT 25
#define NUM_LEDS_DRIVE 50
#define CHIPSET APA102
CRGB leds[NUM_LEDS];
CRGB allianceColor = CRGB::Blue;
CRGB notBlindingWhite = CRGB(128,128,128);

//robot states
#define TELEOP_DISABLED 1
#define TELEOP_ENABLED 2
#define AUTO_DISABLED 3
#define AUTO_ENABLED 4
#define E_STOP 5
#define DISCONNECTED 6
#define SEARCH_FEEDER 7
#define GEAR_READY 8
#define GEAR_IN 9
#define SEARCH_LIFT 10
#define CLIMB 11

//test settings
byte mode = AUTO_ENABLED;
int z = 1;

void setup() {
  FastLED.addLeds<CHIPSET, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
}

void teleopDisabled() {
  //STILL
  
  //front: alternating alliance color and white
  for (int x=START_LED; x<NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT-1; x+=2) {
    leds[x] = allianceColor;
    leds[x+1] = notBlindingWhite;
  }
  
  //drivetrain: alternating alliance color and white in sets of two (ex. blue, blue, white, white)
  for (int x=NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT; x<NUM_LEDS-3; x+=4) {
    leds[x] = leds[x+1] = allianceColor;
    leds[x+2] = leds[x+3] = notBlindingWhite;
  }
}

void teleopEnabled() {
  //STILL
  
  //front: top 3 alliance colored, rest white
  for (int x=3; x<NUM_LEDS_FRONT_LEFT; x++) {
    leds[x] = leds[x+NUM_LEDS_FRONT_LEFT] = notBlindingWhite;
  }
  leds[0] = leds[1] = leds[2] = leds[NUM_LEDS_FRONT_LEFT] = leds[NUM_LEDS_FRONT_LEFT+1] = leds[NUM_LEDS_FRONT_LEFT+2] = allianceColor;
  
  //drivetrain: alliance color
  for (int x=NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT; x<NUM_LEDS; x++) {
    leds[x] = allianceColor;
  }
}

void autoDisabled() {
  //STILL
  
  //front: alternating alliance color and off
  for (int x=0; x<NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT-1; x+=2) {
    leds[x] = allianceColor;
    leds[x+1] = CRGB::Black;
  }
  
  //drivetrain: alternating alliance color and off in sets of two (ex. blue, blue, off, off)
  for (int x=NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT; x<NUM_LEDS-3; x+=4) {
    leds[x] = leds[x+1] = allianceColor;
    leds[x+2] = leds[x+3] = CRGB::Black;
  }
}

void autoEnabled() {
  //MOVING
  
  //front: alternating alliance color and white pulses, 0.5s each (ex. blue, off, blue, off; off, white, off, white)
  int red1, green1, blue1, red2, green2, blue2;
  CRGB color1, color2;
  int red, green, blue;
  CRGB lightColor;
  bool ascending = false;
  while (mode==AUTO_ENABLED) {
  if (allianceColor == CRGB(0,0,256)) {
      red1 = green1 = 0;
      blue1 = 256;
  }
  else {
      red1 = 256;
      green1 = blue1 = 0;
  }
  red2 = green2 = blue2 = 0;
    //alliance color lights
      if (ascending == true) {
        blue1++;
      }
      else {
        blue1--;
      }
      if (ascending == true) {
        red1++;
      }
      else {
        red1--;
      }
    //white lights
    if (ascending == false) {
      red2++;
      green2++;
      blue2++;
      if (red2==256 || green2==256 || blue2==256) ascending = true;
    }
    else {
      red2--;
      green2--;
      blue2--;
      if (red2==0 || green2==0 || blue2==0) ascending = false;
    }
  for (int x=0; x<NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT-1; x+=2) {
    color1 = CRGB(red1, green1, blue1);
    color2 = CRGB(red2, green2, blue2);
    leds[x] = color1;
    leds[x+1] = color2;
  }
  
  //drivetrain: alliance color pulse, 0.5s
  switch (allianceColor) {
    case CRGB::Blue:
      red = green = 0;
      blue = 256;
      break;
    case CRGB::Red:
      red = 256;
      green = blue = 0;
      break;
  }
  if (ascending==true) blue++;
  else blue--;
  if (ascending==true) red++;
  else red--;
  for (int x=NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT; x<NUM_LEDS; x++) {
    leds[x] = CRGB(red, green, blue);
  }
  FastLED.show();
  delay(2);
}
}

void eStop() {
  //MOVING
  
  //robot: alternating red and orange, 0.5s each (ex. red, red, red, red; orange, orange, orange, orange)
  for (int x=START_LED; x<NUM_LEDS; x++) {
    leds[x] = CRGB::Red;
  }
  FastLED.show();
  delay(500);
  for (int x=START_LED; x<NUM_LEDS; x++) {
    leds[x] = CRGB(255,64,0);
  }
  FastLED.show();
  delay(500);
}

void disconnected() {
  //MOVING
  
  //robot: 3 light chase up, across, and down (ex. red, white, blue, off, off, red, white, blue)
  int lightSpeed = 75;
  for (int x=START_LED; x<NUM_LEDS-5; x+=5) {
    leds[x] = CRGB::Red;
    leds[x+1] = CRGB::White;
    leds[x+2] = CRGB::Blue;
    leds[x+3] = CRGB::Black;
    leds[x+4] = CRGB::Black;
  }
  FastLED.show();
  delay(lightSpeed);
  for (int x=START_LED; x<NUM_LEDS-5; x+=5) {
    leds[x] = CRGB::White;
    leds[x+1] = CRGB::Blue;
    leds[x+2] = CRGB::Black;
    leds[x+3] = CRGB::Black;
    leds[x+4] = CRGB::Red;
  }
  FastLED.show();
  delay(lightSpeed);
  for (int x=START_LED; x<NUM_LEDS-5; x+=5) {
    leds[x] = CRGB::Blue;
    leds[x+1] = CRGB::Black;
    leds[x+2] = CRGB::Black;
    leds[x+3] = CRGB::Red;
    leds[x+4] = CRGB::White;
  }
  FastLED.show();
  delay(lightSpeed);
  for (int x=START_LED; x<NUM_LEDS-5; x+=5) {
    leds[x] = CRGB::Black;
    leds[x+1] = CRGB::Black;
    leds[x+2] = CRGB::Red;
    leds[x+3] = CRGB::White;
    leds[x+4] = CRGB::Blue;
  }
  FastLED.show();
  delay(lightSpeed);
  for (int x=START_LED; x<NUM_LEDS-5; x+=5) {
    leds[x] = CRGB::Black;
    leds[x+1] = CRGB::Red;
    leds[x+2] = CRGB::White;
    leds[x+3] = CRGB::Blue;
    leds[x+4] = CRGB::Black;
  }
  FastLED.show();
  delay(lightSpeed);
}

void searchFeeder() {
  //MOVING

  //drivetrain: alliance color
  for (int x=NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT; x<NUM_LEDS; x++) {
    leds[x] = allianceColor;
  }
  FastLED.show();
  
  //front: yellow pulse, 1s (ex. yellow, yellow, yellow; off, off, off)
  pulse(0,NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT, CRGB::Yellow, 1000);
}

void gearReady() {
  //MOVING
 
  //drivetrain: alliance color
  for (int x=NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT; x<NUM_LEDS; x++) {
    leds[x] = allianceColor;
  }
  
  //front: green pulse, 1s (ex. green, green, green; off, off, off)
  pulse(0,NUM_LEDS_FRONT_RIGHT+NUM_LEDS_FRONT_LEFT, CRGB::Green, 1000);
}

void gearIn() {
  //STILL
  
  //front: green flash, 0.75s
  for (int x=0; x<NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT; x++) {
    leds[x] = CRGB::Green;
  }
  
  //drivetrain: alliance color
  for (int x=NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT; x<NUM_LEDS; x++) {
    leds[x] = allianceColor;
  }
}

void searchLift() {
  //MOVING
  
  //front: top 3 alliance color, rest white (ex. blue, blue, blue, white, white, white)
  leds[0] = leds[1] = leds[2] = leds[NUM_LEDS_FRONT_LEFT] = leds[NUM_LEDS_FRONT_LEFT+1] = leds[NUM_LEDS_FRONT_LEFT+2] = allianceColor;
  for (int x=3; x<NUM_LEDS_FRONT_LEFT; x++) {
    leds[x] = notBlindingWhite;
    leds[x+NUM_LEDS_FRONT_LEFT] = notBlindingWhite;
  }
  //drivetrain: alliance color pulse, 1s (ex. blue, blue, blue; off, off, off)
  pulse(NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT, NUM_LEDS_DRIVE, allianceColor, 1000);
}

void climb() {
  //MOVING
  
  //drivetrain: alliance color
  for (int x=START_LED+NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT; x<NUM_LEDS; x++) {
    leds[x] = allianceColor;
  }
  
  //front: 3 light chase alternating up sides (ex. left: blue, white, blue, off, off, off; right: off, off, off, blue, white, blue)
  int lightSpeed = 75;
  for (int x=START_LED; x<NUM_LEDS_FRONT_LEFT-6; x+=6) {
    leds[x] = leds[NUM_LEDS_FRONT_LEFT+x+3] = allianceColor;
    leds[x+1] = leds[NUM_LEDS_FRONT_LEFT+x+4] = notBlindingWhite;
    leds[x+2] = leds[NUM_LEDS_FRONT_LEFT+x+5] = allianceColor;
    leds[x+3] = leds[x+4] = leds[x+5] = leds[NUM_LEDS_FRONT_LEFT+x] = leds[NUM_LEDS_FRONT_LEFT+x+1] = leds[NUM_LEDS_FRONT_LEFT+x+2] = CRGB::Black;
  }
  FastLED.show();
  delay(lightSpeed);
  for (int x=START_LED; x<NUM_LEDS_FRONT_LEFT-6; x+=6) {
    leds[x] = leds[NUM_LEDS_FRONT_LEFT+x+3] = notBlindingWhite;
    leds[x+1] = leds[NUM_LEDS_FRONT_LEFT+x+4] = allianceColor;
    leds[x+2] = leds[x+3] = leds[x+4] = leds[NUM_LEDS_FRONT_LEFT+x+5] = leds[NUM_LEDS_FRONT_LEFT+x] = leds[NUM_LEDS_FRONT_LEFT+x+1] = CRGB::Black;
    leds[x+5] = leds[NUM_LEDS_FRONT_LEFT+x+2] = allianceColor;
  }
  FastLED.show();
  delay(lightSpeed);
  for (int x=START_LED; x<NUM_LEDS_FRONT_LEFT-6; x+=6) {
    leds[x] = leds[NUM_LEDS_FRONT_LEFT+x+3] = allianceColor;
    leds[x+1] = leds[x+2] = leds[x+3] = leds[NUM_LEDS_FRONT_LEFT+x+4] = leds[NUM_LEDS_FRONT_LEFT+x+5] = leds[NUM_LEDS_FRONT_LEFT+x] = CRGB::Black;
    leds[x+4] = leds[NUM_LEDS_FRONT_LEFT+x+1] = allianceColor;
    leds[x+5] = leds[NUM_LEDS_FRONT_LEFT+x+2] = notBlindingWhite;
  }
  FastLED.show();
  delay(lightSpeed);
  for (int x=START_LED; x<NUM_LEDS_FRONT_LEFT-6; x+=6) {
    leds[x] = leds[x+1] = leds[x+2] = leds[NUM_LEDS_FRONT_LEFT+x+3] = leds[NUM_LEDS_FRONT_LEFT+x+4] = leds[NUM_LEDS_FRONT_LEFT+x+5] = CRGB::Black;
    leds[x+3] = leds[NUM_LEDS_FRONT_LEFT+x] = allianceColor;
    leds[x+4] = leds[NUM_LEDS_FRONT_LEFT+x+1] = notBlindingWhite;
    leds[x+5] = leds[NUM_LEDS_FRONT_LEFT+x+2] = allianceColor;
  }
  FastLED.show();
  delay(lightSpeed);
  for (int x=START_LED; x<NUM_LEDS_FRONT_LEFT-6; x+=6) {
    leds[x] = leds[x+1] = leds[NUM_LEDS_FRONT_LEFT+x+3] = leds[NUM_LEDS_FRONT_LEFT+x+4] = CRGB::Black;
    leds[x+2] = leds[NUM_LEDS_FRONT_LEFT+x+5] = allianceColor;
    leds[x+3] = leds[NUM_LEDS_FRONT_LEFT+x] = notBlindingWhite;
    leds[x+4] = leds[NUM_LEDS_FRONT_LEFT+x+1] = allianceColor;
    leds[x+5] = leds[NUM_LEDS_FRONT_LEFT+x+2] = CRGB::Black;
  }
  FastLED.show();
  delay(lightSpeed);
  for (int x=START_LED; x<NUM_LEDS_FRONT_LEFT-6; x+=6) {
    leds[x] = leds[NUM_LEDS_FRONT_LEFT+x+3] = CRGB::Black;
    leds[x+1] = leds[NUM_LEDS_FRONT_LEFT+x+4] = allianceColor;
    leds[x+2] = leds[NUM_LEDS_FRONT_LEFT+x+5] = notBlindingWhite;
    leds[x+3] = leds[NUM_LEDS_FRONT_LEFT+x] = allianceColor;
    leds[x+4] = leds[x+5] = leds[NUM_LEDS_FRONT_LEFT+x+1] = leds[NUM_LEDS_FRONT_LEFT+x+2] = CRGB::Black;
  }
  FastLED.show();
  delay(lightSpeed);
}

void pulse(int startLed, int numLeds, CRGB color, int pulseSpeed) {
  int red, green, blue;
  CRGB lightColor;
  bool ascending = false;
  for (int x=startLed; x<startLed+numLeds; x++){
    switch (color) {
      case CRGB::Blue:
        red = green = 0;
        blue = 256;
        if (ascending==true) blue++;
        else blue--;
        if (blue==0) ascending = true;
        if (blue==256) ascending = false;
        break;
      case CRGB::Red:
        red = 256;
        green = blue = 0;
        if (ascending==true) red++;
        else red--;
        if (red==0) ascending = true;
        if (red==256) ascending = false;
        break;
      case CRGB::Yellow:
        red = green = 256;
        blue = 0;
        if (ascending==true) {
          red++;
          green++;
        }
        else {
          red--;
          green--;
        }
        if (red==0 || green==0) ascending = true;
        if (red==256 || green==256) ascending = false;
        break;
      case CRGB::Green:
        red = blue = 0;
        green = 256;
        if (ascending==true) green++;
        else green--;
        if (green==0) ascending = true;
        if (green==256) ascending = false;
        break;
    }
    leds[x] = lightColor;
    FastLED.show();
    delay(pulseSpeed);
  }
}

void loop() {
  Serial.print("nyah1");
  switch (mode) {
    case TELEOP_DISABLED:
      Serial.print("nyah2");
      teleopDisabled();
      break;
    case TELEOP_ENABLED:
      teleopEnabled();
      break;
    case AUTO_DISABLED:
      autoDisabled();
      break;
    case AUTO_ENABLED:
      autoEnabled();
      break;
    case E_STOP:
      eStop();
      break;
    case DISCONNECTED:
      disconnected();
      break;
    case SEARCH_FEEDER:
      searchFeeder();
      break;
    case GEAR_READY:
      gearReady();
      break;
    case GEAR_IN:
      gearIn();
      break;
    case SEARCH_LIFT:
      searchLift();
      break;
    case CLIMB:
      climb();
      break;
    default:
      disconnected();
  }
      
      /*for (int x=175;x<NUM_LEDS-7;x+=3) {
        if (z==1) {
        leds[x] = CRGB(255,0,255);
        leds[x+1] = CRGB::Red;
        leds[x+2] = CRGB::Green;
        }
        else if (z==2){
          leds[x] = CRGB::Red;
          leds[x+1] = CRGB::Green;
          leds[x+2] = CRGB(255,0,255);
        }
        else {
          leds[x] = CRGB::Green;
          leds[x+1] = CRGB(255,0,255);
          leds[x+2] = CRGB::Red;
        }
        leds[295]=CRGB::Black;
  }
  if (z==1) z = 2;
  else if (z==2) z = 3;
  else z = 1;
  
        FastLED.show();
        delay(125);*/
        
        /*for (int x=200; x<NUM_LEDS; x++) {
    leds[x] = CRGB::Red;
  }
  FastLED.show();
  delay(500);
  for (int x=200; x<NUM_LEDS; x++) {
    leds[x] = CRGB(255,64,0);
  }
  FastLED.show();
  delay(500);*/
}
