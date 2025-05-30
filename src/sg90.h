#include <Arduino.h>
#include <Servo.h> //載入函式庫，這是內建的，不用安裝

Servo myservo; // 建立SERVO物件

void sg90Setup()
{
    myservo.attach(22); // 設定要將伺服馬達接到哪一個PIN腳
}

void sg90Run(double target, int rpm)
{
    static double angle = 0;
    static int flag = 1;
    double speed = rpm * 6.2 / 200; // when no delay
    if(angle >= 180 && flag == 1){
        flag = -1;
    }else if ( angle < 0 && flag == -1 ){
        flag = 1;
    }
    angle += flag * speed;
    myservo.write(angle);
}