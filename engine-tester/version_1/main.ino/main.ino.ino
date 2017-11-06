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

#include <Servo.h>
#define MOTOR_PIN 9

Servo motor;


#include <ArduinoJson.h>

Hx711 scale(A1, A0);
long offset = 0;

long scale_average = 0;
byte scale_average_num = 0;


char inputBuffer[200];
float reg_speed = 0;
long reg_id = 0;
const byte buffSize = 200;
long ndx = 0;

const int th_min = 0;
const int th_max = 70;
int throttle = 0;


    
#define PRINT(T) Serial.println(T);
#define PRINT(T);

void dispatch( JsonObject& response, JsonObject& request ){
    
    if (strcmp(request.get<const char*>("r"), "tare") == 0){
        PRINT("//tare");    
        offset = scale.averageValue();
    }

    if (strcmp(request.get<const char*>("r"), "th") == 0){
        PRINT("//throttle");    
        int throttle2 = map(request.get<long>("th"), 0,100,th_min, th_max);
        int ii = 0;
        if (throttle2 > throttle) {
          for (ii = throttle; ii <throttle2; ii++){ 
            PRINT(ii);    
              
            motor.write(ii); // throttle value define the speed of esc          
            delay(1000);
          }
        } else {
          for ( ii = throttle; ii > throttle2; ii--){ 
            PRINT(ii);    
            motor.write(ii); // throttle value define the speed of esc          
            delay(1000);
          }          
        }
        throttle = ii;
    }

    if (strcmp(request.get<const char*>("r"), "stop") == 0){
        PRINT("//stop");    
        
        int ii = 0;
        for ( ii = throttle; ii > 0; ii--){ 
          PRINT(ii);    
          motor.write(ii); // throttle value define the speed of esc          
          delay(100);
        }          
        throttle = 0;
      }
      
    if (strcmp(request.get<const char*>("r"), "start") == 0){
        PRINT("//stop");    
        throttle = 63;
        int ii = 0;
        for ( ii = 0; ii < throttle; ii++){ 
          PRINT(ii);    
          motor.write(ii); // throttle value define the speed of esc          
          delay(10);
        }          
      }
  
    // In other case, you can do root.set<long>("time", 1351824120);
    response["f"] = ((float)scale_average/scale_average_num - offset) / 742.f ;
    response["fn"] = scale_average_num;
    response["th"] = throttle;
    PRINT("//weight printed");

    scale_average = 0;
    scale_average_num = 0;
    
}




void setup() {
  offset = scale.averageValue();
  motor.attach(MOTOR_PIN);
  motor.write(0);

  Serial.begin(19200);
  Serial.println("{s: w}");  
  while (!Serial.available());
  

}


long dbg = 0;
#define dd Serial.print( "d: " ); Serial.print( dbg ); Serial.println( "" ); dbg++;


void loop(){

 
 if (Serial.available() > 0) {
    
    while (Serial.available() > 0) {
      const char lc = Serial.read();
   
      if (ndx == 0) {// first char readed
        if ( lc == '{') { //
          inputBuffer[ndx] = lc;
          ndx++;
        }
      } else if (ndx > buffSize - 1) {
        ndx = 0; //comm to long, abort
        Serial.println( "{s:s-er}" );
      } else {
        inputBuffer[ndx] = lc;
        ndx ++;       
      }
  }  
}

if (ndx > 0 && inputBuffer[ndx-1] == '}') {
    StaticJsonBuffer<200> jsonBuffer;
    StaticJsonBuffer<200> jsonResponse  ;
    
    JsonObject& root = jsonBuffer.parseObject(inputBuffer);
  
    // Test if parsing succeeds.
    if (!root.success()) {
      Serial.println( "{s:j-er}" );
    } else {
      PRINT(inputBuffer);
      reg_id = root["i"];       
      PRINT(root.get<char>("r"));    
      PRINT(root.get<long>("i"));          
    }    

    
    JsonObject& response = jsonResponse.createObject();

    dispatch( response, root );
    
    response["t"] = millis();
    response["i"] = reg_id;    
  
    response.printTo(Serial);
    Serial.println();

    for (int i =0; i < buffSize; i++){
      inputBuffer[i] = 0x0;  
    }

    ndx = 0;
        
 }

dbg = 0;



 scale_average += scale.getValue();
 scale_average_num++;


}





/*
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


*/


   
    



