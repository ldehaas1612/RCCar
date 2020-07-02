/*
 *
 * Copyright Luc de Haas
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * https://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include <hwlib.hpp>

/**
 * \class this class handles all the receiving of the 433mhz signals
 * it implements the custom protocol
 */
class Receiver433mhz {
private:
    bool       ReceiverHighFlag;    /**< ReceiverHighFlag is a boolean that is set when the input value falls to indicate the duration of the pulse */
    bool       ReceiverLowFlag;     /**< ReceiverLowFlag is a boolean that is set when a HIGH value is read and a timer is started */
    uint_fast64_t      bitTimer;
    hwlib::pin_in      &input;

    uint8_t array[64]  = {0};
    int     time_start = 0;
    int     time_end   = 0;
    uint8_t count     = 0;

    bool validMessage = false;

    /**
     * \brief enum class to create states that better describe where the progress is
     */
    enum class state_t {
        IDLE, TIMING, TIMING_DONE
    };
    state_t state      = state_t::IDLE;

public:
    /**
     * \brief Standard constructor
     *
     * @param input input pin for an 433mhz receiver
     */
    Receiver433mhz(hwlib::pin_in &input);

    /**
     * \brief setter function for ReceiverLowFlag
     *
     * @param b boolean to set the ReceiverLowFlag to
     */
    void setReceiverLowFlag(bool b);

    /**
     * \brief setter function for ReceiverHighFlag
     *
     * @param b boolean to set the ReceiverHighFlag to
     */
    void setReceiverHighFlag(bool b);

    /**
     * \brief getter to check if a new message is available and unpacked to be send to the PCA board
     */
    bool msgAvailable();

    /**
     * \brief function to be called every loop cycle to check for incoming messages
     */
    void messageLoop();

    uint8_t finishedArray[];
};

/**
 * \class this class decodes the message as it's received from the ReceiverClass and turns it into usable
 * values for the controlling of an RC car
 */
class messageReceiver {
private:
    Receiver433mhz & receiver;                   /**< receiver433mhz class for intern use */

    bool motorDir; // true being forward
    bool servoDir; // true being right
    uint16_t Yval; // unsigned int (0 - 1024)
    uint16_t Xval; // unsigned int (0 - 512)
    uint16_t Checksum; // checksum of bits 2 to 32

    bool validMessage = false;

public:
    messageReceiver( Receiver433mhz & rcv ):
        receiver(rcv)
    {}

    /**
     * \brief getter for motorDir
     */
    bool getMotorDir();

    /**
     * \brief getter for servoDir
     */
    bool getServoDir();

    /**
     * \brief getter for Y
     */
    uint16_t getY();

    /**
     * \brief getter for X
     */
    uint16_t getX();

    /**
     * \brief getter to check if a new message is available and unpacked to be send to the PCA board
     */
    bool msgAvailable();

    /**
     * \brief this function unpacks the given array into usable variables
     *
     * @param arr array of uint8_t's that are the equivalent of the full message sent by the transmitter
     */
    bool decodeMessage(uint8_t arr[]);

    /**
     * \brief function verifies the given XOR with the given left and right values to check if the message is valid
     *
     * @param left left value of the XOR equation
     * @param right right value of the XOR equation
     * @param XOR XOR value to evaluate to the XOR of the above parameters
     */
    bool checksum(const uint16_t & left, const uint16_t & right, const uint16_t & XOR);

    void checkLoop();
};