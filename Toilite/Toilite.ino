/*
  ToiLite Controller

  Author: Flavio Caetano
*/

#include <EEPROM.h>

typedef struct RGB {
  byte r;
  byte g;
  byte b;
};

RGB rgb(int h);
void setColorFromAngle(int angle);
void button_pressed();
void loop_colors(int startAngle);

const int RED_PIN = 4; // PIN 3
const int GREEN_PIN = 1; // PIN 6
const int BLUE_PIN = 0; // PIN 5

const int BUTTON = 2; // PIN 7

int currentAngle;
bool shouldStopLooping = false;

void setup()
{
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  pinMode(BUTTON, INPUT);

  randomSeed(analogRead(0));

  // FOR ATTINY85
  GIMSK = 0b00100000;    // turns on pin change interrupts
  PCMSK = 0b00000100;    // turn on interrupts on pins PB2
  sei();                 // enables interrupts

  int addr0 = EEPROM.read(0);
  int addr1 = EEPROM.read(1);

  currentAngle = (addr0 + (addr1 << 8));
  setColorFromAngle(currentAngle);

  attachInterrupt(digitalPinToInterrupt(BUTTON), button_pressed, RISING);

  if (currentAngle == 0) {
    loop_colors(random(360));
  }
}

void loop() {
  // Empty
}

void button_pressed() {
  if (EEPROM.read(0) == 0) {
    int angle = currentAngle;
    EEPROM.write(0, angle % 256);
    EEPROM.write(1, angle >> 8);

    shouldStopLooping = true;
  }
  else {
    // Clears EEPROM
    EEPROM.write(0, 0);
    EEPROM.write(1, 0);

    loop_colors(currentAngle);
  }
}

void loop_colors(int startAngle) {
  shouldStopLooping = false;

  for (int i = startAngle; i < 360; i++) {
    if (shouldStopLooping) {
      return;
    }

    currentAngle = i;
  	setColorFromAngle(i);

    delay(100);
  }

  loop_colors(0);
}

void setColorFromAngle(int angle)
{
  RGB color = rgb(angle);
  analogWrite(RED_PIN, color.r);
  analogWrite(GREEN_PIN, color.g);
  analogWrite(BLUE_PIN, color.b);
}


RGB rgb(int h)
{
  int angle = (h >= 360 ? 0 : h);
  float sector = float(angle) / 60; // Sector
  int i = floor(sector);
  float f = sector - i; // Factorial part of h

  float q = (1 - f);

  switch (i) {
    case 0:
      return {0xff, int(f * 0xff), 0};

    case 1:
      return {int(q * 0xff), 0xff, 0};

    case 2:
      return {0, 0xff, int(f * 0xff)};

    case 3:
      return {0, int(q * 0xff), 0xff};

    case 4:
      return {int(f * 0xff), 0, 0xff};

    default:
      return {0xff, 0, int(q * 0xff)};
  }
}
