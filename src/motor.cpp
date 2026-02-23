#include "motor.h"
#include "encoder.h"
#include "main.h"
#include "WebServerEthernet.h"
#include "AccelStepper.h"

int StepsToMove = 0;
bool calibration = false;
#define DIR PE2            // D2
#define PUL PE4            // D3
#define MotorCalibrate PE5 // D4
#define MotorRun PE6       // D5
AccelStepper stepper(AccelStepper::FULL2WIRE, PUL, DIR);
bool runMotor = false;
void mottor_setup()
{
    stepper.setMaxSpeed(1000);
    stepper.setSpeed(100);
    pinMode(MotorCalibrate, INPUT);
    pinMode(MotorRun, INPUT);
}
void calibrate_encoder()
{
    if (raw_count != 0)
    {
        calibration = true;
        stepper.move(1);
    }
    else
    {
        calibration = false;
    }
}
void MoveMottor()
{
    stepper.runSpeed();
}
void mottor_loop()
{
    static bool lastCalibrateStateButon;
    static bool CalibrateButonTimer = millis();
    static bool lastRunStateButon;
    static bool RunButonTimer = millis();
    bool currentCalibrateStateButon = digitalRead(MotorCalibrate);
    bool currentRunStateButon = digitalRead(MotorRun);
    if (calibration == false || (currentCalibrateStateButon == HIGH && lastCalibrateStateButon == LOW && (millis() - CalibrateButonTimer >= 100)))
    {
        calibrate_encoder();
        CalibrateButonTimer = millis();
    }
    if (currentRunStateButon == HIGH && lastRunStateButon == LOW && (millis() - RunButonTimer >= 100))
    {
        runMotor = !runMotor;
        RunButonTimer = millis();
    }
    if (runMotor == true)
    {
        MoveMottor();
    }else
    {
        stepper.stop();
    }
    
    lastCalibrateStateButon = currentCalibrateStateButon;
    lastRunStateButon = currentRunStateButon;
    stepper.run();
}