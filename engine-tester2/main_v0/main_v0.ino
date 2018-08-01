#include <Wire.h>

// LCD
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

// buttons
#include <YetAnotherDebouncer.h>
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
const int buttons[4] = {10,11,12,13};
Debouncer<Clock::Millis> debouncers[4];
int buttons_s[4] = {0,0,0,0};

//servo/esc
#include <Servo.h>
#define MOTOR_PIN 9

//weight
#include "hx711.h"



// globals
int motor_value = 0;

hd44780_I2Cexp lcd; // declare lcd object: auto locate & config exapander chip

// LCD geometry
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

unsigned long loop_last_time = 0;

Servo motor;

Hx711 scale(A1, A0);
float scale_average = 0;
float scale_value = 0;
long  scale_average_num = 0;
long  scale_offset = 0;


//end globals


void setup()
{
int status;
  loop_last_time = millis();

  Serial.begin(19200);
  Serial.println("init");  

  motor.attach(MOTOR_PIN);
  
  status = lcd.begin(LCD_COLS, LCD_ROWS);
  if(status) // non zero status means it was unsuccesful
  {
    Serial.println("failed");  
    status = -status; // convert negative status value to positive number

    // begin() failed so blink error code using the onboard LED if possible
    hd44780::fatalError(status); // does not return
  }
  Serial.println("active");  
  // initalization was successful, the backlight should be on now

  // Print a message to the LCD
  lcd.print("Hello, World!");

  ///////// init buttons //////////
  for(int i = 0; i < 4; i++){
    pinMode(buttons[i], INPUT_PULLUP);

        int debounce = i*i*25;
        
        
        debouncers[i].begin();
        debouncers[i].set_debounce_stable(debounce);
        debouncers[i].set_callback(debounce_callback, &buttons[i]);    
  }

}



// ############ buttons ##############
void debounce_callback(bool value, const int* i) {
  int ii = i -&buttons[0];
  const int dmin = 1300;
  const int dmax = 0;
  const int dm = 25;
  if (value){
    buttons_s[ ii ] ++;
  

    switch (ii) {
      case 0:
        motor_value+=dm;
      break;  
  
      case 1:
        motor_value-=dm;
      break;
  
      case 2:
        motor_value = 0;
      break;        
      
      case 3:
        motor_value = dmin;
      break;              
    }
    if (motor_value < 0) { 
      motor_value = 0;
    }
  }
}


unsigned int loopi = 0;

void buttons_and_lcd(){
  for (int i=0; i<ARRAY_SIZE(buttons); i++) {
        debouncers[i].set_value(digitalRead(buttons[i]));
  }  
  //delay(100);
  loopi ++;
  if ( millis() - loop_last_time > 1000) {
    loop_last_time = millis();
    lcd.clear();
    //#lcd.print("Hello, World!");
    for(int i = 0; i < 4; i++){
      
        lcd.print(buttons_s[i]);
        lcd.print(" ");
      
    }  
    //lcd.print("\n");
    lcd.print(motor_value);

    lcd.setCursor(0,1);
    lcd.print(scale_value);
    lcd.print("/");
    //lcd.print(scale_average);
    lcd.print("/");
    lcd.print(scale_average_num);
    
    
  }  
}
// end ############ buttons ##############

void force() {

 scale_average = (float)( scale_average*scale_average_num + scale.getValue() ) / (scale_average_num + 1);
 scale_average_num++;  

 if (scale_average_num > 10){
    scale_value = ((float)scale_average - scale_offset) / 742.f ;
    scale_average = 0;
    scale_average_num = 0;
 }


}
void loop() {

  force();
  buttons_and_lcd();
  motor.writeMicroseconds(motor_value);
  
}
