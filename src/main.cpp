#include <AccelStepper.h>
#include "sg90.h"
#include "stepper.h"

void setup()
{
    sg90Setup();
    stepperSetup();
    delay(1000);
}

void loop()
{
    static int yee = 1;
    sg90Run2(180); // sdfghji876trfghbvderghjiuytfcserfgbndfghhdfghdfhfghdgfhd
    // delay(300);
    carControl(300 * yee, 300, backward);
    while( !arrived() ){
        carControl(300 * yee, 300, backward);
    }
    yee++;

}