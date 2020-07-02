/*
 *
 * Copyright Luc de Haas
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * https://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef RCCAR_MOTORCONTROLLER_HPP
#define RCCAR_MOTORCONTROLLER_HPP

#include "hwlib.hpp"
#include "PCA9685.hpp"

class motorController {
protected:
    PCA9685_i2c & PCA;
    const uint16_t FORWARDS;
    const uint16_t BACKWARDS;

    bool direction; // true is forward, false backward

public:

    motorController( PCA9685_i2c & pca, uint16_t forwards = 4095, uint16_t backwards = 0):
            PCA( pca ),
            FORWARDS( forwards ),
            BACKWARDS( backwards ),
            direction(true)
    {}

    virtual void setDirection(bool forward = true) = 0;
    virtual void setSpeed(uint16_t speed) = 0;

    bool getDirection(){
        return direction;
    }
};

class genericDriver : public motorController {
private:

    const uint16_t & pwmPin;
    const uint16_t & dirPin;

public:
    genericDriver( PCA9685_i2c & pca, const uint16_t & PWMPIN, const uint16_t & DIRPIN ):
            motorController( pca ),
            pwmPin(PWMPIN ),
            dirPin(DIRPIN )
    {}

    void setDirection(bool forward) override {
        PCA.setPin(dirPin, (forward ? FORWARDS : BACKWARDS));
        direction = forward;
    }

    void setSpeed(uint16_t speed) override {
        PCA.setPin(pwmPin, speed);
    }
};

class IBT_2 : public motorController {
private:

    const uint16_t & pwmPin;
    const uint16_t & dirPinForward;
    const uint16_t & dirPinBackward;

public:
    IBT_2(PCA9685_i2c & pca, const uint16_t & pwmpin, const uint16_t & forward, const uint16_t & backward ):
            motorController( pca ),
            pwmPin(pwmpin ),
            dirPinForward(forward ),
            dirPinBackward(backward )
    {}

    void setDirection(bool forward) override {
        PCA.setPin(dirPinForward, (forward ? FORWARDS : BACKWARDS));
        PCA.setPin(dirPinBackward, (forward ? BACKWARDS : FORWARDS));
        direction = forward;
    }

    void setSpeed(uint16_t speed) override {
        PCA.setPin(pwmPin, speed);
    }
};

#endif //RCCAR_MOTORCONTROLLER_HPP
