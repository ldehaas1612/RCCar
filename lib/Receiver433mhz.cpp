/*
 *
 * Copyright Luc de Haas
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * https://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "Receiver433mhz.hpp"


Receiver433mhz::Receiver433mhz(hwlib::pin_in &input) :
    input(input)
{}


void Receiver433mhz::setReceiverLowFlag(bool b) {
    ReceiverLowFlag = b;
}

void Receiver433mhz::setReceiverHighFlag(bool b) {
    ReceiverHighFlag = b;
}

bool Receiver433mhz::getMotorDir(){
    return motorDir;
}

bool Receiver433mhz::getServoDir(){
    return servoDir;
}

uint16_t Receiver433mhz::getY(){
    return Yval;
}

uint16_t Receiver433mhz::getX(){
    return Xval;
}

bool Receiver433mhz::messageAvailable(){
    return validMessage;
}

bool Receiver433mhz::decodeMessage(uint8_t arr[]){
    uint32_t fullMessage = arr[3] | (arr[2] << 8) | (arr[1] << 16) | (arr[0] << 24);
    //hwlib::cout << "decimal fullmessage: " << fullMessage << hwlib::endl;
    Checksum = fullMessage & 0b1111111111;
    fullMessage = fullMessage >> 10;
    servoDir = fullMessage & 0x01;
    fullMessage = fullMessage >> 1;
    Xval = fullMessage & 0b111111111;
    fullMessage = fullMessage >> 9;
    Yval = fullMessage & 0b1111111111;
    fullMessage = fullMessage >> 10;
    motorDir = fullMessage & 0x01;
    fullMessage = fullMessage >> 1;
    //hwlib::cout << "motorDir: " << motorDir << " servoDir: " << servoDir << " Y: " << Yval << " X: " << Xval << " checksum: " << Checksum << hwlib::endl;
    return checksum(Yval, ((Xval << 1) | (int) servoDir), Checksum);
}

bool Receiver433mhz::checksum(const uint16_t & left, const uint16_t & right, const uint16_t & XOR){
    return (left ^ right) == XOR;
}

void Receiver433mhz::messageLoop(){
    if(validMessage){
        validMessage = false;
    }
    if(!ReceiverLowFlag){
        setReceiverLowFlag(input.read());
    }

    switch (state) {
        /**
         * in this state the code waits for the receiver to go high
         */
        case state_t::IDLE:
            if(ReceiverLowFlag) {
                count = 0;
                state = state_t::TIMING;
            }
            break;

        /**
         * in this state the code times how long the receiver is high
         */
        case state_t::TIMING:
            if(time_start == 0){
                time_start = hwlib::now_us();
            }
            setReceiverHighFlag(!input.read());
            if(ReceiverHighFlag){
                time_end = hwlib::now_us();
                setReceiverLowFlag(false);
                bitTimer = hwlib::now_us();
                state    = state_t::TIMING_DONE;
            }
            break;

        /**
         * in this state the code uses the time that the receiver was high to construct the message
         * if it takes longer than 2 ms to get a new bit (receiver high) the message is considered finished
         * and the message is send to the listener
         */
        case state_t::TIMING_DONE:

            if(ReceiverHighFlag) {
                if (time_end - time_start > 300) {
                    array[count / 8] |= 1u << (7 - (count % 8));
                }
                count++;
                setReceiverHighFlag(false);
            }

            // new bit took longer then 2ms and still no new pulse. When count is 8 the message is just a keepalive
            // only true when waiting longer then 2 ms |  true when Flag is false |  if the counter is more then 8
            if ((hwlib::now_us() - bitTimer > 2000) && !ReceiverLowFlag && count > 8) {
                validMessage = decodeMessage(array);
                for(size_t i = 0; i < count/8; i++) {
                    array[i] = 0x00;
                }
                state = state_t::IDLE;
            } else if (ReceiverLowFlag) {
                time_start = 0;
                state = state_t::TIMING;
            }
            break;
    }
}