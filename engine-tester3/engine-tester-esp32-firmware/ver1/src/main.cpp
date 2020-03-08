//-------------------------------------------------------------------------------------
// HX711_ADC.h
// Arduino master library for HX711 24-Bit Analog-to-Digital Converter for Weigh Scales
// Olav Kallhovd sept2017
// Tested with      : HX711 asian module on channel A and YZC-133 3kg load cell
// Tested with MCU  : Arduino Nano, ESP8266
//-------------------------------------------------------------------------------------
// This is an example sketch on how to use this library
// Settling time (number of samples) and data filtering can be adjusted in the config.h file

// This example shows how to calibrate the load cell and optionally save the calibration  
// value to EEPROM, and also how to change the value.
// The value can later be fetched from EEPROM in your project sketch.

#include <HX711_ADC.h>
#include <EEPROM.h>
#include <helpers.h>
#include <dataframe.h>
//HX711 constructor (dout pin, sck pin):

#define interruptPin 3
HX711_ADC LoadCell_a(4, 5);
HX711_ADC LoadCell_b(6, 7);

int eepromAdress_a = 0;
int eepromAdress_b = 0;

long t;


void setup(){
  
  Serial.begin(9600); 
  delay(10);
  
  Serial.println();

  setup_one_cell(LoadCell_a);
  setup_one_cell(LoadCell_b);

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), record_spin, RISING);

}

float t;
float millis_per_10spin = 250;
volatile long spins = 0;

void record_spin(){
  spins = spins + 1;
}

void loop() {
  //update() should be called at least as often as HX711 sample rate; >10Hz@10SPS, >80Hz@80SPS
  //longer delay in sketch will reduce effective sample rate (be carefull with delay() in the loop)
  noInterrupts();  
    LoadCell_a.update();
    LoadCell_b.update();
    const int current_spins = spins;
  interrupts();
  //get smoothed value from the data set

  millis_per_10spin = t / (float)current_spins;

  // 25000 rpm -> millis_per_10spin ~ 20
  const float dt = millis_per_10spin 
  if (millis() > t + dt) {

    DataFrame dataframe;

    dataframe.f1 = LoadCell_a.getData();
    dataframe.f1 = LoadCell_b.getData();

    noInterrupts();  
    if (spins > 10)
    interrupts();

    dataframe.timestamp = millis();

    Serial.write( (byte*)&dataframe, sizeof(DataFrame) );

    t = millis();
  }

  //receive from serial terminal
  if (Serial.available() > 0) {
    float i;
    char inByte = Serial.read();
    bool status_ok = true;

    DataFrame dataframe;
    dataframe.f1 = -1;
    dataframe.f1 = -1;
    dataframe.timestamp = millis();    

    switch (inByte)
    {
      case 't':
          LoadCell_a.tareNoDelay();
          LoadCell_b.tareNoDelay(); 
          //check if last tare operation is complete
          while (!LoadCell_a.getTareStatus() || !LoadCell_b.getTareStatus()) {
            debug("Tare in progress  v v v");
          }
                    
        break;
      case 'c':
          bool status_ok &= calibrate(LoadCell_a, eepromAdress_a);
          status_ok &= calibrate(LoadCell_b, eepromAdress_b);
          dataframe.status = status_ok ? 't' : 'e';
        break;
      
      default:
        break;
    }
  }



}