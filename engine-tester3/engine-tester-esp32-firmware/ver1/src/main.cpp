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

#define interruptPin D7
const float millis_per_10samples = 100;
HX711_ADC LoadCell_a(D0, D1);
HX711_ADC LoadCell_b(D3, D4);

int eepromAdress_a = 0;
int eepromAdress_b = 1;

long t;


//float millis_per_10spin = 250;
volatile unsigned long spins = 0;

void ICACHE_RAM_ATTR record_spin(){
  noInterrupts();  
  spins = spins + 1;
  interrupts();  
}

// ############################################################################################
// ############################################################################################
// ############################################################################################
// ############################################################################################
// ############################################################################################

void setup(){
  
  Serial.begin(9600); 
  delay(10);
  
  Serial.println();

  setup_one_cell(LoadCell_a, eepromAdress_a);
  setup_one_cell(LoadCell_b, eepromAdress_b);

  //pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), record_spin, RISING);

}

long i = 0;
void loop() {
  i = i + 1;
  //update() should be called at least as often as HX711 sample rate; >10Hz@10SPS, >80Hz@80SPS
  //longer delay in sketch will reduce effective sample rate (be carefull with delay() in the loop)
  static boolean newDataReady = 0;
  unsigned long current_spins = 0;
  if (LoadCell_a.update() && LoadCell_b.update()) newDataReady = true;
  
  const unsigned long current_time = millis();


  // ######################################################################
 // if (current_spins > 10) {
 if (newDataReady && i > 50000) {
    i = 0;
    current_spins = spins;
    DataFrame dataframe;
    dataframe.f1 = -1;
    dataframe.f2 = -1;
    dataframe.timestamp = current_time;
    dataframe.rpms = (float)current_spins / (float)(current_time - t) * 60000.;
    dataframe.spins = current_spins;
    dataframe.status = 'd';
    noInterrupts();
    //spins = 0;
       dataframe.f1 = LoadCell_a.getData();
       dataframe.f2 = LoadCell_b.getData();
       spins = 0;
    interrupts();
 
    serialWriteData(dataframe);

    t = millis();

  }


  // ######################################################################
  //receive from serial terminal
  if (Serial.available() > 0) {
    
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
          status_ok &= calibrate(LoadCell_a, eepromAdress_a);
          status_ok &= calibrate(LoadCell_b, eepromAdress_b);
        break;
      
      default:
        break;
    }

    dataframe.status = status_ok ? 't' : 'e';

    serialWriteData(dataframe);

  }
}