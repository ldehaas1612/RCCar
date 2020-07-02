/*
 *
 * Copyright Luc de Haas
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * https://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include <hwlib.hpp>


class Transmit433mhzController{
	hwlib::pin_out & Transmitter;

protected:
	/**
	 * \brief send a 1 over 433mhz
	 */
	void send_one();

	/**
	 * \brief send a 0 over 433mhz
	 */
	void send_zero();

	/**
	 * \brief send one byte over 433mhz
	 *
	 * @param B the byte that should be send
	 */
	void sendByte(uint8_t B);

public:

    /**
     * \brief Standard constructor
     *
     * @param transmitter pin of transmitter to output to
     */
    Transmit433mhzController(hwlib::pin_out & transmitter);

    /**
	 * \brief function to send a multibyte message over 433mhz
	 *
	 * @param data an array of uint8_t's that have the data
	 * @param size the size of the array
	 * @param count the amount of times the message should be send
	 * @param delay_ms the amount of miliseconds between each message
	 */
	void sendMessage(uint8_t data[], size_t size, int count, int delay_ms);

    /**
     * \brief send one byte over 433mhz
     * this function will send out a bunch of zero's to keep the connection alive
     */
    void keepAlive();
};

class constructMessage {
private:
    Transmit433mhzController transmitter;                   /**< transmit433mhz class for intern use */
    uint8_t transmitData[4] = { 0x00, 0x00, 0x00, 0x00 };   /**< array of 4 uint8_t that make up a complete message */
    uint16_t X = 0;                                         /**< uint16_t value of X */
    uint16_t Y = 0;                                         /**< uint16_t value of Y */
    uint16_t XOR = 0;                                       /**< uint16_t value of XOR */
    bool mdirFlag = false, sdirFlag = false, YFlag = false, XFlag = false, motorDirection = true, servoDirection = false;

public:
    /**
     * \brief Standard constructor
     *
     * @param transmitter pin of transmitter to output to.
     * the constructor creates it's own Transmit433mhzController using the transmitterPin provided
     */
    constructMessage(hwlib::pin_out & transmitter);

    /**
     * \brief Set the motor direction
     *
     * @param dir Direction to set motorDirection to.
     */
    void setMotorDir(bool dir);

    /**
     * \brief Set the servo direction
     *
     * @param dir Direction to set servoDirection to.
     */
    void setServoDir(bool dir);

    /**
     * \brief Set the Y value
     *
     * @param yValue speed to set motor to.
     */
    void setY(uint16_t yValue);

    /**
     * \brief Set the X value
     *
     * @param xValue rotation to set motor to.
     */
    void setX(uint16_t xValue);

    /**
	 * \brief function to adapt a input variable from it's max range of numbers to a comparable new value within a new range
	 *
	 * @param value value that need to be scaled
	 * @param oldMin old minimum value of value variable
	 * @param oldMax old maximum value of value variable
	 * @param newMin new minimum value of value variable
     * @param newMax new maximum value of value variable
	 */
    static uint16_t adapter(const uint16_t & value, const uint16_t & oldMin, const uint16_t & oldMax, const uint16_t & newMin, const uint16_t & newMax);

    /**
	 * \brief function that creates a checksum.
     * this checksum is created by doing a XOR operation on the left and right value.
	 *
	 * @param left first value to be XOR'ed with
	 * @param right second value to be XOR'ed with
	 */
    static uint16_t makeChecksum(const uint16_t & left, const uint16_t & right);

    /**
     * \brief this function need to be called repeatedly in order to check if there are new values to be sent out
     * note that this function sends out a keepalive signal to the 433mhz transmitter to idle the 433mhz chip
     */
    void makeMessage();
};
