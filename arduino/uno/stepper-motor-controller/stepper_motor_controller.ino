/**
 * @author:  trieunvt
 * @file:    stepper_motor_controller.ino
 * @date:    15 Aug 2024
 * @version: v1.0.0
 * @brief:   The Arduino Uno stepper motor controller.
**/

/* The stspin220.h file */
#ifndef _STSPIN220_H_
#define _STSPIN220_H_

#include "Arduino.h"
#include "math.h"

/* For developing */
#define OUTPUT                  0

/* User-defined macros */
#define DEFAULT_STEPS_PER_REV   2048    /* Default steps per revolution when driving the stepper motor in the full-step mode */
#define DEFAULT_STEP_PERIOD     1       /* The period between 2 step clocks (ms) */

/* Developer-defined macros */
#define CLOCKWISE               0
#define COUNTER_CLOCKWISE       1
#define MODE_FULL_STEP          0
#define MODE_1P2_STEP           1
#define MODE_1P4_STEP           2
#define MODE_1P8_STEP           3
#define MODE_1P16_STEP          4
#define MODE_1P32_STEP          5
#define MODE_1P64_STEP          6
#define MODE_1P128_STEP         7
#define MODE_1P256_STEP         8
                                                    //  MODE3   MODE4   MODE1   MODE2
                                                    //   STCK    DIR
static unsigned int StepModeMap[9]   = {0b0000,     //    0       0       0       0     MODE_FULL_STEP
                                        0b1010,     //    1       0       1       0     MODE_1P2_STEP
                                        0b0101,     //    0       1       0       1     MODE_1P4_STEP
                                        0b1011,     //    1       0       1       1     MODE_1P8_STEP
                                        0b1111,     //    1       1       1       1     MODE_1P16_STEP
                                        0b0001,     //    0       0       0       1     MODE_1P32_STEP
                                        0b0111,     //    1       1       1       1     MODE_1P64_STEP
                                        0b0010,     //    0       0       1       0     MODE_1P128_STEP
                                        0b0011};    //    0       0       1       1     MODE_1P256_STEP

/* Struct type definitions for the following usage */
typedef struct STSPIN220_Class  STSPIN220_ClassTypeDef;
typedef struct STSPIN220_APIs   STSPIN220_APIsTypeDef;
typedef struct STSPIN220_Pins   STSPIN220_PinsTypeDef;

/* Stepper motor driver module class */
struct STSPIN220_Class {
    const STSPIN220_APIsTypeDef *api;
    STSPIN220_PinsTypeDef *pin;
    unsigned int stepMode;
    unsigned int stepPeriod;
    int isRunning;
};

/* Stepper motor driver module APIs */
struct STSPIN220_APIs {
    void (*init)(STSPIN220_ClassTypeDef *self, STSPIN220_PinsTypeDef *pin, unsigned int stepMode);

    void (*setStepMode)(STSPIN220_ClassTypeDef *self, unsigned int stepMode);
    unsigned int (*getStepMode)(STSPIN220_ClassTypeDef *self);

    void (*runSteps)(STSPIN220_ClassTypeDef *self, unsigned long stepNumber, unsigned int runDirection);
    void (*runRevolutions)(STSPIN220_ClassTypeDef *self, unsigned int revNumber, unsigned int runDirection);
    void (*stop)(STSPIN220_ClassTypeDef *self);

    void (*setDirection)(STSPIN220_ClassTypeDef *self, unsigned int runDirection);
    unsigned int (*getDirection)(STSPIN220_ClassTypeDef *self);

    void (*setStepPeriod)(STSPIN220_ClassTypeDef *self, unsigned int stepPeriod);
    unsigned int (*getStepPeriod)(STSPIN220_ClassTypeDef *self);
};

/* Stepper motor driver module pins */
struct STSPIN220_Pins {
    unsigned int pinStandbyReset;
    unsigned int pinMode1;
    unsigned int pinMode2;
    unsigned int pinMode3StepClock;
    unsigned int pinMode4Direction;
};

/* For everywhere usage */
extern STSPIN220_ClassTypeDef mySTSPIN220;

#endif /* _STSPIN220_H_ */

/* The stspin220.c file */
#include "stspin220.h"

/* APIs function declarations */
void init(STSPIN220_ClassTypeDef *self, STSPIN220_PinsTypeDef *pin, unsigned int stepMode);

void setStepMode(STSPIN220_ClassTypeDef *self, unsigned int stepMode);
unsigned int getStepMode(STSPIN220_ClassTypeDef *self);

void runSteps(STSPIN220_ClassTypeDef *self, unsigned long stepNumber, unsigned int runDirection);
void runRevolutions(STSPIN220_ClassTypeDef *self, unsigned int revNumber, unsigned int runDirection);
void stop(STSPIN220_ClassTypeDef *self);

void setDirection(STSPIN220_ClassTypeDef *self, unsigned int runDirection);
unsigned int getDirection(STSPIN220_ClassTypeDef *self);

void setStepPeriod(STSPIN220_ClassTypeDef *self, unsigned int stepPeriod);
unsigned int getStepPeriod(STSPIN220_ClassTypeDef *self);

/* The stepper motor driver module APIs declaration */
static STSPIN220_APIsTypeDef STSPIN220_APIs = {
    .init           = init,

    .setStepMode    = setStepMode,
    .getStepMode    = getStepMode,

    .runSteps       = runSteps,
    .runRevolutions = runRevolutions,
    .stop           = stop,

    .setDirection   = setDirection,
    .getDirection   = getDirection,

    .setStepPeriod  = setStepPeriod,
    .getStepPeriod  = getStepPeriod,
};

/* The stepper motor driver module class declaration */
STSPIN220_ClassTypeDef mySTSPIN220 = {
    .api = &STSPIN220_APIs,
};

/* Initialize the stepper motor driver module */
void init(STSPIN220_ClassTypeDef *self, STSPIN220_PinsTypeDef *pin, unsigned int stepMode) {
    self->pin = pin;

    pinMode(pin->pinStandbyReset, OUTPUT);
    pinMode(pin->pinMode1, OUTPUT);
    pinMode(pin->pinMode2, OUTPUT);
    pinMode(pin->pinMode3StepClock, OUTPUT);
    pinMode(pin->pinMode4Direction, OUTPUT);

    self->stepPeriod = DEFAULT_STEP_PERIOD;

    self->api->setStepMode(self, stepMode);
};

/* Set the step mode of the stepper motor driver module */
void setStepMode(STSPIN220_ClassTypeDef *self, unsigned int stepMode) {
    self->stepMode = stepMode;
    stepMode = StepModeMap[stepMode];

    digitalWrite(self->pin->pinStandbyReset, 0);
    delay(100);

    digitalWrite(self->pin->pinMode3StepClock, stepMode >> 3 & 0b1);
    digitalWrite(self->pin->pinMode4Direction, stepMode >> 2 & 0b1);
    digitalWrite(self->pin->pinMode1, stepMode >> 1 & 0b1);
    digitalWrite(self->pin->pinMode2, stepMode & 0b1);
    delay(100);
};

/* Get the step mode of the stepper motor driver module */
unsigned int getStepMode(STSPIN220_ClassTypeDef *self) {
    return self->stepMode;
};

/* Run the stepper motor driver module in the step mode */
void runSteps(STSPIN220_ClassTypeDef *self, unsigned long stepNumber, unsigned int runDirection) {
    self->isRunning = 1;

    digitalWrite(self->pin->pinStandbyReset, 1);
    delay(100);
    digitalWrite(self->pin->pinMode4Direction, runDirection);

    for (unsigned long i = 0; i < stepNumber; ++i) {
        digitalWrite(self->pin->pinMode3StepClock, 1);
        delay(self->stepPeriod);

        /* For testing */
        // if (i == DEFAULT_STEPS_PER_REV) {
        //     self->api->stop(self);                  /* Stop running */
        //     self->api->setStepPeriod(self, 1000);   /* Set the new step period */
        // }

        digitalWrite(self->pin->pinMode3StepClock, 0);
        delay(self->stepPeriod);
    }

    self->isRunning = 0;
};

/* Run the stepper motor driver module in the revolution mode */
void runRevolutions(STSPIN220_ClassTypeDef *self, unsigned int revNumber, unsigned int runDirection) {
    self->isRunning = 1;
    unsigned long stepNumber = revNumber * DEFAULT_STEPS_PER_REV * pow(2, self->stepMode);

    digitalWrite(self->pin->pinStandbyReset, 1);
    delay(100);
    digitalWrite(self->pin->pinMode4Direction, runDirection);

    for (unsigned long i = 0; i < stepNumber; ++i) {
        digitalWrite(self->pin->pinMode3StepClock, 1);
        delay(self->stepPeriod);

        /* For testing */
        // if (i == DEFAULT_STEPS_PER_REV) {
        //     self->api->stop(self);                  /* Stop running */
        //     self->api->setStepPeriod(self, 1000);   /* Set the new step period */
        // }

        digitalWrite(self->pin->pinMode3StepClock, 0);
        delay(self->stepPeriod);
    }

    self->isRunning = 0;
};

/* Stop the stepper motor driver module */
void stop(STSPIN220_ClassTypeDef *self) {
    self->api->setStepMode(self, self->stepMode);
    self->isRunning = 0;
};

/* Set the direction of the stepper motor driver module */
void setDirection(STSPIN220_ClassTypeDef *self, unsigned int runDirection) {
    digitalWrite(self->pin->pinMode4Direction, runDirection);
    delay(10);
};

/* Get the direction of the stepper motor driver module */
unsigned int getDirection(STSPIN220_ClassTypeDef *self) {
    return digitalRead(self->pin->pinMode4Direction);
};

/* Set the step period of the stepper motor driver module */
void setStepPeriod(STSPIN220_ClassTypeDef *self, unsigned int stepPeriod) {
    self->stepPeriod = stepPeriod / 2;
};

/* Get the step period of the stepper motor driver module */
unsigned int getStepPeriod(STSPIN220_ClassTypeDef *self) {
    return self->stepPeriod;
};

/* The main program */
void setup() {
    /* Initial pins declarations */
    STSPIN220_PinsTypeDef myPin = {
        .pinStandbyReset    = 8,
        .pinMode1           = 5,
        .pinMode2           = 10,
        .pinMode3StepClock  = 3,
        .pinMode4Direction  = 7,
    };

    /* Initialize the stepper motor driver module */
    mySTSPIN220.api->init(&mySTSPIN220, &myPin, MODE_1P2_STEP);

    /* Check the step mode and the step period */
    Serial.begin(115200);
    Serial.print("Step Mode: ");
    Serial.print(mySTSPIN220.api->getStepMode(&mySTSPIN220));
    Serial.print("\n");

    Serial.print("Step Period: ");
    Serial.print(mySTSPIN220.api->getStepPeriod(&mySTSPIN220));
    Serial.print("\n");
}

void loop() {
    /* Run 4096 steps clockwise */
    mySTSPIN220.api->runSteps(&mySTSPIN220, DEFAULT_STEPS_PER_REV * 2, CLOCKWISE);

    /* Change the running direction */
    mySTSPIN220.api->setDirection(&mySTSPIN220, ~(mySTSPIN220.api->getDirection(&mySTSPIN220)));

    /* Run 4096 steps counter-clockwise */
    mySTSPIN220.api->runSteps(&mySTSPIN220, DEFAULT_STEPS_PER_REV * 2, mySTSPIN220.api->getDirection(&mySTSPIN220));

    /* Set the new step mode */
    mySTSPIN220.api->setStepMode(&mySTSPIN220, MODE_FULL_STEP);

    /* Run 7 revolutions */
    mySTSPIN220.api->runRevolutions(&mySTSPIN220, 7, mySTSPIN220.api->getDirection(&mySTSPIN220));

    /* Do nothing infinitely */
    while (1) {
        delay(1000);
    }
}
