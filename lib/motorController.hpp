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

// ==========================================================================
//
// motorController, accessed through PCA9685
//
// ==========================================================================
/**
 *  \brief motorController class. This abstract class that allows for more types of motor controllers to be implemented later on
 */
class motorController {
protected:
    PCA9685_i2c & PCA;
    const uint16_t FORWARDS;    /**< has a default value of 4096 but can be set to allow to run at lower speeds at max speed */
    const uint16_t BACKWARDS;   /**< has a default value of 0 but can be set to allow to run motors with an higher start value */

    bool direction;             /**<  true is forward, false backward */

public:
    /**
     * \brief abstract motorController class
     *
     * @param pca reference to the PCA object to interact with
     * @param forwards maximum speed to run motor forwards at
     * @param backwards maximum speed to run motor backwards at
     */
    motorController( PCA9685_i2c & pca, uint16_t forwards = 4096, uint16_t backwards = 0):
            PCA( pca ),
            FORWARDS( forwards ),
            BACKWARDS( backwards ),
            direction(true)
    {}

    /**
     *  \brief direction setter.
     *  @param forward bool used to indicate forward (true) or backward (false)
     */
    virtual void setDirection(bool forward = true) = 0;

    /**
     *  \brief speed setter.
     *  @param speed uint16_t with speed
     */
    virtual void setSpeed(uint16_t speed) = 0;

    /**
     *  \brief direction getter.
     */
    bool getDirection(){
        return direction;
    }
};

/**
 *  \brief genericDriver class. This child class was written to implement my workbench test setup
 *  it requires only one enable pin and one PWM pin
 */
class genericDriver : public motorController {
private:

    const uint8_t & pwmPin;
    const uint8_t & dirPin;

public:
    /**
     * \brief motorController child class constructor
     *
     * @param pca reference to the PCA object to interact with
     * @param PWMPIN pin on PCA used to send pwm to motor driver
     * @param DIRPIN pin on PCA used to indicate forwards or backwards to motor driver
     */
    genericDriver( PCA9685_i2c & pca, const uint8_t & PWMPIN, const uint8_t & DIRPIN ):
            motorController( pca ),
            pwmPin( PWMPIN ),
            dirPin( DIRPIN )
    {}

    void setDirection(bool forward) override {
        PCA.setPin(dirPin, (forward ? FORWARDS : BACKWARDS));
        direction = forward;
    }

    void setSpeed(uint16_t speed) override {
        PCA.setPin(pwmPin, speed);
    }
};

/**
 *  \brief IBT_2 class. This child class was written to implement my RCcar setup
 *  it requires two enable pin and one PWM pin
 */
class IBT_2 : public motorController {
private:

    const uint8_t & pwmPin;
    const uint8_t & dirPinForward;
    const uint8_t & dirPinBackward;

public:
    /**
     * \brief motorController child class constructor
     *
     * @param pca reference to the PCA object to interact with
     * @param pwmpin pin on PCA used to send pwm to motor driver
     * @param forward pin on PCA used to indicate forwards to motor driver
     * @param backward pin on PCA used to indicate backwards to motor driver
     */
    IBT_2(PCA9685_i2c & pca, const uint8_t & pwmpin, const uint8_t & forward, const uint8_t & backward ):
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
