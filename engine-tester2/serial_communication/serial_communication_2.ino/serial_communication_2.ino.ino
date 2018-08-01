void setup() {
  // initialize the serial communication:
  Serial.begin(9600);
}

unsigned long i = 0;
void loop() {
  // send the value of analog input 0:
  Serial.println(i);
  // wait a bit for the analog-to-digital converter to stabilize after the last
  // reading:
  delay(2);
  i = i + 1;
}  
