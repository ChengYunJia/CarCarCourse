#ifndef BTRFID_HPP
#define BTRFID_HPP 1

#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>
#include "pins.hpp"

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance
SoftwareSerial BTSerial(RX, TX);  // RX, TX (Ensure correct wiring)

bool messageFlag = 0;
char received = 'p';

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

void handleMessage()
{
    if (BTSerial.available())
    {
        messageFlag = 0;
        received = BTSerial.read();
        Serial.print(received);
    }
    else if (messageFlag)
    {
        Serial.println();
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
    Serial.print("Card UID: ");
    BTSerial.print("Card UID: ");

    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        BTSerial.print(mfrc522.uid.uidByte[i], HEX);

        if (i < mfrc522.uid.size - 1)
        {
            Serial.print(":");
            BTSerial.print(":");
        }
    }

    Serial.println();
    BTSerial.println();

    // Halt the card and stop encryption
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    // Delay to prevent multiple rapid reads of the same card
    delay(100);
}

void btSetup()
{
    Serial.begin(9600);   // Serial Monitor for debugging
    BTSerial.begin(9600); // HC-05 Bluetooth Module
    Serial.println("BT Ready!");
}

void rfidSetup()
{
    SPI.begin();
    mfrc522.PCD_Init(); // Initialize MFRC522

    Serial.println("RFID Ready!");
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