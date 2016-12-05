/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>
#define rPin 6
#define gPin 5
#define bPin 3
#define s1Pin 9
#define s2Pin 10

Servo myservo1;  
Servo myservo2;

int p1 = 8;    // Lower Limit
int p2 = 88;    // Upper Limit
int pos = 0;    // variable to store the servo position
int posServo1 = 0;
int posServo2 = 0;
int k = (p2 - p1) / 4;
int offset = p1 + 2*k;
float xScaled;
float yScaled;
float prePosServo1 = 0;
float prePosServo2 = 0;

void setup() {
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  myservo1.attach(9);  
  myservo2.attach(10);
  Serial.begin(115200);
  Serial.print(k);
  Serial.print(offset);
}

int clear() {
  while(Serial.peek() == ' ' || Serial.peek() == '\r') {
    Serial.read();
  }
  return Serial.available();
}

int getInt(int& x) {
  char next_char = Serial.peek();
  if (next_char == '\n') {
//    Serial.println("hit enter, reset!");
    return -1;
  } else {
    x = Serial.parseInt();
    return 0;
  }
}

void loop() {
 if (clear()>0) {
  if (Serial.peek() == '\n') {
    Serial.read();
    return;
  }
  Serial.println("loop");
  int r, g, b, x, y;

  r = Serial.parseInt(); // 0 to 255
  if(getInt(g)) return; // 0 to 255
  if(getInt(b)) return; // 0 to 255
  if(getInt(x)) return; // 0 to 100
  if(getInt(y)) return; // 0 to 100
  
//  Serial.print(r);
//  Serial.print(g);
//  Serial.print(b);
//  Serial.print(x);
//  Serial.println(y);
 
  /* Scale position */
  xScaled = (float)x / 50 - 1;    // -1 to 1
  yScaled = (float)y / 50 - 1;    // -1 to 1
  prePosServo1 = -xScaled + yScaled;    // -2 to 2
  prePosServo2 = -xScaled - yScaled;    // -2 to 2
  posServo1 = (float)k * prePosServo1 + offset;    // 34 to 90
  posServo2 = (float)k * prePosServo2 + offset;    // 34 to 90

  Serial.println(xScaled);
  Serial.println(yScaled);
  Serial.println(prePosServo1);
  Serial.println(prePosServo2);
  Serial.println(posServo1);
  Serial.println(posServo2);
  Serial.println(k);
  Serial.println(b);

  /* Write values*/
  myservo1.write(posServo1);
  myservo2.write(posServo2);
  analogWrite(rPin, r);
  analogWrite(gPin, g);
  analogWrite(bPin, b);
 }
}
