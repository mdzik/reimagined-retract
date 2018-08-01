// For calibrating you may use the following code snipet
#include <Servo.h>
#define MOTOR_PIN 9

Servo motor;

void setup() {
  Serial.begin(9600);
  Serial.println("Program begin, press to start...");
  // Wait for input
  

  
  motor.attach(MOTOR_PIN);
  
}



void loop() {  


 if (Serial.available()) {
    int value = Serial.parseInt();
    motor.writeMicroseconds(value);
    Serial.print("v: ");
    Serial.println(value);
 }


}
