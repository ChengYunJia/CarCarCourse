#include <Arduino.h>
// #include <SoftwareSerial.h>
// #include <SPI.h>
// #include <MFRC522.h>

#include "pins.hpp"
#include "btRfid.hpp"
#include "control.hpp"

void setup()
{
    motorSetup();
    tcrtSetup();
    btSetup();
    rfidSetup();
    delay(3000);
}

void loop()
{
    // handleMessage();
    // handleCommand();
    digitalWrite(PWMA, HIGH);
    digitalWrite(PWMB, HIGH);

    if ( countWhite() >= 2)
        Tracking();
    else
    {
        MotorWriting(0, 0);
        Takeinstruct();
    }
    
    if( canReadCard() ) handleCard() ;
}

/*
1. identify enter and leave the block in series
2. once leave the block stop
3. take the instruction
*/