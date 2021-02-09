#include "LedControl.h" //The library for controlling the MAX7219
/*
   Setting up MAX:
   pin 7 for DataIn
   pin 6 for CS
   pin 5 for CLK
*/

LedControl lc = LedControl(7, 5, 6, 1);

unsigned long delaytime = 500;

const int motorPin0 = 8;
const int motorPin1 = 9;
const int btnPin0 = 2;
const int btnPin1 = 3;

bool btnState0 = LOW;
bool btnState1 = LOW;

void setup() {
  pinMode(motorPin0, OUTPUT);
  pinMode(motorPin1, OUTPUT);
  pinMode(btnPin0, INPUT);
  pinMode(btnPin1, INPUT);
  //pinMode(ledPin, OUTPUT);


  // Wakeup call for MAX7219
  lc.shutdown(0, false);
  // Set the brightness
  lc.setIntensity(0, 8);
  // Clear the display
  lc.clearDisplay(0);
}
void loop() {
  static byte* dot = makeDot();
  write8(dot);
  
  btnState0 = digitalRead(btnPin0);
  btnState1 = digitalRead(btnPin1);
  
  if (btnState0 == HIGH) {
    //Forwards
    digitalWrite(motorPin0, HIGH);
    forwards(dot);
  } else if (btnState1 == HIGH) {
    //Backwards
    digitalWrite(motorPin1, HIGH);
    backwards(dot);
  } else {
    //No input
    digitalWrite(motorPin0, LOW);
    digitalWrite(motorPin1, LOW);
  }
}
byte* makeDot() {
  uint8_t arrSize = 8;
  byte* dot = new byte[arrSize];

  dot[0] = B00000000;
  dot[1] = B00001000;
  dot[2] = B00000100;
  dot[3] = B00111110;
  dot[4] = B00111110;
  dot[5] = B00000100;
  dot[6] = B00001000;
  dot[7] = B00000000;

  return dot;
}
void forwards(byte* a) {
  uint8_t arrSize = 8;
  bool isOdd = 0;

  //Bitshift every byte 1 to the right
  //But copying over the least significant bit to the most significant position
  for (uint8_t i = 0; i < arrSize; i++) {
    //Using mod(2) to determine the least significant bit (if odd -> 1)
    isOdd = a[i] % 2;

    //Bitshifting to the right (If we loose a bit, it is stored in isOdd)
    a[i] = a[i] >> 1;

    //Placing the old least significant bit in the most significant position
    //We use 8bit precision (2^7) becuase we only operate on bytes
    a[i] = a[i] + (isOdd * pow(2, 7));
  }
}
void backwards(byte* a) {
  uint8_t arrSize = 8;
  bool has8thBit = 0;

  //Bitshift every byte 1 to the left
  //But copying over the most significant bit to the least significant position
  for (uint8_t i = 0; i < arrSize; i++) {
    //Using AND on the most significant bit will give us whether there is a 1 or not there
    has8thBit = a[i] & B10000000;

    //Bitshifting to the left (If we loose a bit, it is stored in has8thBit)
    a[i] = a[i] << 1;

    //Add the remaing stored bit
    a[i] = a[i] + has8thBit;
  }
}
void write8(byte a[]) {
  for (int i = 0; i < 8; i++) {
    lc.setRow(0, i, a[i]);
  }
  delay(delaytime);
}
