/*
 *
 * Copyright Luc de Haas
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * https://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef IPASS_INPUT_H
#define IPASS_INPUT_H

/**
 *  inputController class. This abstract class allows for more types of controllers to be implemented later on
 */
class inputController {

    virtual uint16_t readX() = 0;
    virtual uint16_t readY() = 0;
};

#endif //IPASS_INPUT_H
