#ifndef PINS_HPP
#define PINS_HPP 1

const int digitalPin[5] = {32, 34, 36, 38, 40};   

enum PIN{
    // bt
    RX = 10,
    TX = 11,

    // right motor
    PWMA = 12,
    AIN1 = 2,
    AIN2 = 3,

    // left motor
    PWMB = 13,
    BIN1 = 5,
    BIN2 = 6,

    //rfid
    RST_PIN = 9,
    SS_PIN  = 53,
};

#endif