struct DataFrame {
    float f1;
    float f2;
    unsigned long timestamp;
    unsigned long spins;
    float rpms;
    char status = 'o';
};


void serialWriteData(DataFrame dataframe){
    Serial.println("Dataframe: ");
    char buffer[30];
    sprintf(buffer, "f1    -> %f", dataframe.f1); Serial.println( buffer );
    sprintf(buffer, "f2    -> %f", dataframe.f2); Serial.println( buffer );
    sprintf(buffer, "time  -> %ld", dataframe.timestamp); Serial.println( buffer );
    sprintf(buffer, "spins  -> %ld", dataframe.spins); Serial.println( buffer );
    sprintf(buffer, "rpms  -> %f", dataframe.rpms); Serial.println( buffer );
    sprintf(buffer, "state -> %c", dataframe.status); Serial.println( buffer );



   // Serial.write( (byte*)&dataframe, sizeof(DataFrame) );
}