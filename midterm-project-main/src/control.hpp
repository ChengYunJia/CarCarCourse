#include <Arduino.h>
#include "pins.hpp"
#include "btRfid.hpp"

enum COLOR
{
    WHITE = 0,
    BLACK = 1,
};

// int instruction[] = {GO, RIGHT, GO, LEFT, UTURN, GO, LEFT, GO, UTURN, GO, RIGHT, RIGHT, GO, GO, RIGHT, UTURN, GO, UTURN, RIGHT, GO, RIGHT, RIGHT, UTURN}; // 1 = straight, 2 = uturn, 3 = turn right, 4 = turn left
// fflfbfrrlrbllflbfs
// int instruction[] = {GO, LEFT, RIGHT, RIGHT, UTURN, GO, LEFT, GO, UTURN, GO, RIGHT, RIGHT, GO, GO, RIGHT, UTURN, GO, UTURN, RIGHT, GO, RIGHT, RIGHT, UTURN}; // 1 = straight, 2 = uturn, 3 = turn right, 4 = turn left
// int instruction[] = {GO, GO, LEFT, GO, UTURN, GO, RIGHT, RIGHT, LEFT, RIGHT, UTURN, LEFT, LEFT, GO, LEFT, UTURN, GO, STOP}; // 1 = straight, 2 = uturn, 3 = turn right, 4 = turn left

int white[5] = {0, 0, 0, 0, 0};
int sum_white = 0;
double weight[5] = {5.2, 2.3, 0, -2.3, -5.2};

const int Kp = 25;
const int Kd = 15;
const int Tp = 180; // speed
double gasgasgas = 1.05;
double error;
double last_error = 0;
unsigned long prevTime = 0;

void tcrtSetup(){
    for (int i = 0; i < 5; i++)
    {
        pinMode(digitalPin[i], INPUT);
    }
    Serial.begin(9600);
}

void motorSetup(){
    pinMode(PWMA, OUTPUT);
    pinMode(PWMB, OUTPUT);
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);
}

void MotorWriting(double, double);
void Tracking();
void Takeinstruct();
int countWhite();

void MotorWriting(double vL, double vR)
{
    // digitalWrite(PWMA, HIGH); // R
    // digitalWrite(PWMB, HIGH); // L
    if (vR > 0)
    {
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
    }
    else if (vR < 0)
    {
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
        vR = -vR;
    }

    if (vL > 0)
    {

        digitalWrite(BIN1, HIGH);
        digitalWrite(BIN2, LOW);
    }
    else if (vL < 0)
    {
        digitalWrite(BIN1, LOW);
        digitalWrite(BIN2, HIGH);
        vL = -vL;
    }

    vL *= 1.05; // left motor offset

    if(vR >= 255) vR = 255;
    if(vL >= 255) vL = 255;

    analogWrite(PWMB, vL);
    analogWrite(PWMA, vR);
}

// void handleCommand(){
//     switch (received)
//     {
//     case 'w':
//         MotorWriting(Tp, Tp);
//         break;
//     case 'a':
//         MotorWriting(0, Tp);
//         break;
//     case 's':
//         MotorWriting(-Tp, -Tp);
//         break;
//     case 'd':
//         MotorWriting(Tp, 0);
//         break;
//     case 'p':
//         MotorWriting(0, 0);
//         break;

//     default:
//         break;
//     }
// }

void Tracking()
{
    if( instruction[instructIndex+1] != GO ){
        gasgasgas = 1;
    }else{
        gasgasgas = 1.1;
    }

    if(digitalRead( digitalPin[4] == BLACK)){
        MotorWriting(Tp, Tp*0.5);
        delay(10);
    }
    if(digitalRead( digitalPin[0] == BLACK)){
        MotorWriting(Tp*0.5, Tp);
        delay(10);
    }

    error = 0;
    for (int i = 0; i < 5; i++)
    {
        error += white[i] * weight[i];
    }
    if (sum_white != 0)
    {
        error /= sum_white;
    }

    unsigned long currentTime = micros();
    double dt = (currentTime - prevTime) * 1.0e6;
    int powerCorrection = Kp * error + Kd * (error - last_error) / dt;  // ex. Kp = 100, 也與w2 & w3有關
    last_error = error;
    prevTime = micros();

    int vR = Tp - powerCorrection;
    int vL = Tp + powerCorrection;

    MotorWriting(vL*gasgasgas * 1.05, vR*gasgasgas); // Feedback to motors

    // Serial.println(vL);
    // Serial.println(vR);
}

void carGO(){
    if( digitalRead(digitalPin[0]) == WHITE ){
        MotorWriting( Tp, Tp * 0.1 );
        delay(63);
    }else if( digitalRead(digitalPin[4]) == WHITE ){
        MotorWriting( Tp * 0.1, Tp );
        delay(63);
    }


    MotorWriting(Tp*gasgasgas * 1.05, Tp*gasgasgas);
    delay(200);
    while ( digitalRead(digitalPin[1]) == BLACK && digitalRead(digitalPin[2]) == BLACK && digitalRead(digitalPin[3]) == BLACK )
    {
        if( digitalRead(digitalPin[0]) == WHITE ){
            MotorWriting(Tp*gasgasgas * 1.05, Tp*gasgasgas*0.95);
            delay(100);
        }
        else if( digitalRead(digitalPin[4]) == WHITE ){
            MotorWriting(Tp*gasgasgas*0.95 * 1.05, Tp*gasgasgas);
            delay(100);
        }else{
            delay(100);
        }
    }
    return;
}
void carUTURN(){
    if( digitalRead(digitalPin[0]) == WHITE ){
        MotorWriting( Tp, Tp * 0.1 );
        delay(30);
    }else if( digitalRead(digitalPin[4]) == WHITE ){
        MotorWriting( Tp * 0.1, Tp );
        delay(30);
    }


    if( digitalRead(digitalPin[0]) == WHITE ){
        MotorWriting(-Tp * 1.35, Tp * 0.6);
        delay(50);
    }
    MotorWriting(-Tp * 1.2, Tp);
    delay(300);
    MotorWriting(-Tp * 0.8, Tp*0.4);
    delay(200);
    while ( digitalRead(digitalPin[0]) == WHITE && digitalRead(digitalPin[1]) == WHITE )
    {
        MotorWriting(-Tp*0.5, Tp*0.4);
    }
    MotorWriting(Tp*1.35, Tp*0.6);
    delay(150);
}
void carUTURNR(){
    if( digitalRead(digitalPin[4]) == WHITE ){
        MotorWriting( Tp * 0.1, Tp );
        delay(30);
    }else if( digitalRead(digitalPin[0]) == WHITE ){
        MotorWriting( Tp, Tp*0.1 );
        delay(30);
    }


    if( digitalRead(digitalPin[4]) == WHITE ){
        MotorWriting(Tp * 0.55, -Tp * 1.3);
        delay(50);
    }
    MotorWriting(Tp, -Tp * 1.2);
    delay(300);
    MotorWriting(Tp*0.4, -Tp * 0.8);
    delay(200);
    while ( digitalRead(digitalPin[4]) == WHITE && digitalRead(digitalPin[3]) == WHITE )
    {
        MotorWriting(Tp*0.4, -Tp*0.5);
    }
    MotorWriting(Tp*0.6, Tp*1.3);
    delay(150);
}
void carRIGHT(){
    // double fracRight = 1;
    // if( instruction[instructIndex-1] == GO ){
    //     fracRight = 0.95;
    // }

    if( digitalRead(digitalPin[0]) == WHITE ){
        MotorWriting( Tp, Tp * 0.1 );
        delay(50);
    }else if( digitalRead(digitalPin[4]) == WHITE ){
        MotorWriting( Tp * 0.1, Tp );
        delay(50);
    }


    MotorWriting(1.25 * Tp, Tp / 7);
    delay(333);
    while ( digitalRead(digitalPin[1]) == WHITE && digitalRead(digitalPin[2]) == WHITE )
    {
        MotorWriting(1.3 * Tp, Tp / 2.5);
    }
    MotorWriting(Tp, Tp*1.37);
    delay(119);
}
void carLEFT(){
    if( digitalRead(digitalPin[0]) == WHITE ){
        MotorWriting( Tp, Tp * 0.1 );
        delay(50);
    }else if( digitalRead(digitalPin[4]) == WHITE ){
        MotorWriting( Tp * 0.1, Tp );
        delay(50);
    }


    MotorWriting(Tp / 6, 1.2 * Tp);
    delay(350);
    while ( digitalRead(digitalPin[2]) == WHITE && digitalRead(digitalPin[3]) == WHITE )
    {
        MotorWriting(Tp / 1.9, 1.2 * Tp);
    }
    MotorWriting(Tp*1.30, Tp*0.97);
    delay(100);
}

void Takeinstruct()
{
    if( instructIndex >= rightBoarder ){
        return;
    }
    switch (instruction[ instructIndex ])
    {
    case GO:
        carGO();
        break;
    case UTURN: // check
        carUTURN();
        break;
    case RIGHT:
        carRIGHT();
        break;
    case LEFT:
        carLEFT();
        break;

    case STOP:
        MotorWriting(0, 0);
        // delay(10000);
        break;

    case UTURNR: // check
        carUTURNR();
        break;

    default:
        MotorWriting(0, 0);
        break;
    }
    instructIndex++;
    // Serial.println(ite);
}

int countWhite()
{
    sum_white = 0;
    for (int i = 0; i < 5; i++)
    {
        white[i] = ( digitalRead(digitalPin[i]) == WHITE );
        sum_white += white[i];
    }
    return sum_white;
}