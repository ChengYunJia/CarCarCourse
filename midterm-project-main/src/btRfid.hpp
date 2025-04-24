#ifndef BTRFID_HPP
#define BTRFID_HPP 1

#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>
#include "pins.hpp"

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance
SoftwareSerial BTSerial(RX, TX);  // RX, TX (Ensure correct wiring)

bool messageFlag = 0;
char received = 's';

// f l r b s
enum DIRECTION
{
    GO = 1,
    UTURN = 2,
    RIGHT = 3,
    LEFT = 4,
    STOP = 5,
    UTURNR = 6,
};

int instruction[1000];
int rightBoarder = 0;
int instructIndex = 0;
int command = STOP;
// char defaultInstruct[500] = "fflfbfrrlr2llfr2fs";
// char defaultInstruct[500] = "fllfr2fbrffr2flfbfrf2llr2llflbf2lffr2lfflbrfflbrfflbf2lfrr2lrr2flfbfrrfbrrs";
char defaultInstruct[500] = "";


bool canReadCard()
{
    if (!mfrc522.PICC_IsNewCardPresent())
    {
        return 0;
    }

    // Select the card
    if (!mfrc522.PICC_ReadCardSerial())
    {
        return 0;
    }

    return 1;
}

void handleReceived(){
    switch (received)
    {
        case 'f':
        /* code */
        command = GO;
        break;

        case 'l':
        /* code */
        command = LEFT;
        break;

        case 'r':
        /* code */
        command = RIGHT;
        break;

        case 'p':
        /* code */
        command = UTURN;
        break;

        case 's':
        command = STOP;
        /* code */
        break;

        case 'q':
        command = UTURNR;
        /* code */
        break;

        default:
            return;
        break;
    }
    instruction[ rightBoarder ] = command;
    rightBoarder++;
}

void handleMessage()
{
    if (BTSerial.available())
    {
        messageFlag = 1;
        received = BTSerial.read();
        Serial.print(received);
        handleReceived();

    }
    else if (messageFlag)
    {
        Serial.println("");
        messageFlag = 0;
    }

    if (Serial.available())
    {
        BTSerial.write(Serial.read());
    }

    return;
}

void handleCard()
{
    // Read UID and send it via Bluetooth
    // Serial.print("UID");
    BTSerial.println("UID");

    char buffer[3];

    for (byte i = 0; i < mfrc522.uid.size; i++)
    {

        sprintf(buffer, "%02X", mfrc522.uid.uidByte[i]); // 2-digit uppercase hex
        Serial.print(buffer);
        BTSerial.print(buffer);
        
        // if (i < mfrc522.uid.size - 1)
        // {
        //     Serial.print(":");
        //     BTSerial.print(":");
        // }
    }

    Serial.println();
    BTSerial.println();

    // Halt the card and stop encryption
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    // Delay to prevent multiple rapid reads of the same card
    delay(30);
}

void btSetup()
{
    Serial.begin(9600);   // Serial Monitor for debugging
    BTSerial.begin(9600); // HC-05 Bluetooth Module
    Serial.println(" BT Ready!");
    for(int i=0 ; i<100 ; i++){
        received = defaultInstruct[i];
        handleReceived();
    }
}

void rfidSetup()
{
    SPI.begin();
    mfrc522.PCD_Init(); // Initialize MFRC522

    Serial.println(" RFID Ready!");
}

// void setup() {
//     btSetup();
//     rfidSetup();
// }

// void loop() {

//   handleMessage();

//   if( canReadCard() ) handleCard();

// }

#endif /* btRfid.hpp */