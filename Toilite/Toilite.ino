/*
  ToiLite Controller

  Author: Flavio Caetano
*/

#include <EEPROM.h>

#define RED_PIN PB4 // PIN 3
#define GREEN_PIN PB1 // PIN 6
#define BLUE_PIN PB0 // PIN 5

#define BUTTON PB2 // PIN 7

typedef struct RGB {
  byte r;
  byte g;
  byte b;
};

RGB rgb(int h);
void setColorFromAngle(int angle);
void button_pressed();
void loop_colors(int startAngle);

int currentAngle;
volatile bool shouldStopLooping = false;

int main(void) {
  init();
  initVariant();

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(A3, INPUT);

  randomSeed(analogRead(A3));

  // FOR ATTINY85
  GIMSK |= (1 << PCIE);    // turns on pin change interrupts
  PCMSK |= (1 << PCINT2);    // turn on interrupts on pins PB2
  sei();                 // enables interrupts

  int addr0 = EEPROM.read(0);
  int addr1 = EEPROM.read(1);

  currentAngle = (addr0 + (addr1 << 8));

  if (currentAngle != 0) {
    setColorFromAngle(currentAngle);
    shouldStopLooping = true;
  }
  else {
    currentAngle = random(360);
  }

  while (true) {
    loop_colors(currentAngle);
  }
}

ISR(PCINT0_vect) {
  if (digitalRead(BUTTON) == HIGH) return;
  
  if (EEPROM.read(0) == 0) {
    EEPROM.write(0, currentAngle % 256);
    EEPROM.write(1, currentAngle >> 8);

    shouldStopLooping = true;
  }
  else {
    // Clears EEPROM
    EEPROM.write(0, 0);
    EEPROM.write(1, 0);

    shouldStopLooping = false;
  }
}

void loop_colors(int startAngle) {
  for (currentAngle = startAngle; currentAngle < 360; currentAngle++) {
    if (shouldStopLooping) {
      return;
    }

    setColorFromAngle(currentAngle);

    delay(100);
  }

  currentAngle = 0;
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
