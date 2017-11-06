
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

#define REGIS 70

volatile float regi[REGIS][3];


float Smoothed[6];


const int iAx = 0;
const int iAy = 1;
const int iAz = 2;
const int iGx = 3;
const int iGy = 4;
const int iGz = 5;


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

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



  // initialize device
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();

//  accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);

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

 for (int i = 0; i < REGIS; i++) {

    Serial.println("Registering");
  
 //   delay(250);
    getSmoothed();

    regi[i][0] = Smoothed[0];
    regi[i][1] = Smoothed[1];
    regi[i][2] = Smoothed[2];
  
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(10);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(10);    
    
 }


}

void getSmoothed(){
    int16_t RawValue[6];
    int i;
    const int N = 100;
    const int usDelay = 3150;
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
  



 for (int i = 0; i < REGIS; i++) {
    Smoothed[0] = regi[i][0];
    Smoothed[1] = regi[i][1];
    Smoothed[2] = regi[i][2];
    
    float g = Smoothed[0]*Smoothed[0]  + Smoothed[1]*Smoothed[1] + Smoothed[2]*Smoothed[2];
    String tmp;
    
    tmp = tmp + Smoothed[0] + "|" + Smoothed[1] + "|" + Smoothed[2] + "| \n";
    tmp = tmp + g + "\n";

    Serial.print(tmp);
 }  

 delay(1000);

  
}


