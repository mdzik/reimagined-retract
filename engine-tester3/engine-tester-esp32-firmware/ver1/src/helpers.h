#include <HX711_ADC.h>
#include <EEPROM.h>

inline void debug(const char* line){
  Serial.println(line);
};

bool calibrate(HX711_ADC& LoadCell, const int& eepromAdress ) {
  debug("***");
  debug("Start calibration:");
  debug("It is assumed that the mcu was started with no load applied to the load cell.");
  debug("Now, place your known mass on the loadcell,");
  debug("then send the weight of this mass (i.e. 100.0) from serial monitor.");
  float m = 0;
  boolean f = 0;
  while (f == 0) {
    LoadCell.update();
    if (Serial.available() > 0) {
      m = Serial.parseFloat();
      if (m != 0) {
        Serial.print("Known mass is: ");
        Serial.println(m);
        f = 1;
      }
      else {
        debug("Invalid value");
        return false;
      }
    }
  }
  float c = LoadCell.getData() / m;
  LoadCell.setCalFactor(c);
  Serial.print("Calculated calibration value is: ");
  Serial.print(c);
  debug(", use this in your project sketch");
  f = 0;
  Serial.print("Save this value to EEPROM adress ");
  Serial.print(eepromAdress);
  debug("? y/n");
  while (f == 0) {
    if (Serial.available() > 0) {
      char inByte = Serial.read();
      if (inByte == 'y') {
        #if defined(ESP8266) 
        EEPROM.begin(512);
        #endif
        EEPROM.put(eepromAdress, c);
        #if defined(ESP8266)
        EEPROM.commit();
        #endif
        EEPROM.get(eepromAdress, c);
        Serial.print("Value ");
        Serial.print(c);
        Serial.print(" saved to EEPROM address: ");
        Serial.println(eepromAdress);
        f = 1;

      }
      else if (inByte == 'n') {
        debug("Value not saved to EEPROM");
        return false;
        f = 1;
      }
    }
  }
  debug("End calibration");
  debug("For manual edit, send 'c' from serial monitor");
  debug("***");

  return true;
}

void setup_one_cell(HX711_ADC& LoadCell) {
  LoadCell.begin();
  long stabilisingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilising time
  LoadCell.start(stabilisingtime);
  if (LoadCell.getTareTimeoutFlag()) {
    error("Tare timeout, check MCU>HX711 wiring and pin designations");
  }
  else {
    LoadCell.setCalFactor(1.0); // user set calibration value (float)
    debug("Startup + tare is complete");
  }
  while (!LoadCell.update());
  
}
