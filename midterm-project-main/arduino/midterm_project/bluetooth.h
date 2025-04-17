/***************************************************************************/
// File			  [bluetooth.h]
// Author		  [Erik Kuo]
// Synopsis		[Code for bluetooth communication]
// Functions  [ask_BT, send_msg, send_byte]
// Modify		  [2020/03/27 Erik Kuo]
/***************************************************************************/

/*if you have no idea how to start*/
/*check out what you have learned from week 2*/

#include "enumDirection.h"

char extern received;
SoftwareSerial extern BTSerial;

int extern instruction[1000];
int extern rightBoarder;
int extern instructIndex;
int command = STOP;
char extern defaultInstruct[100];


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

        case 'b':
        /* code */
        command = UTURN;
        break;

        case 's':
        command = STOP;
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
        received = BTSerial.read();
        Serial.print(received);
    }

    return;
}


void send_msg(const char& msg) {
    // TODO:
    BTSerial.write(msg);

}

// send UID back through Serial1(bluetooth serial)
void send_byte(byte* id, byte& idSize) {
    for (byte i = 0; i < idSize; i++) {  // Send UID consequently.
        BTSerial.write(id[i]);
    }
#ifdef DEBUG
    Serial.print("Sent id: ");
    for (byte i = 0; i < idSize; i++) {  // Show UID consequently.
        Serial.print(id[i], HEX);
    }
    Serial.println();
#endif
}  // send_byte

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
