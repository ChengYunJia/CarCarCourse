#include <Arduino.h>
#include "pins.hpp"
#include "btRfid.hpp"

enum DIRECTION
{
    GO = 1,
    UTURN = 2,
    RIGHT = 3,
    LEFT = 4,
};

enum COLOR
{
    WHITE = 0,
    BLACK = 1,
};

int instruction[] = {GO, LEFT, RIGHT, RIGHT, UTURN, GO, LEFT, GO, UTURN, GO, RIGHT, RIGHT, GO, GO, RIGHT, UTURN, GO, UTURN, RIGHT, GO, RIGHT, RIGHT, UTURN}; // 1 = straight, 2 = uturn, 3 = turn right, 4 = turn left

int ite = 0;
int white[5] = {0, 0, 0, 0, 0};
int sum_white = 0;
int weight[5] = {5, 2, 0, -2, -5};

const int Kp = 25;
const int Kd = 15;
const int Tp = 200; // speed
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

void handleCommand(){
    switch (received)
    {
    case 'w':
        MotorWriting(Tp, Tp);
        break;
    case 'a':
        MotorWriting(0, Tp);
        break;
    case 's':
        MotorWriting(-Tp, -Tp);
        break;
    case 'd':
        MotorWriting(Tp, 0);
        break;
    case 'p':
        MotorWriting(0, 0);
        break;

    default:
        break;
    }
}

void Tracking()
{
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

    MotorWriting(vL, vR); // Feedback to motors

    // Serial.println(vL);
    // Serial.println(vR);
}

void Takeinstruct()
{
    switch (instruction[ite])
    {
    case GO:

        if( digitalRead(digitalPin[0]) == WHITE ){
            MotorWriting( Tp, Tp * 0.1 );
            delay(30);
        }else if( digitalRead(digitalPin[4]) == WHITE ){
            MotorWriting( Tp * 0.1, Tp );
            delay(30);
        }


        MotorWriting(Tp, Tp);
        delay(100);
        while ( digitalRead(digitalPin[1]) == BLACK && digitalRead(digitalPin[2]) == BLACK && digitalRead(digitalPin[3]) == BLACK )
        {
            if( digitalRead(digitalPin[0]) == WHITE ){
                MotorWriting(Tp, Tp*0.95);
                delay(100);
            }
            if( digitalRead(digitalPin[4]) == WHITE ){
                MotorWriting(Tp*0.95, Tp);
                delay(100);
            }
        }
        break;
    case UTURN: // check

        if( digitalRead(digitalPin[0]) == WHITE ){
            MotorWriting( Tp, Tp * 0.1 );
            delay(30);
        }else if( digitalRead(digitalPin[4]) == WHITE ){
            MotorWriting( Tp * 0.1, Tp );
            delay(30);
        }


        if( digitalRead(digitalPin[0]) == WHITE ){
            MotorWriting(-Tp * 1.3, Tp * 0.6);
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
        MotorWriting(Tp*1.3, Tp*0.6);
        delay(150);
        break;
    case RIGHT:

        if( digitalRead(digitalPin[0]) == WHITE ){
            MotorWriting( Tp, Tp * 0.1 );
            delay(30);
        }else if( digitalRead(digitalPin[4]) == WHITE ){
            MotorWriting( Tp * 0.1, Tp );
            delay(30);
        }


        MotorWriting(1.2 * Tp, Tp / 6);
        delay(330);
        while ( digitalRead(digitalPin[1]) == WHITE && digitalRead(digitalPin[2]) == WHITE )
        {
            MotorWriting(1.2 * Tp, Tp / 2);
        }
        MotorWriting(Tp, Tp*1.2);
        delay(100);
        break;
    case LEFT:

        if( digitalRead(digitalPin[0]) == WHITE ){
            MotorWriting( Tp, Tp * 0.1 );
            delay(30);
        }else if( digitalRead(digitalPin[4]) == WHITE ){
            MotorWriting( Tp * 0.1, Tp );
            delay(30);
        }


        MotorWriting(Tp / 6, 1.2 * Tp);
        delay(330);
        while ( digitalRead(digitalPin[2]) == WHITE && digitalRead(digitalPin[3]) == WHITE )
        {
            MotorWriting(Tp / 2, 1.2 * Tp);
        }
        MotorWriting(Tp*1.2, Tp);
        delay(100);
        break;
    }
    ite++;
    if(ite == 23) ite = 1;
    
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