/*
 *
 * Copyright Luc de Haas
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * https://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "joystick.hpp"

namespace target = hwlib::target;

joystickController::joystickController (hwlib::pin_in & click, hwlib::adc & x, hwlib::adc & y):
        click(click),
        X(x),
        Y(y)
{}

bool joystickController::clicked() {
    return click.read();

}

uint16_t joystickController::readX() {
    checkJoystick();
    return xCoor;
}

uint16_t joystickController::readY() {
    checkJoystick();
    return yCoor;
}

void joystickController::checkJoystick() {
    xCoor = X.read();
    yCoor = Y.read();
}
