/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
//  pinMode(LED_BUILTIN, OUTPUT);
    
  myservo.attach(10);  // attaches the servo on pin 9 to the servo object
}

void loop() {

 //myservo.write(60);
 //for (pos = 25; pos <= 85; pos += 10) { // goes from 180 degrees to 0 degrees
 //   myservo.write(pos);              // tell servo to go to position in variable 'pos'
 
 //
 //}
// digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW  
 myservo.write(0);

 delay(4000);                       // waits 15ms for the servo to reach the position

// digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
 myservo.write(200);

 delay(4000);                       // waits 15ms for the servo to reach the position
 
}

