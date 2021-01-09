#include <HX711_ADC.h>
#include <EEPROM.h>

inline void debug(const char* line){
  Serial.println(line);
};


inline void error(const char* line){
  Serial.println(line);
};

bool calibrate(HX711_ADC& LoadCell, const int& eepromAdress ) {
  debug("Calibrating - current load will be equall to one unit");

  LoadCell.setCalFactor(1000.);
  float c = LoadCell.getData();
  float c1;
  LoadCell.setCalFactor(c);

  #if defined(ESP8266) 
  EEPROM.begin(512);
  #endif
  EEPROM.put(eepromAdress, c);
  #if defined(ESP8266)
  EEPROM.commit();
  #endif
  EEPROM.get(eepromAdress, c1);
  
  return c == c1;
}

void setup_one_cell(HX711_ADC& LoadCell, const int& eepromAdress ) {
  LoadCell.begin();
  long stabilisingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilising time
  LoadCell.start(stabilisingtime, true);
  if (LoadCell.getTareTimeoutFlag()) {
    error("Tare timeout, check MCU>HX711 wiring and pin designations");
  }
  else {


    float c1;

    EEPROM.get(eepromAdress, c1);

    if (c1 == c1) { //hope to detect NaN
      LoadCell.setCalFactor(c1); // user set calibration value (float)
    } else {
      LoadCell.setCalFactor(1000.); 
    }

    debug("Startup + tare is complete");
  }
  while (!LoadCell.update());
  
}



