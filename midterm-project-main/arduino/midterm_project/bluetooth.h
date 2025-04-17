/***************************************************************************/
// File			  [bluetooth.h]
// Author		  [Erik Kuo]
// Synopsis		[Code for bluetooth communication]
// Functions  [ask_BT, send_msg, send_byte]
// Modify		  [2020/03/27 Erik Kuo]
/***************************************************************************/

/*if you have no idea how to start*/
/*check out what you have learned from week 2*/

bool extern messageFlag;
char extern received;
SoftwareSerial extern BTSerial;

enum BT_CMD {
    
    // TODO: add your own command type here
};

bool ask_BT() {
    //BT_CMD message = NOTHING;
    bool success = false;
    char cmd;
    if (BTSerial.available()) {
// TODO:
// 1. get cmd from Serial1(bluetooth serial)
// 2. link bluetooth message to your own command type
        messageFlag = 1;
        received = BTSerial.read();
        success = true;
        Serial.print(received);

#ifdef DEBUG
        Serial.print("cmd : ");
        Serial.println(cmd);
#endif
    }
    else if (messageFlag)
    {
        Serial.println("");
        messageFlag = 0;
    }
    return success;
}  // ask_BT

// send msg back through Serial1(bluetooth serial)
// can use send_byte alternatively to send msg back
// (but need to convert to byte type)
void send_msg(const char& msg) {
    // TODO:
    BTSerial.write(msg);

}  // send_msg

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
