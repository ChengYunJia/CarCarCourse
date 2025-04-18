/***************************************************************************/
// File       [final_project.ino]
// Author     [Erik Kuo]
// Synopsis   [Code for managing main process]
// Functions  [setup, loop, Search_Mode, Hault_Mode, SetState]
// Modify     [2020/03/27 Erik Kuo]
/***************************************************************************/

#define DEBUG  // debug flag

// for RFID
#include <MFRC522.h>
#include <SoftwareSerial.h>
#include <SPI.h>

/*===========================define pin & create module object================================*/
// BlueTooth
// BT connect to Serial1 (Hardware Serial)
// Mega               HC05
// Pin  (Function)    Pin
// 18    TX       ->  RX
// 19    RX       <-  TX
// TB6612, 請按照自己車上的接線寫入腳位(左右不一定要跟註解寫的一樣)
// TODO: 請將腳位寫入下方
#define MotorR_I1 2     // 定義 A1 接腳（右）
#define MotorR_I2 3     // 定義 A2 接腳（右）
#define MotorR_PWMR 12  // 定義 ENA (PWM調速) 接腳
#define MotorL_I3 5     // 定義 B1 接腳（左）
#define MotorL_I4 6     // 定義 B2 接腳（左）
#define MotorL_PWML 13  // 定義 ENB (PWM調速) 接腳
// 循線模組, 請按照自己車上的接線寫入腳位
#define IRpin_LL 32
#define IRpin_L 34
#define IRpin_M 36
#define IRpin_R 38
#define IRpin_RR 40
// RFID, 請按照自己車上的接線寫入腳位
#define RST_PIN 9                 // 讀卡機的重置腳位
#define SS_PIN 53                 // 晶片選擇腳位
// Bluetooth pins
#define RX 10
#define TX 11

MFRC522 mfrc522(SS_PIN, RST_PIN);  // 建立MFRC522物件
SoftwareSerial BTSerial(RX, TX);  // RX, TX (Ensure correct wiring)
/*===========================define pin & create module object===========================*/

/*============setup============*/
void setup() {
    // bluetooth initialization
    BTSerial.begin(9600);
    // Serial window
    Serial.begin(9600);
    Serial.println(" BT Ready!");
    // RFID initial
    SPI.begin();
    mfrc522.PCD_Init();
    Serial.println(" RFID Ready!");
    // TB6612 pin
    pinMode(MotorR_I1, OUTPUT);
    pinMode(MotorR_I2, OUTPUT);
    pinMode(MotorL_I3, OUTPUT);
    pinMode(MotorL_I4, OUTPUT);
    pinMode(MotorL_PWML, OUTPUT);
    pinMode(MotorR_PWMR, OUTPUT);
    // tracking pin
    pinMode(IRpin_LL, INPUT);
    pinMode(IRpin_L, INPUT);
    pinMode(IRpin_M, INPUT);
    pinMode(IRpin_R, INPUT);
    pinMode(IRpin_RR, INPUT);
#ifdef DEBUG
    Serial.println("Start!");
#endif
}
/*============setup============*/

/*=====Import header files=====*/
#include "RFID.h"
#include "bluetooth.h"
#include "node.h"
#include "track.h"
#include "enumDirection.h"
/*=====Import header files=====*/

/*===========================initialize variables===========================*/
int l2 = 0, l1 = 0, m0 = 0, r1 = 0, r2 = 0;  // 紅外線模組的讀值(0->white,1->black)
int Tp = 180;                                // set your own value for motor power
//bool state = false;     // set state to false to halt the car, set state to true to activate the car
//BT_CMD _cmd = NOTHING;  // enum for bluetooth message, reference in bluetooth.h line 2
char received = 's';
bool send_N_cmd = false;
int instruction[1000];
int rightBoarder = 0;
int instructIndex = 0;
char defaultInstruct[100] = "s";
bool bt_setup = false;
/*===========================initialize variables===========================*/

/*===========================declare function prototypes===========================*/
void Search();    // search graph
void SetState();  // switch the state
/*===========================declare function prototypes===========================*/

/*===========================define function===========================*/
void loop() {
    if(!bt_setup)
    {
        for(int i=0 ; i<100 ; i++){
            received = defaultInstruct[i];
            handleReceived();
        }
        bt_setup = true;
    }
    digitalWrite(MotorR_PWMR, HIGH);
    digitalWrite(MotorL_PWML, HIGH);

    l2 = ( digitalRead(IRpin_LL) == WHITE );
    l1 = ( digitalRead(IRpin_L) == WHITE );
    m0 = ( digitalRead(IRpin_M) == WHITE );
    r1 = ( digitalRead(IRpin_R) == WHITE );
    r2 = ( digitalRead(IRpin_RR) == WHITE );

    SetState();
    Search();
}

void SetState() {
    // TODO:
    // 1. Get command from bluetooth
    // 2. Change state if need
    /*
    if( countWhite(l2, l1, m0, r1, r2)<2 and send_N_cmd == false )
    {
        send_msg('N');
        send_msg('\n');
        Serial.print('N');
        delay(50);
        send_N_cmd = true;
    }
    else if( countWhite(l2, l1, m0, r1, r2)>=2 and send_N_cmd == true )
    {
        send_N_cmd = false;
    }
    */

    handleMessage();
}

void Search() {
    // TODO: let your car search graph(maze) according to bluetooth command from computer(python
    // code)
    if ( countWhite(l2, l1, m0, r1, r2) == 5)//problem
        tracking(l2, l1, m0, r1, r2);
    else
    {
        MotorWriting(0, 0);
        Takeinstruct(l2, l1, m0, r1, r2);
        //send_msg('N');//on node, ask next cmd
    }

    //if( canReadCard() ) handleCard() ;
}
/*===========================define function===========================*/
