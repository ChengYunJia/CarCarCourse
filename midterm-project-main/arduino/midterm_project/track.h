/***************************************************************************/
// File			  [track.h]
// Author		  [Erik Kuo]
// Synopsis		[Code used for tracking]
// Functions  [MotorWriting, MotorInverter, tracking]
// Modify		  [2020/03/27 Erik Kuo]
/***************************************************************************/

/*if you have no idea how to start*/
/*check out what you have learned from week 1 & 6*/
/*feel free to add your own function for convenience*/

/*===========================import variable===========================*/
int extern _Tp;
char extern received;
/*===========================import variable===========================*/

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

void MotorWriting(double, double);
void Tracking();
void Takeinstruct();
int countWhite();



// Write the voltage to motor.
void MotorWriting(double vL, double vR) {
    // TODO: use TB6612 to control motor voltage & direction
    // digitalWrite(PWMA, HIGH); // R
    // digitalWrite(PWMB, HIGH); // L
    if (vR > 0)
    {
        digitalWrite(MotorR_I1, LOW);
        digitalWrite(MotorR_I2, HIGH);
    }
    else if (vR < 0)
    {
        digitalWrite(MotorR_I1, HIGH);
        digitalWrite(MotorR_I2, LOW);
        vR = -vR;
    }

    if (vL > 0)
    {

        digitalWrite(MotorL_I3, HIGH);
        digitalWrite(MotorL_I4, LOW);
    }
    else if (vL < 0)
    {
        digitalWrite(MotorL_I3, LOW);
        digitalWrite(MotorL_I4, HIGH);
        vL = -vL;
    }

    //vL *= 1.05; // left motor offset

    if(vR >= 255) vR = 255;
    if(vL >= 255) vL = 255;

    analogWrite(MotorL_PWML, vL);
    analogWrite(MotorR_PWMR, vR);
}  // MotorWriting

// Handle negative motor_PWMR value.
//void MotorInverter(int motor, bool& dir) {
    // Hint: the value of motor_PWMR must between 0~255, cannot write negative value.
//}  // MotorInverter

void handleCommand(){
    switch (received)
    {
    case 'w':
        MotorWriting(_Tp, _Tp);
        break;
    case 'a':
        MotorWriting(0, _Tp);
        break;
    case 's':
        MotorWriting(-_Tp, -_Tp);
        break;
    case 'd':
        MotorWriting(_Tp, 0);
        break;
    case 'p':
        MotorWriting(0, 0);
        break;

    default:
        break;
    }
}


// P/PID control Tracking
void tracking(int l2, int l1, int m0, int r1, int r2) {
    // TODO: find your own parameters!
    static double           last_error = 0;
    static unsigned long    prevTime = 0;
    int sum_white = l2 + l1 + m0 + r1 + r2;

    double _w0 = 0;  //
    double _w1 = 2;  //
    double _w2 = 5;  //
    double _Kp = 25;  // p term parameter
    double _Kd = 15;  // d term parameter (optional)
    double _Ki;  // i term parameter (optional) (Hint: 不要調太大)
    double error = (l2 * _w2 + l1 * _w1 + m0 * _w0 + r1 * (-_w1) + r2 * (-_w2))/sum_white;
    double vR = 180, vL = 180;  // 馬達左右轉速原始值(從PID control 計算出來)。Between -255 to 255.
    double adj_R = 1, adj_L = 1.05;  // 馬達轉速修正係數。MotorWriting(_Tp,_Tp)如果歪掉就要用參數修正。

    // TODO: complete your P/PID tracking code

    unsigned long currentTime = micros();
    double dt = (currentTime - prevTime) * 1.0e6;
    int powerCorrection = _Kp * error + _Kd * (error - last_error) / dt;  // ex. Kp = 100, 也與w2 & w3有關
    last_error = error;
    prevTime = micros();

    int vR = _Tp - powerCorrection;
    int vL = _Tp + powerCorrection;

    // end TODO
    MotorWriting(adj_L * vL, adj_R * vR);
}  // tracking

void Takeinstruct(int cmd,int l2, int l1, int m0, int r1, int r2)
{
    switch (cmd)
    {
    case GO:
        MotorWriting(_Tp, _Tp);
        delay(100);
        while ( digitalRead(l1) == BLACK && digitalRead(m0) == BLACK && digitalRead(r1) == BLACK )
        {
            if( digitalRead(l2) == WHITE ){
                MotorWriting(_Tp, _Tp*0.95);
                delay(100);
            }
            if( digitalRead(r2) == WHITE ){
                MotorWriting(_Tp*0.95, _Tp);
                delay(100);
            }
        }
        break;
    case UTURN: // check
        if( digitalRead(l2) == WHITE ){
            MotorWriting(-_Tp * 1.3, _Tp * 0.6);
            delay(50);
        }
        MotorWriting(-_Tp * 1.2, _Tp);
        delay(300);
        MotorWriting(-_Tp * 0.8, _Tp*0.4);
        delay(200);
        while ( digitalRead(l2) == WHITE && digitalRead(l1) == WHITE )
        {
            MotorWriting(-_Tp*0.5, _Tp*0.4);
        }
        MotorWriting(_Tp*1.3, _Tp*0.6);
        delay(150);
        break;
    case RIGHT:
        MotorWriting(1.2 * _Tp, _Tp / 6);
        delay(330);
        while ( digitalRead(l1) == WHITE && digitalRead(m0) == WHITE )
        {
            MotorWriting(1.2 * _Tp, _Tp / 2);
        }
        MotorWriting(_Tp, _Tp*1.2);
        delay(100);
        break;
    case LEFT:
        MotorWriting(_Tp / 6, 1.2 * _Tp);
        delay(330);
        while ( digitalRead(m0) == WHITE && digitalRead(r1) == WHITE )
        {
            MotorWriting(_Tp / 2, 1.2 * _Tp);
        }
        MotorWriting(_Tp*1.2, _Tp);
        delay(100);
        break;
    }
    
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