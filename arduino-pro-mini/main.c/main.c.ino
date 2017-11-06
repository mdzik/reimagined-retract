///////////

//const float gOpen = 1.2;
//const float gClose = 5.5;

const float gOpen = 3;
const float gClose = 2;


///////////




// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high









#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

volatile int gearPos = 0;    // variable to store the servo position

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 6;     // the number of the pushbutton pin
const int ledPin = 5;     // the number of the pushbutton pin
const int hallSensorPin = A0;

// variables will change:
volatile int gearState = 0;    
volatile int hallState = 0;    
int buttonState = 0;
int hallSensorState = 0;

float Smoothed[6];

int gMin = 90;
int gMax = 45;

const int iAx = 0;
const int iAy = 1;
const int iAz = 2;
const int iGx = 3;
const int iGy = 4;
const int iGz = 5;


void setup() {
  myservo.attach( 10 );  // attaches the servo on pin 9 to the servo object
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

   
  
  // start serial port at 9600 bps:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  // join I2C bus (I2Cdev library doesn't do this automatically)
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin();
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
  #endif

  Serial.println("g min");
  gearState = gMin;
  myservo.write(gearState);  
  delay(1000);
  gearPos = gearState;

  Serial.println("g max");
  gearState = gMax;
  myservo.write(gearState);  
  delayMicroseconds(1000);
  gearPos = gearState;


  // initialize device
  Serial.println("Initializing I2C devices...");
  accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);
  accelgyro.initialize();

  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  accelgyro.setXAccelOffset(237);
  accelgyro.setYAccelOffset(2546);
  accelgyro.setZAccelOffset(708);
  accelgyro.setXGyroOffset (59);
  accelgyro.setYGyroOffset (-34);
  accelgyro.setZGyroOffset (11);

  //attachInterrupt(0, hall_fun, RISING);  

}

void getSmoothed(){
    int16_t RawValue[6];
    int i;
    const int N = 100;
    const int usDelay = 30;
    long Sums[6];
    for (i = iAx; i <= iGz; i++){ 
      Sums[i] = 0;
    }


    for (i = 1; i <= N; i++)
      { // get sums
        accelgyro.getMotion6(&RawValue[iAx], &RawValue[iAy], &RawValue[iAz], 
                             &RawValue[iGx], &RawValue[iGy], &RawValue[iGz]);
        if ((i % 500) == 0)
          delayMicroseconds(usDelay);
        for (int j = iAx; j <= iGz; j++)
          Sums[j] = Sums[j] + RawValue[j];
      } // get sums
//    unsigned long usForN = micros() - Start;
//    Serial.print(" reading at ");
//    Serial.print(1000000/((usForN+N/2)/N));
//    Serial.println(" Hz");
    
    for (i = iAx; i <= iGz; i++)
      { Smoothed[i] = float((Sums[i] + N/2) / N) / 16400. ; }
  } // GetSmoothed




void loop() {
  pinMode(LED_BUILTIN, OUTPUT);  
  
  if (gearPos != gearState){
    myservo.write(gearPos);  
    delayMicroseconds(10);
    gearPos = gearState;
    Serial.println("state change \n");

    if ( gearState == gMin ) {
      for (int i = 0; i < 2000; i++){
        digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off by making the voltage LOW      
        delayMicroseconds(1000);
        digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)    
     }
    }
      
    
  }

  String tmp;
  //for (int i = 0; i < hallSensorState; i++) {
  //  tmp = tmp + '+';
  //}

  getSmoothed();
  float g = Smoothed[0]*Smoothed[0]  + Smoothed[1]*Smoothed[1] + Smoothed[2]*Smoothed[2];
  tmp = tmp + Smoothed[0] + " | " + Smoothed[1] + " | " + Smoothed[2] + " | " +  '\n';
  tmp = tmp + g + "g \n";

  if ( g >= gClose ) {  
    Serial.println("g > gClose - closing");
    gearState = gMax;  
  }
  if ( g < gOpen ) {      
    Serial.println("g > gClose - opening");
    gearState = gMin;  
  }
  
  Serial.write(tmp.c_str());
  
}

void hall_fun(){
    gearState = gMin;      
    hallState = 1;
}
