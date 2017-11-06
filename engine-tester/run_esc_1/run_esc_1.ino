// For calibrating you may use the following code snipet
#include <Servo.h>
#define MAX_SIGNAL 2000
#define MIN_SIGNAL 500
#define MOTOR_PIN 9

Servo motor;
//#define CALIBRATE

void setup() {
  Serial.begin(9600);
  Serial.println("Program begin, press to start...");
  // Wait for input
  

  
  motor.attach(MOTOR_PIN);

  #ifdef CALIBRATE
    Serial.println("This program will calibrate the ESC.");
    Serial.println("Now writing maximum output.");
    Serial.println("Turn on power source, then wait 2 seconds and press any key.");
    motor.writeMicroseconds(MAX_SIGNAL);
  
    // Wait for input
    while (!Serial.available());
    Serial.read();

    // Send min output
    Serial.println("Sending minimum output. Press key to start");
    motor.writeMicroseconds(MIN_SIGNAL);
  #endif
  
  delay(1000);
  
  while (!Serial.available());
  Serial.read();
  
}


const int th_min = 25;
const int th_max = 50;


void loop() {  
int throttle = th_min;

const int dt = 3;

for (; throttle < th_max; throttle+=dt) {
  
  motor.write(throttle); // throttle value define the speed of esc
  delay(2000);
  Serial.println(throttle);
}
for (; throttle > th_min; throttle-=dt) {
  
  motor.write(throttle); // throttle value define the speed of esc
  delay(2000);
  Serial.println(throttle);
}
Serial.println("Test ended. press to restart");
    
  while (!Serial.available());
  Serial.read();
}
