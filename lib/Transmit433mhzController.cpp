/*
 *
 * Copyright Luc de Haas
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * https://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "Transmit433mhzController.hpp"


Transmit433mhzController::Transmit433mhzController(hwlib::pin_out & transmitter) : Transmitter(transmitter) {}

void Transmit433mhzController::send_one(){
    Transmitter.write(1);
    Transmitter.flush();
	hwlib::wait_us(400);
    Transmitter.write(0);
    Transmitter.flush();
	hwlib::wait_us(200);
}

void Transmit433mhzController::send_zero(){
    Transmitter.write(1);
    Transmitter.flush();
	hwlib::wait_us(200);
    Transmitter.write(0);
    Transmitter.flush();
	hwlib::wait_us(400);
}

void Transmit433mhzController::sendByte(uint8_t B){
	for(int i = 0; i < 8; i++){
		if(B & 0b10000000){
			send_one();
		}else{
			send_zero();
		}
		B = B << 1;
	}
}

void Transmit433mhzController::sendMessage(uint8_t data[], size_t size, int count, int delay_ms){
	for(int i = 0; i < count; i++) {
		for (size_t j = 0; j < size; j++) {
			sendByte(data[j]);
		}
		hwlib::wait_ms(delay_ms);
	}
}

void Transmit433mhzController::keepAlive(){
    uint8_t dummydata[] = {0x00};
    sendMessage(dummydata, 1, 1, 3);
}

constructMessage::constructMessage(hwlib::pin_out & transmitter):
    transmitter(Transmit433mhzController( transmitter ))
{}

void constructMessage::setMotorDir(bool dir) {
    motorDirection = dir;
    mdirFlag = true;
}

void constructMessage::setServoDir(bool dir) {
    servoDirection = dir;
    sdirFlag = true;
}

void constructMessage::setY(uint16_t yValue){
    Y = yValue;
    YFlag = true;
}

void constructMessage::setX(uint16_t xValue){
    X = xValue;
    XFlag = true;
}

uint16_t constructMessage::adapter(const uint16_t & value, const uint16_t & oldMin, const uint16_t & oldMax, const uint16_t & newMin, const uint16_t & newMax) {
    return newMin + (float)(newMax - newMin) * ((float)(value - oldMin) / (float)(oldMax - oldMin));
}

uint16_t constructMessage::makeChecksum(const uint16_t & left, const uint16_t & right){
    return left ^ right;
}

void constructMessage::makeMessage(){
    if ( (mdirFlag && sdirFlag && YFlag && XFlag) || (mdirFlag && YFlag) || (sdirFlag && XFlag) ){
        if(YFlag){
            Y = adapter(Y, (uint16_t)0, (uint16_t)4095, (uint16_t)0, (uint16_t)1023);
        }
        if(XFlag) {
            X = adapter(X, (uint16_t) 0, (uint16_t) 4095, (uint16_t) 0, (uint16_t) 511);
        }
        XOR = makeChecksum(Y, ((X<<1)|servoDirection));

        uint32_t fullMessage = 0x01;
        fullMessage = fullMessage << 1;
        fullMessage = fullMessage | motorDirection;
        fullMessage = fullMessage << 10;
        fullMessage = fullMessage | Y;
        fullMessage = fullMessage << 9;
        fullMessage = fullMessage | X;
        fullMessage = fullMessage << 1;
        fullMessage = fullMessage | servoDirection;
        fullMessage = fullMessage << 10;
        fullMessage = fullMessage | XOR;


        //use MSB / big endian to put the fullMessage into 4 uint8_t places of transmitData
        transmitData[0] = fullMessage >> 24;
        transmitData[1] = fullMessage >> 16;
        transmitData[2] = fullMessage >> 8;
        transmitData[3] = fullMessage;

        //delay the next message with 6ms to allow the i2c code to be send before the start of the next message
        transmitter.sendMessage(transmitData, 4, 1, 6);

        mdirFlag = false;
        sdirFlag = false;
        YFlag = false;
        XFlag = false;
    } else {
        // If no flags set, set out a keepalive signal to stop noise from accumulating
        transmitter.keepAlive();
    }
}