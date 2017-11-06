// Define which sensors are attached
//#define SENSORS_BMP180_ATTACHED
//#define SENSORS_HMC5883L_ATTACHED
#define SENSORS_MPU6050_ATTACHED
#include <Wire.h>
#include <Sensors.h>


void setup() {
    // Activate serial port (for debug printing)
    Serial.begin(9600);

    // Activate i2c
    Wire.begin();

    // Initialize devices
    Sensors::initialize();
}

void loop() {
    Accelerometer *accelerometer = Sensors::getAccelerometer();
    if(accelerometer) {
        Vector3 a = accelerometer->getAcceleration();
        Serial.print("Acceleration (m/s^2)  ");
        Serial.print(a.x);
        Serial.print(", ");
        Serial.print(a.y);
        Serial.print(", ");
        Serial.println(a.z);
    } else {
      Serial.println("No acc");
    }

    Gyroscope *gyroscope = Sensors::getGyroscope();
    if(gyroscope) {
        Vector3 g = gyroscope->getRotation();
        Serial.print("Rotation (rad/s)      ");
        Serial.print(g.x);
        Serial.print(", ");
        Serial.print(g.y);
        Serial.print(", ");
        Serial.println(g.z);
    } else {
      Serial.println("No gyro");
    }
    delay(500);
}
