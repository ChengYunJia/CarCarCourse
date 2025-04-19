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
    // delay(20000);
    // BTSerial.println("z");
    while(rightBoarder<=1){
        handleMessage();
    } // start when connected to bt
}

void loop()
{
    handleMessage();
    // handleCommand();

    digitalWrite(PWMA, HIGH);
    digitalWrite(PWMB, HIGH);

    if ( countWhite() >= 2 && instructIndex != rightBoarder)
        Tracking();
    else
    {
        MotorWriting(0, 0);
        // BTSerial.write("msg\n");
        // BTSerial.write("z\n");
        BTSerial.println("msg");
        BTSerial.println("z");
        Takeinstruct();
    }
    
    if( canReadCard() ) handleCard();
}

/*
1. identify enter and leave the block in series
2. once leave the block stop
3. take the instruction
*/