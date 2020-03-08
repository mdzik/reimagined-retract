#include <Adafruit_GFX.h>    // Core graphics library
#include <MCUFRIEND_kbv.h>   // Hardware-specific library
MCUFRIEND_kbv tft;

//#include <Fonts/FreeSans9pt7b.h>
//#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSerif12pt7b.h>

#include <FreeDefaultFonts.h>
#define LCD_RESET A3

#define BLACK   0x0000
#define RED     0xF800
#define GREEN   0x07E0
#define WHITE   0xFFFF
#define GREY    0x8410

#define BLUE    0x001F
#define RED     0xF800

#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0



////////////////

#include <OneWire.h>
#include <DallasTemperature.h>
 
OneWire oneWire(A5); //Podłączenie do A5
DallasTemperature sensors(&oneWire); //Przekazania informacji do biblioteki
char buffer[100];

int i = 0;

volatile float tempBuff[319];

unsigned long lastChecked = 0;
#define hours 1
#define intervalAll 3600000 //hours * 3600000
unsigned long interval = 1000;
////////////////
#include <Wire.h>
void receiveEvent(int howMany);

float T = 0;
void setup() {
    Wire.begin(8);                // join i2c bus wit
    Wire.onReceive(receiveEvent); // register event
    Serial.begin(9600);
    uint16_t ID = tft.readID();
    if (ID == 0xD3) ID = 0x9481;
    tft.begin(ID); 
    tft.setRotation(1);

    for (int j = 0; j <= tft.width(); j++){
      tempBuff[j] = 0;  
    }
    sensors.begin(); //Inicjalizacja czujnikow
    tft.fillScreen(GREY);
    interval = (unsigned long)(intervalAll) / (unsigned long) tft.width();
    Serial.println(intervalAll);
}

float T0 = 30;
float Tmin = 20;
float Tmax = 40;
volatile float t0 = 0;
volatile int nt = 0;
float Ts = 0.;
int triggers=0;

// function that executes whenever data is received from master
// this function is registered as an event, see setup()


void receiveEvent(int howMany) {
  int x = Wire.read();    // receive byte as an integer
  int b0 = Wire.read();    // receive byte as an integer
  int b1 = Wire.read();    // receive byte as an integer
  t0 = b0 + b1 * 0.1;
  tempBuff[i] = (tempBuff[i]*nt + t0)/ (nt+1);
  nt = nt + 1;
  Serial.print("-> ");
  Serial.println(x);


}

void loop(void)
{

    //sensors.requestTemperatures(); //Pobranie temperatury czujnika
    //t0 = sensors.getTempCByIndex(0);
    //tempBuff[i] = (tempBuff[i]*nt + t0)/ (nt+1);
    ///nt = nt + 1;

   /* Serial.print(i);
    Serial.print(' ');
    Serial.print(nt);
    Serial.print(' ');
    Serial.print(interval);
    Serial.print(' ');
    Serial.print(millis() - lastChecked);
    Serial.println();*/

    if (i > tft.width()) {
      i = 0;
    }
    if ((i > 10 ) && (i%10 == 0)) {
      tft.fillScreen(GREY);
      for (int j = 0; j < i-1; j++){
        T0 = T0 + tempBuff[j];  
        if (Tmin > tempBuff[j]){Tmin = tempBuff[j];};
        if (Tmax < tempBuff[j]){Tmax = tempBuff[j];};
      }
      //Serial.println(T0);
      T0 = T0 / i;  

    }
    
        
    float L = tft.width();
    float H = tft.height() - 30;
    int Y0 = tft.height() - H/2.;
    
    int x, y, w = tft.width(), h = tft.height();
    
    tft.drawFastHLine(0, Y0-1, w, BLACK);
    tft.drawFastHLine(0, Y0, w, BLACK);
    tft.drawFastHLine(0, Y0+1, w, BLACK);

    sprintf(buffer, "%d.%01d", (int)T0, (int)(T0*10)%10);    
    showmsgXY(10, Y0-12, 0.5, &FreeSmallFont, buffer, BLACK);

    for (float t = 2.5; t < Tmax-T0; t=t+2.5) {
      tft.drawFastHLine(0, Y0-t*(H/(Tmax-Tmin)), w, BLACK);
    }

    for (float t = 2.5; t < T0-Tmin; t=t+2.5) {
      tft.drawFastHLine(0, Y0+t*(H/(Tmax-Tmin)), w, BLACK);
    }
    
    for (int j = 0; j < 10; j++) {
      tft.drawFastVLine(intervalAll / 10.* j * w, 30 , h, BLACK);
    }    
    
    for (x = 0; x < w; x += (int)(float)w/(float)hours/6. ) tft.drawFastVLine(x, 0, h, BLACK);
           
    tft.fillRect(0,0,tft.width(), 30,  BLACK);
    

    
    int c = GREY;

    if (i > 20) {
      float tavg= 0;
      float tavg2= 0;
      for (int j = 0; j < 20; j++){
        tavg += tempBuff[i-j];
        if (j < 10) tavg2 += tempBuff[i-j];
      }
      tavg = tavg / 20.;
      tavg2 = tavg2 / 10.;
      if ( tavg2 - tavg > 0.5     ){
        c = RED;
      }
      if ( tavg2 - tavg < -0.5 ){
        c = BLUE;
      }    
    }
    sprintf(buffer, "%d.%02d C", (int)tempBuff[i], (int)(tempBuff[i]*100)%100);    
    showmsgXY(20, 20, 1, &FreeSerif12pt7b, buffer, c);

    if (analogRead(A4) < 100){
      triggers = triggers + 1;
    }
    if (triggers > 5) {
      Ts = tempBuff[i];  
      triggers= 0;
    }
    sprintf(buffer, "%d.%02d C", (int)Ts, (int)(Ts*100)%100);    
    showmsgXY(110, 20, 1, &FreeSerif12pt7b, buffer, MAGENTA);

    const float dt = tempBuff[i] - Ts;
    if (dt > 0) { 
     sprintf(buffer, "%d.%02d C", (int)dt, (int)(dt*100)%100);         
      showmsgXY(200, 20, 1, &FreeSerif12pt7b, buffer, RED);  
    } else {
      sprintf(buffer, "-%d.%02d C", (int)-dt, (int)(-dt*100)%100);          
      showmsgXY(200, 20, 1, &FreeSerif12pt7b, buffer, BLUE);  
    }
    
    
    //Serial.println(buffer);  //Wyswietlenie informacji


    

    for (int j = 1; j <= i; j++){
      tft.drawLine(j, Y0 + (T0-tempBuff[j])*(H/(Tmax-Tmin)),j-1, Y0 + (T0-tempBuff[j-1])*(H/(Tmax-Tmin)),  RED);
      tft.drawLine(j, Y0 + (T0-tempBuff[j])*(H/(Tmax-Tmin))+1,j-1, Y0 + (T0-tempBuff[j-1])*(H/(Tmax-Tmin))+1,  RED);

    }


    
    //delay(50);

    if ( millis() - lastChecked > interval) { 
      lastChecked = millis();  
      nt = 0;
      t0 = 0;
      i = i + 1;
    }    
        

}

void showmsgXY(int x, int y, int sz, const GFXfont *f, const char *msg, const int color)
{
    int16_t x1, y1;
    uint16_t wid, ht;
  //  tft.drawFastHLine(0, y, tft.width(), WHITE);
    tft.setFont(f);
    tft.setCursor(x, y);
    tft.setTextColor(color);
    tft.setTextSize(sz);
    tft.print(msg);
    //delay(1000);
}
