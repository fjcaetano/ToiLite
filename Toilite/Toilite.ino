/*
  ToiLite Controller

  Author: Flavio Caetano
*/

#include <EEPROM.h>

#define RED_PIN PB4 // PIN 3
#define GREEN_PIN PB1 // PIN 6
#define BLUE_PIN PB0 // PIN 5

#define BUTTON PB2 // PIN 7

void setColorFromAngle(char angle);
void loop(char currentAngle);

bool isButtonPressed = false;

int main(void) {
  initVariant();
  
  // Setting output mode for pins
  DDRB |= (1 << RED_PIN);
  DDRB |= (1 << GREEN_PIN);
  DDRB |= (1 << BLUE_PIN);

  // Set BUTTON as pull-up
  PORTB |= (1 << BUTTON);

  char currentAngle = EEPROM.read(0);
  setColorFromAngle(currentAngle);

  if (currentAngle == 0) {
    currentAngle = analogRead(PB3);
  }

  while (true) {
    loop(currentAngle);
    currentAngle = (currentAngle + 1) % 256;
  }
}

void loop(char currentAngle) {
  if (EEPROM.read(0) == 0) {
    setColorFromAngle(currentAngle);
  }

  if ((PINB & (1 << BUTTON)) != 0) {
    isButtonPressed = false;
    _delay_ms(100);
    return;
  }

  if (!isButtonPressed) {
    isButtonPressed = true;
    EEPROM.write(0, (EEPROM.read(0) == 0) ? currentAngle : 0);
  }
}

void setColorFromAngle(char angle) {
  float sector = float(angle * 6) / 256;
  switch (int(floor(sector))) {
    case 0:
      analogWrite(RED_PIN, (sector - floor(sector)) * 0xff);
      analogWrite(GREEN_PIN, 0);
      analogWrite(BLUE_PIN, 0);
      break;

    case 1:
      analogWrite(RED_PIN, 0xff);
      // analogWrite(GREEN_PIN, 0);
      analogWrite(BLUE_PIN, (sector - floor(sector)) * 0xff);
      break;

    case 2:
      // analogWrite(RED_PIN, 0xff);
      analogWrite(GREEN_PIN, (sector - floor(sector)) * 0xff);
      analogWrite(BLUE_PIN, 0xff);
      break;

    case 3:
      analogWrite(RED_PIN, (1 - (sector - floor(sector))) * 0xff);
      analogWrite(GREEN_PIN, 0xff);
      // analogWrite(BLUE_PIN, 0xff);
      break;

    case 4:
      analogWrite(RED_PIN, 0);
      // analogWrite(GREEN_PIN, 0xff);
      analogWrite(BLUE_PIN, (1 - (sector - floor(sector))) * 0xff);

    case 5:
      // analogWrite(RED_PIN, 0);
      analogWrite(GREEN_PIN, (1 - (sector - floor(sector))) * 0xff);
      analogWrite(BLUE_PIN, 0);
      break;
  }
}