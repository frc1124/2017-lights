//lights setup
#include <FastLED.h>
#include <Wire.h>
#define START_LED 138
#define NUM_LEDS 244
=======
#define START_LED 147
#define NUM_LEDS 239
#define DATA_PIN 12
#define CLOCK_PIN 11
#define NUM_LEDS_FRONT_LEFT 28
#define NUM_LEDS_FRONT_RIGHT 28
#define NUM_LEDS_DRIVE 36
#define CHIPSET APA102
#define BRIGHTNESS 50
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
byte mode = CLIMB;
int z = 1;

void setup() {
  FastLED.addLeds<CHIPSET, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  Wire.begin(4);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
}
void receiveEvent(int howMany){
  while(Wire.available()){
    byte b = Wire.read();
    if(b==0xFF){
      byte isAllianceRed = Wire.read();
      allianceColor = (isAllianceRed==1) ? CRGB::Red : CRGB::Blue;
      byte m = Wire.read();
      mode = m;
      byte hasGear = Wire.read();
    }
  }
}
void teleopDisabled() {
  //STILL
  
  //front: alternating alliance color and white
  for (int x=START_LED; x<START_LED+NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT-1; x+=2) {
    leds[x] = allianceColor;
    leds[x+1] = CRGB::White;
  }
  
  //drivetrain: alternating alliance color and white in sets of two (ex. blue, blue, white, white)
  for (int x=START_LED+NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT; x<NUM_LEDS-3; x+=4) {
    leds[x] = leds[x+1] = allianceColor;
    leds[x+2] = leds[x+3] = CRGB::White;
  }
  int specialBoi = NUM_LEDS_DRIVE % 4; //fixes the last 1, 2, or 3
  switch (specialBoi) {
    case 0:
      break;
    case 1:
      leds[NUM_LEDS-1] = allianceColor;
      break;
    case 2:
      leds[NUM_LEDS-2] = leds[NUM_LEDS-1] = allianceColor;
      break;
    case 3:
      leds[NUM_LEDS-3] = leds[NUM_LEDS-2] = allianceColor;
      leds[NUM_LEDS-1] = CRGB::White;
     break;
  }
  FastLED.show();
}

void teleopEnabled() {
  //STILL
  
  //front: top 3 alliance colored, rest white
  for (int x=START_LED+3; x<START_LED+NUM_LEDS_FRONT_LEFT; x++) {
    leds[x] = leds[x+NUM_LEDS_FRONT_LEFT] = notBlindingWhite;
  }
  leds[START_LED] = leds[START_LED+1] = leds[START_LED+2] = leds[START_LED+NUM_LEDS_FRONT_LEFT] = leds[START_LED+NUM_LEDS_FRONT_LEFT+1] = leds[START_LED+NUM_LEDS_FRONT_LEFT+2] = allianceColor;
  
  //drivetrain: alliance color
  for (int x=START_LED+NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT; x<NUM_LEDS; x++) {
    leds[x] = allianceColor;
  }
  FastLED.show();
}

void autoDisabled() {
  //STILL
  
  //front: alternating alliance color and off
  for (int x=START_LED; x<START_LED+NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT-1; x+=2) {
    leds[x] = allianceColor;
    leds[x+1] = CRGB::Black;
  }
  
  //drivetrain: alternating alliance color and off in sets of two (ex. blue, blue, off, off)
  for (int x=START_LED+NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT; x<NUM_LEDS-3; x+=4) {
    leds[x] = leds[x+1] = allianceColor;
    leds[x+2] = leds[x+3] = CRGB::Black;
  }
  int specialBoi = NUM_LEDS_DRIVE % 4; //fixes the last 1, 2, or 3
  switch (specialBoi) {
    case 0:
      break;
    case 1:
      leds[NUM_LEDS-1] = allianceColor;
      break;
    case 2:
      leds[NUM_LEDS-2] = leds[NUM_LEDS-1] = allianceColor;
      break;
    case 3:
      leds[NUM_LEDS-3] = leds[NUM_LEDS-2] = allianceColor;
      leds[NUM_LEDS-1] = CRGB::Black;
     break;
  }
  FastLED.show();
}

void autoEnabled() {
  //MOVING
  
  //front: alternating alliance color and white pulses, 0.5s each (ex. blue, off, blue, off; off, white, off, white)
  int red1, green1, blue1, red2, green2, blue2;
  CRGB color1, color2;
  int red, green, blue;
  CRGB lightColor;
  bool ascending = false;
  if (allianceColor == CRGB(0,0,255)) {
      red1 = green1 = 0;
      blue1 = 255;
      red = green = 0;
      blue = 255;
  }
  else {
      red1 = 255;
      green1 = blue1 = 0;
      red = 255;
      green = blue = 0;
  }
  red2 = green2 = blue2 = 0;
  while (mode==AUTO_ENABLED) {
    //alliance color lights
      if (allianceColor == CRGB(0,0,255)) {
        if (ascending == true) {
          blue1+=5;
        }
        else {
         blue1-=5;
       }
      }
      else {
        if (ascending == true) {
          red1+=5;
        }
        else {
          red1-=5;
        }
      }
    //white lights
    if (ascending == false) {
      red2+=5;
      green2+=5;
      blue2+=5;
      if (red2==255 || green2==255 || blue2==255) ascending = true;
    }
    else {
      red2-=5;
      green2-=5;
      blue2-=5;
      if (red2==0 || green2==0 || blue2==0) ascending = false;
    }
  for (int x=START_LED; x<START_LED+NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT-1; x+=2) {
    color1 = CRGB(red1, green1, blue1);
    color2 = CRGB(red2, green2, blue2);
    leds[x] = color1;
    leds[x+1] = color2;
  }
  
  //drivetrain: alliance color pulse, 0.5s
  if (allianceColor == CRGB(0,0,255)) {
    if (ascending==true) blue+=5;
    else blue-=5;
  }
  else {
    if (ascending==true) red+=5;
    else red-=5;
  }
  for (int x=START_LED+NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT; x<NUM_LEDS; x++) {
    leds[x] = CRGB(red, green, blue);
  }
  FastLED.show();
  delay(12);
}
}

void eStop() {
  //MOVING
  
  //robot: off, red, orange, red, off chase
  /*for (int x=START_LED; x<NUM_LEDS; x++) {
    leds[x] = CRGB::Red;
  }
  FastLED.show();
  delay(500);
  for (int x=START_LED; x<NUM_LEDS; x++) {
    leds[x] = CRGB(255,128,0);
  }
  FastLED.show();
  delay(500);*/
  
  for (int x=START_LED; x<NUM_LEDS-4; x+=5) {
    leds[x] = leds[x+2] = CRGB::Red;
    leds[x+1] = CRGB::Orange;
    leds[x+3] = leds[x+4] = CRGB::Black;
  }
  FastLED.show();
  delay(50);
  for (int x=START_LED; x<NUM_LEDS-4; x+=5) {
    leds[x+4] = leds[x+1] = CRGB::Red;
    leds[x] = CRGB::Orange;
    leds[x+2] = leds[x+3] = CRGB::Black;
  }
  FastLED.show();
  delay(50);
  for (int x=START_LED; x<NUM_LEDS-4; x+=5) {
    leds[x+3] = leds[x] = CRGB::Red;
    leds[x+4] = CRGB::Orange;
    leds[x+1] = leds[x+2] = CRGB::Black;
  }
  FastLED.show();
  delay(50);
  for (int x=START_LED; x<NUM_LEDS-4; x+=5) {
    leds[x+2] = leds[x+4] = CRGB::Red;
    leds[x+3] = CRGB::Orange;
    leds[x] = leds[x+1] = CRGB::Black;
  }
  FastLED.show();
  delay(50);
  for (int x=START_LED; x<NUM_LEDS-4; x+=5) {
    leds[x+1] = leds[x+3] = CRGB::Red;
    leds[x+2] = CRGB::Orange;
    leds[x+4] = leds[x] = CRGB::Black;
  }
  FastLED.show();
  delay(50);
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
  for (int x=START_LED+NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT; x<NUM_LEDS; x++) {
    leds[x] = allianceColor;
  }
  FastLED.show();
  
  //front: yellow pulse, 1s (ex. yellow, yellow, yellow; off, off, off)
  int red=255, green=255, blue=0;
  bool ascending = false;
  while(mode==SEARCH_FEEDER) {
    if (ascending==true) {
      red+=5;
      green+=5;
    }
    else {
      red-=5;
      green-=5;
    }
  if (red==255||green==255) ascending = false;
  if (red==0||green==0) ascending = true;
  for (int x=START_LED; x<START_LED+NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT; x++) {
    leds[x] = CRGB(red, green, blue);
  }
  FastLED.show();
  delay(8);
}
}

void gearReady() {
  //MOVING
 
  //drivetrain: alliance color
  for (int x=START_LED+NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT; x<NUM_LEDS; x++) {
    leds[x] = allianceColor;
  }
  
  //front: green pulse, 1s (ex. green, green, green; off, off, off)
  int red=0, green=255, blue=0;
  bool ascending = false;
  while(mode==GEAR_READY) {
    if (ascending==true) {
      green+=5;
    }
    else {
      green-=5;
    }
  if (green==255) ascending = false;
  if (green==0) ascending = true;
  for (int x=START_LED; x<START_LED+NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT; x++) {
    leds[x] = CRGB(red, green, blue);
  }
  FastLED.show();
  delay(8);
}
}

void gearIn() {
  //STILL

  //drivetrain: alliance color
  for (int x=START_LED+NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT; x<NUM_LEDS; x++) {
    leds[x] = allianceColor;
  }
  FastLED.show();
  
  //front: green flash, 0.75s
  for (int x=START_LED; x<START_LED+NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT; x++) {
    leds[x] = CRGB::Green;
  }
  FastLED.show();
  delay(750);
}

void searchLift() {
  //MOVING
  
  //front: top 3 alliance color, rest white (ex. blue, blue, blue, white, white, white)
  leds[START_LED] = leds[START_LED+1] = leds[START_LED+2] = leds[START_LED+NUM_LEDS_FRONT_LEFT] = leds[START_LED+NUM_LEDS_FRONT_LEFT+1] = leds[START_LED+NUM_LEDS_FRONT_LEFT+2] = allianceColor;
  for (int x=START_LED+3; x<START_LED+NUM_LEDS_FRONT_LEFT; x++) {
    leds[x] = CRGB::White;
    leds[x+NUM_LEDS_FRONT_LEFT] = CRGB::White;
  }
  //drivetrain: alliance color pulse, 1s (ex. blue, blue, blue; off, off, off) DOES NOT WORK!!!!!!!!!!!!!
  int red, green=0, blue;
  if (allianceColor == CRGB(0,0,255)) {
    red = 0;
    blue = 255;
  }
  else {
    red = 255;
    blue = 0;
  }
  bool ascending = false;
  while(mode==SEARCH_LIFT) {
    if (allianceColor == CRGB(0,0,255)) {
    if (ascending==true) {
      blue+=5;
    }
    else {
      blue-=5;
    }
    }
    else {
      if (ascending==true) {
      red+=5;
    }
    else {
      red-=5;
    }
    }
    if (allianceColor == CRGB(0,0,255)) {
      if (blue==255) ascending = false;
      if (blue==0) ascending = true;
    }
    else {
      if (red==255) ascending = false;
      if (red==0) ascending = true;
    }
  for (int x=NUM_LEDS-NUM_LEDS_DRIVE; x<NUM_LEDS; x++) {
    leds[x] = CRGB(red, green, blue);
  }
  FastLED.show();
  delay(8);
}
}

void climb() {
  //MOVING
  
  //drivetrain: alliance color
  for (int x=START_LED+NUM_LEDS_FRONT_LEFT+NUM_LEDS_FRONT_RIGHT; x<NUM_LEDS; x++) {
    leds[x] = allianceColor;
  }
  
  //front: 3 light chase alternating up sides (ex. left: blue, white, blue, off, off, off; right: off, off, off, blue, white, blue)
  int lightSpeed = 65;
  for (int x=START_LED; x<NUM_LEDS-NUM_LEDS_DRIVE-6; x+=6) {
    leds[x] = leds[x+2] = allianceColor;
    leds[x+1] = CRGB::White;
    leds[x+3] = CRGB::Black;
    leds[x+4] = CRGB::Black;
    leds[x+5] = CRGB::Black;
  }
  leds[NUM_LEDS-NUM_LEDS_DRIVE-2] = allianceColor;
  leds[NUM_LEDS-NUM_LEDS_DRIVE-1] = CRGB::White;
  FastLED.show();
  delay(lightSpeed);
  for (int x=START_LED; x<NUM_LEDS-NUM_LEDS_DRIVE-6; x+=6) {
    leds[x+5] = leds[x+1] = allianceColor;
    leds[x] = CRGB::White;
    leds[x+2] = CRGB::Black;
    leds[x+3] = CRGB::Black;
    leds[x+4] = CRGB::Black;
  }
  leds[NUM_LEDS-NUM_LEDS_DRIVE-2] = CRGB::White;
  leds[NUM_LEDS-NUM_LEDS_DRIVE-1] = allianceColor;
  FastLED.show();
  delay(lightSpeed);
  for (int x=START_LED; x<NUM_LEDS-NUM_LEDS_DRIVE-6; x+=6) {
    leds[x+4] = leds[x] = allianceColor;
    leds[x+5] = CRGB::White;
    leds[x+1] = CRGB::Black;
    leds[x+2] = CRGB::Black;
    leds[x+3] = CRGB::Black;
  }
  leds[NUM_LEDS-NUM_LEDS_DRIVE-2] = allianceColor;
  leds[NUM_LEDS-NUM_LEDS_DRIVE-1] = CRGB::Black;
  FastLED.show();
  delay(lightSpeed);
  for (int x=START_LED; x<NUM_LEDS-NUM_LEDS_DRIVE-6; x+=6) {
    leds[x+3] = leds[x+5] = allianceColor;
    leds[x+4] = CRGB::White;
    leds[x] = CRGB::Black;
    leds[x+1] = CRGB::Black;
    leds[x+2] = CRGB::Black;
  }
  leds[NUM_LEDS-NUM_LEDS_DRIVE-2] = leds[NUM_LEDS-NUM_LEDS_DRIVE-1] = CRGB::Black;
  FastLED.show();
  delay(lightSpeed);
  for (int x=START_LED; x<NUM_LEDS-NUM_LEDS_DRIVE-6; x+=6) {
    leds[x+2] = leds[x+4] = allianceColor;
    leds[x+3] = CRGB::White;
    leds[x+5] = CRGB::Black;
    leds[x] = CRGB::Black;
    leds[x+1] = CRGB::Black;
  }
  leds[NUM_LEDS-NUM_LEDS_DRIVE-2] = leds[NUM_LEDS-NUM_LEDS_DRIVE-1] = CRGB::Black;
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
  switch (mode) {
    case TELEOP_DISABLED:
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
}
