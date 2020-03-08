// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <YetAnotherDebouncer.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

volatile  int pressed = 0;

void debounce_callback(bool value, int* i) {
    Serial1.print("Button #");
    Serial1.print(*i);
    Serial1.print(" changed to ");
    Serial1.println(value? "OFF" : "ON");
    if (value) {
      pressed++;
    }
}


int input_pins[] = {4};
Debouncer<Clock::Millis> debouncers[ARRAY_SIZE(input_pins)];



OneWire oneWire(A1); //Podłączenie do A5
DallasTemperature sensors(&oneWire); //Przekazania informacji do biblioteki

#include "DHT.h"

#define DHTPIN A0    // Digital pin connected to the DHT sensor
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);


// digital pin 2 has a pushbutton attached to it. Give it a name:
int pushButton = 4;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial1.begin(9600);
  // make the pushbutton's pin an input:
  pinMode(pushButton, OUTPUT);  
  Wire.begin(); // join i2c bus (address optional for master)
  sensors.begin(); //Inicjalizacja czujnikow
  dht.begin();
  Serial1.println("Good morning");

//////////////////////////////////

  for (int i=0; i<ARRAY_SIZE(input_pins); i++) {
      int debounce = i*i*25;
      
      pinMode(input_pins[i], INPUT_PULLUP);
      debouncers[i].begin();
      debouncers[i].set_debounce_stable(debounce);
      debouncers[i].set_callback(debounce_callback, &input_pins[i]);

      Serial1.print("Pin #");
      Serial1.print(input_pins[i]);
      Serial1.print(" initialized with ");
      Serial1.print(debounce);
      Serial1.println("ms of debounce"); 
  }

}

byte x = 0;

void loop() {

  digitalWrite(pushButton,LOW);
  digitalWrite(pushButton,HIGH);
  sensors.requestTemperatures(); //Pobranie temperatury czujnika


  ////////////////
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  float t2 = sensors.getTempCByIndex(0);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(t2)) {
    Serial1.println(F("Failed to read data!"));
    return;
  }
    
  digitalWrite(pushButton,LOW);
  
  Wire.beginTransmission(8); // transmit to device #8
  
  Wire.write(x);              // sends one byte
  Wire.write((int)((t*1.)/1)); 
  Wire.write(((int)(t*100)%100)); 
  
  Wire.write((int)((t2*1.)/1));
  Wire.write(((int)(t2*100)%100)); 
  
  Wire.write((int)((h*1.)/1));
  Wire.write(((int)(h*100)%100)); 

  Wire.write(pressed); 
  
  Wire.endTransmission();    // stop transmitting
    

  // print out the state of the button:
  Serial1.println(x);
  Serial1.println((int)((t*10.)/1)); 
  Serial1.println((int)((t2*10.)/1));
  Serial1.println((int)((h*10.)/1));

  Serial1.println(t); 
  Serial1.println(t2);
  Serial1.println(h);
  Serial1.println(pressed);
  
  
  x++;

  
  digitalWrite(pushButton,LOW);
  digitalWrite(pushButton,HIGH);
  digitalWrite(pushButton,LOW);
  
  digitalWrite(pushButton,LOW);
  digitalWrite(pushButton,HIGH);
  digitalWrite(pushButton,LOW); 

  for (int i=0; i<ARRAY_SIZE(input_pins); i++) {
        debouncers[i].set_value(digitalRead(input_pins[i]));
  }  


 }
