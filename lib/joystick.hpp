/*
 *
 * Copyright Luc de Haas
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * https://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef IPASS_JOYSTICK_HPP
#define IPASS_JOYSTICK_HPP

#include <hwlib.hpp>
#include "inputController.hpp"

/**
 *  joystickController.
 *  this class implements abstract class inputController so that readX and ReadY are always implemented.
 */
class joystickController: public inputController {
private:
    hwlib::pin_in &click;
    hwlib::adc &X;
    hwlib::adc &Y;

    uint16_t xCoor = 2075;  /**< uint16_t value of X. defaults to centerpoint. */
    uint16_t yCoor = 2075;  /**< uint16_t value of Y. defaults to centerpoint. */

    /**
     * \brief reads from all analog pins and updates xCoor and yCoor
     */
    void checkJoystick();

public:
    /**
     * \brief joystickController constructor
     *
     * @param click pin used to detect joystick click on. This value will return true when not pressed due to the pull up
     * @param x analog to digital pin used to detect joystick movement along x axis.
     * @param y analog to digital pin used to detect joystick movement along y axis.
     */
    joystickController (hwlib::pin_in & click, hwlib::adc & x, hwlib::adc & y);

    /**
     * \brief returns whether the joystick is clicked
     * note that this function will return true when *not* pressed due to the pull up
     */
    bool clicked();

    /**
     * \brief returns value of current X axis
     */
    uint16_t readX() override;

    /**
     * \brief return value of current Y axis
     */
    uint16_t readY() override;
};

#endif //IPASS_JOYSTICK_HPP
