void setup() {
    //P0, P1, and P4 are capable of hardware PWM (analogWrite).
    pinMode(0, OUTPUT); //0 is P0, 1 is P1, 4 is P4 - unlike the analog inputs, 
    pinMode(1, OUTPUT); //0 is P0, 1 is P1, 4 is P4 - unlike the analog inputs, 
                        //for analog (PWM) outputs the pin number matches the port number.
}

void loop() {
    for (int i =0; i <2; i++){
    analogWrite(i,255); //Turn the pin on full (100%)
    delay(1000);
    analogWrite(i,128); //Turn the pin on half (50%)
    delay(1000);
    analogWrite(i,0);   //Turn the pin off (0%)
    delay(1000);
    }
}
