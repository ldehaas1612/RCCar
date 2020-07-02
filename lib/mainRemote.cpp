#include "hwlib.hpp"
#include "joystick.hpp"
#include "Transmit433mhzController.hpp"
#include "MovingAverage.hpp"

int main() {

    // wait for the PC console to start
    hwlib::wait_ms(500);

    namespace target = hwlib::target;

    auto click = target::pin_in(target::pins::d2);
    click.pullup_enable();
    auto X = due::pin_adc(due::ad_pins::a0);
    auto Y = due::pin_adc(due::ad_pins::a1);
    joystickController joy(click, X, Y);

    auto transmitter = target::pin_out(target::pins::d9);
    constructMessage message(transmitter);

    // Motordriver controller

    uint16_t direction = true;              //true meaning forward, false meaning backwards

    bool MotortimerRunning      = false;       // timer running
    uint32_t timeBetweenStep     = 100;         // = versnellingsstap tijd (Time between each step update)
    uint16_t Stepsize = 80;         // = volgende PWM waarde (Value of the increment per step)
    uint16_t motorAcceleration = 0;           // = huidige snelheid (Current PWM value written to board)
    int16_t targetSpeed = 0;       // = gewenste snelheid (Target PWM value as read from joystick)
    int16_t previousSpeed = 0;         // staat stil (Previous value of acceleration)
    int8_t  speedUp_slowDown = 1;   // 1 = versnellen -1 = vertragen 0 = idle
    uint_fast64_t motortimer;                // variable that holds the timer

    MovingAverage <uint16_t> joyYAverage(50);

    // Servodriver controller

    bool servoTimerRunning      = false;       // timer running
    uint32_t timeBetweenServoStep     = 100;         // = versnellingsstap tijd (Time between each step update)
    uint16_t servoStepsize = 200;         // = volgende PWM waarde (Value of the increment per step)
    uint_fast64_t servotimer;                // variable that holds the timer
    int16_t targetRotation = 0;       // = (Target PWM value as read from joystick)
    int16_t targetDirection = 0;       // = temp value to hold increase/decrease value
    int16_t previousRotation = 0;         // (Previous value of rotation)
    int8_t  changePosition = 1;   // 1 = versnellen -1 = vertragen 0 = idle
    int16_t servoRotation = 0;           // = huidige snelheid (Current PWM value written to board)
    MovingAverage <uint16_t> joyXAverage(20);


    volatile bool _true = true;
    while (_true) {

        // read joystick value and remap to -2048 - +2048
        targetSpeed = joyYAverage.CalculateMovingAverage(joy.readY());// - 2048) * 2;
        targetRotation = joyXAverage.CalculateMovingAverage(joy.readX());
        //hwlib::cout << "joystick: " << targetRotation;

        targetSpeed = (targetSpeed - 2048) * 2;
        targetRotation = (targetRotation - 2048) * 2;
        //hwlib::cout << "  na normalisatie: " << targetRotation;

        if (targetSpeed >= -50 && targetSpeed <= 150){
            targetSpeed = 0;
        }
        if (targetRotation >= -100 && targetRotation <= 100){
            targetRotation = 0;
        }
        //hwlib::cout << "  na deadzone: " << targetRotation;

        // Pick direction
        if (targetSpeed < 0){
            direction = true;
        } else if (targetSpeed > 0) {
            direction = false;
        }

        // make speed absolute
        if (targetSpeed < 0){
            targetSpeed *= -1;
        }
        // make rotation temporarily absolute for direction control
        if (targetRotation < 0){
            targetDirection = targetRotation * -1;
        } else {
            targetDirection = targetRotation;
        }

        if (targetSpeed > previousSpeed) {
            speedUp_slowDown = 1;
        } else if (targetSpeed < previousSpeed) {
            speedUp_slowDown = -1;
        }else {
            speedUp_slowDown = 0;
        }
        if (targetDirection > previousRotation) {
            changePosition = 1;
        } else if (targetDirection < previousRotation) {
            changePosition = -1;
        }else {
            changePosition = 0;
        }

        //hwlib::cout << " Joystick abs value: " << targetRotation;

        // decide to move
        if (speedUp_slowDown != 0) {
            if (!MotortimerRunning) {
                motortimer = hwlib::now_us();
                MotortimerRunning = true;
            }
        }
        if (changePosition != 0) {
            if (!servoTimerRunning) {
                servotimer = hwlib::now_us();
                servoTimerRunning = true;
            }
        }

        // decide direction to move in
        if (MotortimerRunning) {
            if (hwlib::now_us() - motortimer > timeBetweenStep){

                // change acceleration
                motorAcceleration += (Stepsize * speedUp_slowDown);
                if (motorAcceleration >= targetSpeed) {
                    motortimer       = 0;
                    MotortimerRunning  = false;
                    motorAcceleration = targetSpeed;
                } else {
                    // restart timer
                    motortimer = hwlib::now_us();
                }
            }
        }

        if (servoTimerRunning) {
            if (hwlib::now_us() - servotimer > timeBetweenServoStep){

                // change acceleration
                servoRotation += (servoStepsize * changePosition);
                if (servoRotation >= targetRotation) {
                    servotimer       = 0;
                    servoTimerRunning  = false;
                    servoRotation = targetRotation;
                } else {
                    // restart timer
                    servotimer = hwlib::now_us();
                }
            }
        }

        //hwlib::cout << " Servo is: " << servoRotation << "  ";

        if (speedUp_slowDown != 0) {
            message.setMotorDir(direction);
            //motor.setDirection(direction);
            //hwlib::cout << "motor: " << motorAcceleration << "  ";
            message.setY(motorAcceleration);
            //motor.setSpeed(motorAcceleration);
            previousSpeed = motorAcceleration;
        }
        if (changePosition != 0) {
            if(servoRotation < 0){
                message.setServoDir(false);
                message.setX(servoRotation*-1);
            } else {
                message.setServoDir(true);
                message.setX(servoRotation);
            }
            //ser.setPosition(ser.remapperInverse(servoRotation));
            previousRotation = servoRotation;
        }
        message.makeMessage();
    }
}