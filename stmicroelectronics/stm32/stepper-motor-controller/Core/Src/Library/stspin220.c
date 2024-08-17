/**
 * @author:  trieunvt
 * @file:    stspin220.c
 * @date:    15 Aug 2024
 * @version: v1.0.0
 * @brief:   The STSPIN220 stepper motor driver.
**/

#include "stspin220.h"

/* APIs function declarations */
void init(STSPIN220_ClassTypeDef *self, STSPIN220_PinsTypeDef *pin, uint16_t stepMode);

void setStepMode(STSPIN220_ClassTypeDef *self, uint16_t stepMode);
uint16_t getStepMode(STSPIN220_ClassTypeDef *self);

void runSteps(STSPIN220_ClassTypeDef *self, uint64_t stepNumber, uint16_t runDirection);
void runRevolutions(STSPIN220_ClassTypeDef *self, uint64_t revNumber, uint16_t runDirection);
void stop(STSPIN220_ClassTypeDef *self);

void setDirection(STSPIN220_ClassTypeDef *self, uint16_t runDirection);
uint16_t getDirection(STSPIN220_ClassTypeDef *self);

void setStepPeriod(STSPIN220_ClassTypeDef *self, uint32_t stepPeriod);
uint32_t getStepPeriod(STSPIN220_ClassTypeDef *self);

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
void init(STSPIN220_ClassTypeDef *self, STSPIN220_PinsTypeDef *pin, uint16_t stepMode) {
    self->pin = pin;
    self->stepPeriod = DEFAULT_STEP_PERIOD;

    self->api->setStepMode(self, stepMode);
};

/* Set the step mode of the stepper motor driver module */
void setStepMode(STSPIN220_ClassTypeDef *self, uint16_t stepMode) {
    self->stepMode = stepMode;
    stepMode = StepModeMap[stepMode];

    HAL_GPIO_WritePin(self->pin->pinStandbyResetPort, self->pin->pinStandbyResetPin, 0);
    HAL_Delay(100);

    HAL_GPIO_WritePin(self->pin->pinMode3StepClockPort, self->pin->pinMode3StepClockPin, stepMode >> 3 & 0b1);
    HAL_GPIO_WritePin(self->pin->pinMode4DirectionPort, self->pin->pinMode4DirectionPin, stepMode >> 2 & 0b1);
    HAL_GPIO_WritePin(self->pin->pinMode1Port, self->pin->pinMode1Pin, stepMode >> 1 & 0b1);
    HAL_GPIO_WritePin(self->pin->pinMode2Port, self->pin->pinMode2Pin, stepMode & 0b1);
    HAL_Delay(100);
};

/* Get the step mode of the stepper motor driver module */
uint16_t getStepMode(STSPIN220_ClassTypeDef *self) {
    return self->stepMode;
};

/* Run the stepper motor driver module in the step mode */
void runSteps(STSPIN220_ClassTypeDef *self, uint64_t stepNumber, uint16_t runDirection) {
    self->isRunning = 1;

    HAL_GPIO_WritePin(self->pin->pinStandbyResetPort, self->pin->pinStandbyResetPin, 1);
    HAL_Delay(100);
    HAL_GPIO_WritePin(self->pin->pinMode4DirectionPort, self->pin->pinMode4DirectionPin, runDirection);

    for (uint64_t i = 0; i < stepNumber; ++i) {
        HAL_GPIO_WritePin(self->pin->pinMode3StepClockPort, self->pin->pinMode3StepClockPin, 1);
        HAL_Delay(self->stepPeriod);

        /* For testing */
        // if (i == DEFAULT_STEPS_PER_REV) {
        //     self->api->stop(self);                  /* Stop running */
        //     self->api->setStepPeriod(self, 1000);   /* Set the new step period */
        // }

        HAL_GPIO_WritePin(self->pin->pinMode3StepClockPort, self->pin->pinMode3StepClockPin, 0);
        HAL_Delay(self->stepPeriod);
    }

    self->isRunning = 0;
};

/* Run the stepper motor driver module in the revolution mode */
void runRevolutions(STSPIN220_ClassTypeDef *self, uint64_t revNumber, uint16_t runDirection) {
    self->isRunning = 1;
    uint64_t stepNumber = revNumber * DEFAULT_STEPS_PER_REV * pow(2, self->stepMode);

    HAL_GPIO_WritePin(self->pin->pinStandbyResetPort, self->pin->pinStandbyResetPin, 1);
    HAL_Delay(100);
    HAL_GPIO_WritePin(self->pin->pinMode4DirectionPort, self->pin->pinMode4DirectionPin, runDirection);

    for (uint64_t i = 0; i < stepNumber; ++i) {
        HAL_GPIO_WritePin(self->pin->pinMode3StepClockPort, self->pin->pinMode3StepClockPin, 1);
        HAL_Delay(self->stepPeriod);

        /* For testing */
        // if (i == DEFAULT_STEPS_PER_REV) {
        //     self->api->stop(self);                  /* Stop running */
        //     self->api->setStepPeriod(self, 1000);   /* Set the new step period */
        // }

        HAL_GPIO_WritePin(self->pin->pinMode3StepClockPort, self->pin->pinMode3StepClockPin, 0);
        HAL_Delay(self->stepPeriod);
    }

    self->isRunning = 0;
};

/* Stop the stepper motor driver module */
void stop(STSPIN220_ClassTypeDef *self) {
    self->api->setStepMode(self, self->stepMode);
    self->isRunning = 0;
};

/* Set the direction of the stepper motor driver module */
void setDirection(STSPIN220_ClassTypeDef *self, uint16_t runDirection) {
    HAL_GPIO_WritePin(self->pin->pinMode4DirectionPort, self->pin->pinMode4DirectionPin, runDirection);
    HAL_Delay(10);
};

/* Get the direction of the stepper motor driver module */
uint16_t getDirection(STSPIN220_ClassTypeDef *self) {
    return HAL_GPIO_ReadPin(self->pin->pinMode4DirectionPort, self->pin->pinMode4DirectionPin);
};

/* Set the step period of the stepper motor driver module */
void setStepPeriod(STSPIN220_ClassTypeDef *self, uint32_t stepPeriod) {
    self->stepPeriod = stepPeriod / 2;
};

/* Get the step period of the stepper motor driver module */
uint32_t getStepPeriod(STSPIN220_ClassTypeDef *self) {
    return self->stepPeriod;
};
