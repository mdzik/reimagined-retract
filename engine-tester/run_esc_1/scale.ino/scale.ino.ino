/* sample for digital weight scale of hx711, display with a HD44780 liquid crtstal monitor
 *
 * hardware design: syyyd
 * available at http://syyyd.taobao.com
 *
 * library design: Weihong Guan (@aguegu)
 * http://aguegu.net
 *
 * library host on
 * https://github.com/aguegu/Arduino
 */

// Hx711.DOUT - pin #A1
// Hx711.SCK - pin #A0

#include "hx711.h"
#include <ArduinoJson.h>

Hx711 scale(A1, A0);
long offset = 0;

void setup() {

  Serial.begin(19200);
  Serial.println("{s: w}");  
  while (!Serial.available());
  
  offset = scale.averageValue();

}

char buffer[200];
float reg = 0;


const byte buffSize = 200;

char inputBuffer[buffSize];



void loop() {

  StaticJsonBuffer<200> jsonBuffer;
    
  byte maxChars = buffSize - 1; // use full size of buffer for this function

  byte charCount = 0; 
  byte ndx = 0;       
    
  if (Serial.available() > 0) {
    while (Serial.available() > 0) {
      if (ndx > maxChars - 1) {
        ndx = maxChars;
      }
      inputBuffer[ndx] = Serial.read();
    
      ndx ++;       
      charCount ++;
    }

    JsonObject& root = jsonBuffer.parseObject(inputBuffer);
  
    // Test if parsing succeeds.
    if (!root.success()) {
      Serial.print( inputBuffer );
      Serial.print( "  " );
      Serial.print( ndx );
      Serial.println(" - parseObject() failed");
    
    } else {
      reg = root["reg"];  
    }    
    for (int i =0; i < buffSize; i++){
      inputBuffer[i] = 0x0;  
    }
  
    JsonObject& root2 = jsonBuffer.createObject();
  
    // In other case, you can do root.set<long>("time", 1351824120);
    root2["f"] = (float)(scale.getValue() - offset) / 742.f;
    root2["time"] = millis();
    root2["reg"] = reg;
  
    root2.printTo(Serial);
    Serial.println();
  }

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   
    



