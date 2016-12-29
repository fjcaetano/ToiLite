/*
  ToiLite Controller

  Author: Flavio Caetano
*/

typedef struct RGB {
  byte r;
  byte g;
  byte b;
};

RGB rgb(int h);
void setColor(RGB rgb);

const int redPin = 11; // PIN 3
const int bluePin = 10; // PIN 5
const int greenPin = 9  ; // PIN 6

const int BUTTON = 2; // PIN 7
int buttonPreviousRead = LOW;
int isFixed = false;

const int PIR = 4;

RGB color;

void setup()
{
  Serial.begin(9600);
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  pinMode(BUTTON, INPUT);
  pinMode(PIR, INPUT);
}

void loop()
{
  for (int i = 0; i < 3600; i++) {
    int button = digitalRead(BUTTON);

    if (button == HIGH && buttonPreviousRead != HIGH) {
      isFixed = !isFixed;
    }

    buttonPreviousRead = button;

    Serial.println(digitalRead(PIR));
    Serial.print(button);
    Serial.print(" - ");
    Serial.println(isFixed);

    if (!isFixed) {
      color = rgb(i / 10);
    }

    setColor(color);

    delay(10);
  }
}

void setColor(RGB rgb)
{
  analogWrite(redPin, rgb.r);
  analogWrite(greenPin, rgb.g);
  analogWrite(bluePin, rgb.b);
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
