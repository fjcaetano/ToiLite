/*
  ToiLite Controller

  Author: Flavio Caetano
*/

#include <EEPROM.h>

#define RED_PIN PB4 // PIN 3
#define GREEN_PIN PB1 // PIN 6
#define BLUE_PIN PB0 // PIN 5

#define BUTTON PB2 // PIN 7

#define MAX_ANGLE 256

typedef struct RGB {
  char r;
  char g;
  char b;
};

RGB rgb(char h);
void setColorFromAngle(char angle);
void button_pressed();
void loop_colors(char startAngle);

char currentAngle;
bool shouldStopLooping = false;

void setup()
{
//  pinMode(RED_PIN, OUTPUT);
//  pinMode(GREEN_PIN, OUTPUT);
//  pinMode(BLUE_PIN, OUTPUT);

  // Setting output mode for pins
  DDRB |= (1 << RED_PIN);
  DDRB |= (1 << GREEN_PIN);
  DDRB |= (1 << BLUE_PIN);

//  pinMode(BUTTON, INPUT);

  randomSeed(analogRead(0));

  // FOR ATTINY85
  GIMSK = 0b00100000;    // turns on pin change interrupts
  PCMSK = 0b00000100;    // turn on interrupts on pins PB2
  sei();                 // enables interrupts

  currentAngle = EEPROM.read(0);
  setColorFromAngle(currentAngle);

  attachInterrupt(BUTTON, button_pressed, RISING);

  if (currentAngle == 0) {
    loop_colors(random(MAX_ANGLE));
  }
}

void loop() {
  // Empty
}

void button_pressed() {
  if (EEPROM.read(0) == 0) {
    char angle = currentAngle;
    EEPROM.write(0, angle);

    shouldStopLooping = true;
  }
  else {
    // Clears EEPROM
    EEPROM.write(0, 0);

    loop_colors(currentAngle);
  }
}

void loop_colors(char startAngle) {
  shouldStopLooping = false;

  for (char i = startAngle; i < MAX_ANGLE; i++) {
    if (shouldStopLooping) {
      return;
    }

    currentAngle = i;
    setColorFromAngle(i);

    delay(100);
  }

  loop_colors(0);
}

void setColorFromAngle(char angle)
{
  RGB color = rgb(angle);

 analogWrite(RED_PIN, color.r);
//  sbi(TCCR0A, COM0A1);
//  OCR0A = color.r;

//  analogWrite(GREEN_PIN, color.g);
//  sbi(TCCR0A, COM0B1);
//  OCR0B = color.g;

//  analogWrite(BLUE_PIN, color.b);
//  sbi(TCCR0A, COM0A1);
//  OCR0A = color.b;
}


RGB rgb(char angle)
{
  float sector = float(angle % MAX_ANGLE) / float(MAX_ANGLE / 6); // Sector
  char i = floor(sector);
  float f = sector - i; // Factorial part of h

  float q = (1 - f);

  switch (i) {
    case 0:
      return {0xff, char(f * 0xff), 0};

    case 1:
      return {char(q * 0xff), 0xff, 0};

    case 2:
      return {0, 0xff, char(f * 0xff)};

    case 3:
      return {0, char(q * 0xff), 0xff};

    case 4:
      return {char(f * 0xff), 0, 0xff};

    default:
      return {0xff, 0, char(q * 0xff)};
  }
}

