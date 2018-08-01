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

float scale_average = 0;
long scale_average_num = 0;


char inputBuffer[200];
float reg_speed = 0;
long reg_id = 0;
const byte buffSize = 200;
long ndx = 0;

const int th_min = 1300;
const int th_max = 1800;
int throttle = 0;



 // read RPM

 volatile int half_revolutions = 0;

 float rpm = 0;
 unsigned long rpm_num;
 
 unsigned long lastmillis = 0;


    
//#define PRINT(T) Serial.println(T);
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
              
            motor.writeMicroseconds(ii); // throttle value define the speed of esc          
            delay(100);
          }
        } else {
          for ( ii = throttle; ii > throttle2; ii--){ 
            PRINT(ii);    
            motor.writeMicroseconds(ii); // throttle value define the speed of esc          
            delay(100);
          }          
        }
        throttle = ii;
    }

    if (strcmp(request.get<const char*>("r"), "stop") == 0){
        PRINT("//stop");    
        
        int ii = 0;
        for ( ii = throttle; ii > th_min; ii--){ 
          PRINT(ii);    
          motor.writeMicroseconds(ii); // throttle value define the speed of esc          
          delay(100);
        }          
        throttle = 0;
      }
      
    if (strcmp(request.get<const char*>("r"), "start") == 0){
        PRINT("//stop");    
        throttle = th_min;
        int ii = th_min - 10;
        for ( ii = 0; ii < throttle; ii++){ 
          PRINT(ii);    
          motor.writeMicroseconds(ii); // throttle value define the speed of esc          
          delay(10);
        }          
      }
  
    // In other case, you can do root.set<long>("time", 1351824120);
    response["f"] = ((float)scale_average - offset) / 742.f ;
    response["fn"] = scale_average_num;
    response["th"] = throttle;
    response["rpm"] = rpm;
    PRINT("//weight printed");

    scale_average = 0;
    scale_average_num = 0;
    rpm = 0;
    rpm_num = 0;
    
}

 void setup(){
 
  attachInterrupt(0, rpm_fan, FALLING);
 
  offset = scale.averageValue();
  motor.attach(MOTOR_PIN);
  motor.writeMicroseconds(0);

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



 scale_average = (float)( scale_average*scale_average_num + scale.getValue() ) / (scale_average_num + 1);
 scale_average_num++;



 if (millis() - lastmillis >= 10){ //Uptade every one second, this will be equal to reading frecuency (Hz).
   const unsigned long dt = millis() - lastmillis;
   detachInterrupt(0);//Disable interrupt when calculating
   rpm =  (rpm * rpm_num   + (float)( half_revolutions * 30000. / dt )) / ((float)(rpm_num + 1.)); // Convert frecuency to RPM, note: this works for one interruption per full rotation. For two interrups per full rotation use half_revolutions * 30.
   rpm_num++;
   half_revolutions = 0; // Restart the RPM counter
   lastmillis = millis(); // Uptade lasmillis
   attachInterrupt(0, rpm_fan, FALLING); //enable interrupt
  }

}

 // this code will be executed every time the interrupt 0 (pin2) gets low.

 void rpm_fan(){
  half_revolutions++;
 }


   
    



