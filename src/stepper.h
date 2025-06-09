#include <AccelStepper.h> // Use AccelStepper library

// Motor control pins
const int enablePin = 8;
const int xdirPin = 5;
const int xstepPin = 2;
const int ydirPin = 6;
const int ystepPin = 3;
const int zdirPin = 7;
const int zstepPin = 4;
const int adirPin = 13;
const int astepPin = 12;

const int moveSteps = 800; // Test steps

// Define stepper motors
AccelStepper stepperFR(1, xstepPin, xdirPin); // Front Right
AccelStepper stepperBR(1, ystepPin, ydirPin); // Back Right
AccelStepper stepperFL(1, zstepPin, zdirPin); // Front Left
AccelStepper stepperBL(1, astepPin, adirPin); // Back Left

enum Movement {
  forward,
  backward,
  left,
  right,
  leftturn,
  rightturn
};

bool arrived(){
  return ! (stepperBL.distanceToGo() || stepperBR.distanceToGo() || stepperFL.distanceToGo() || stepperFR.distanceToGo() );
}

bool stepperControl(AccelStepper* theStepper, int target, int speed) {
  if (theStepper == &stepperBL) {
    target = -target; // Reverse direction for back left motor
  }

  theStepper->setMaxSpeed(speed);
  theStepper->moveTo(target);
  theStepper->run();

  return theStepper->currentPosition() == target;
}

void carControl(int target, int speed, Movement movement) {
  int tFR = 0, tBR = 0, tFL = 0, tBL = 0;

  switch (movement) {
    case forward:
      tFR = tBR = tFL = tBL = target;
      break;
    case backward:
      tFR = tBR = tFL = tBL = -target;
      break;
    case left:
      tFL = tBR = target;
      tFR = tBL = -target;
      break;
    case right:
        tFL = tBR = -target;
        tFR = tBL = target;
      break;
    case leftturn:
      tFR = tBR = target;
      tFL = tBL = -target;
      break;
    case rightturn:
      tFR = tBR = -target;
      tFL = tBL = target;
      break;
  }

  stepperControl(&stepperFR, tFR, speed);
  stepperControl(&stepperBR, tBR, speed);
  stepperControl(&stepperFL, tFL, speed);
  stepperControl(&stepperBL, tBL, speed);

  stepperFR.run();
  stepperBR.run();
  stepperFL.run();
  stepperBL.run();
}

void stepperSetup() {
  pinMode(xstepPin, OUTPUT);
  pinMode(xdirPin, OUTPUT);
  pinMode(ystepPin, OUTPUT);
  pinMode(ydirPin, OUTPUT);
  pinMode(zstepPin, OUTPUT);
  pinMode(zdirPin, OUTPUT);
  pinMode(astepPin, OUTPUT);
  pinMode(adirPin, OUTPUT);

  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, LOW); // Enable motors

  stepperFR.setMaxSpeed(300.0);
  stepperFR.setAcceleration(500.0);
  stepperBR.setMaxSpeed(300.0);
  stepperBR.setAcceleration(500.0);
  stepperFL.setMaxSpeed(300.0);
  stepperFL.setAcceleration(500.0);
  stepperBL.setMaxSpeed(300.0);
  stepperBL.setAcceleration(500.0);
}

void stepperRun() {
    // carControl(2048, 400, forward);
    // carControl(2048, 400, backward);
    carControl(20480, 400, left);
    // carControl(2048, 400, right);
}
