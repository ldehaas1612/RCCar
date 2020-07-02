/*
 *
 * Copyright Luc de Haas
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * https://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "PCA9685.hpp"

void PCA9685_i2c::begin(uint8_t prescale) {
    reset();
    if (prescale) {
        setExtClk(prescale);
    } else {
        // set a default frequency
        setPWMFreq(1000);
    }
    // set the default internal frequency
    setOscillatorFrequency(oscillator_freq);
}

void PCA9685_i2c::reset() {
    writeByte(registers.MODE1, bits.MODE1_RESTART);
    hwlib::wait_ms(10);
}

void PCA9685_i2c::sleep() {
    uint8_t awake = readByte(registers.MODE1);
    uint8_t sleep = awake | bits.MODE1_SLEEP; // set sleep bit high
    writeByte(registers.MODE1, sleep);
    hwlib::wait_ms(5); // wait until cycle ends for sleep to be active
}

void PCA9685_i2c::wakeup() {
    uint8_t sleep = readByte(registers.MODE1);
    uint8_t wakeup = sleep & ~bits.MODE1_SLEEP; // set sleep bit low
    writeByte(registers.MODE1, wakeup);
}

void PCA9685_i2c::setExtClk(uint8_t prescale) {
    uint8_t oldmode = readByte(registers.MODE1);
    uint8_t newmode = (oldmode & ~bits.MODE1_RESTART) | bits.MODE1_SLEEP; // sleep
    writeByte(registers.MODE1, newmode); // go to sleep, turn off internal oscillator

    // This sets both the SLEEP and EXTCLK bits of the MODE1 register to switch to
    // use the external clock.
    writeByte(registers.MODE1, (newmode |= bits.MODE1_EXTCLK));

    writeByte(registers.PRESCALE, prescale); // set the prescaler

    hwlib::wait_ms(5);
    // clear the SLEEP bit to start
    writeByte(registers.MODE1,(newmode & ~bits.MODE1_SLEEP) | bits.MODE1_RESTART | bits.MODE1_AI);
}

void PCA9685_i2c::setPWMFreq(float freq) {
    // Range output modulation frequency is dependant on oscillator
    if (freq < 1)
        freq = 1;
    if (freq > 3500)
        freq = 3500; // Datasheet limit is 3052=50MHz/(4*4096)

    float prescaleval = ((oscillator_freq / (freq * 4096.0)) + 0.5) - 1;
    if (prescaleval < PRESCALE_MIN)
        prescaleval = PRESCALE_MIN;
    if (prescaleval > PRESCALE_MAX)
        prescaleval = PRESCALE_MAX;
    uint8_t prescale = prescaleval;

    uint8_t oldmode = readByte(registers.MODE1);
    uint8_t newmode = (oldmode & ~bits.MODE1_RESTART) | bits.MODE1_SLEEP; // sleep
    writeByte(registers.MODE1, newmode);                             // go to sleep
    writeByte(registers.PRESCALE, prescale); // set the prescaler
    writeByte(registers.MODE1, oldmode);
    hwlib::wait_ms(5);
    // This sets the MODE1 register to turn on auto increment.
    writeByte(registers.MODE1,oldmode | bits.MODE1_RESTART | bits.MODE1_AI);

}

uint8_t PCA9685_i2c::readPrescale() {
    return readByte(registers.PRESCALE);
}

uint8_t PCA9685_i2c::getPWM(uint8_t num) {
    return readByte(registers.LED0_ON_L + 4 * num);
}

void PCA9685_i2c::setPWM(uint8_t num, uint16_t on, uint16_t off) {
    uint8_t data [4] = { (uint8_t) on, (uint8_t) (on >> 8), (uint8_t) off, (uint8_t) (off >> 8) };
    auto i2c = bus.write( address );
    i2c.write( registers.LED0_ON_L + 4 * num );
    i2c.write( data, 4 );
}

void PCA9685_i2c::setPin(uint8_t num, uint16_t val, bool invert) {
    // value between 0 and 4095 explicitly.
    val = (val > (uint16_t) 4095 ? (uint16_t) 4095 : val);
    if (invert) {
        if (val == 0) {
            // Special value for signal fully on.
            setPWM(num, 4096, 0);
        } else if (val == 4095) {
            // Special value for signal fully off.
            setPWM(num, 0, 4096);
        } else {
            setPWM(num, 0, 4095 - val);
        }
    } else {
        if (val == 4095) {
            // Special value for signal fully on.
            setPWM(num, 4096, 0);
        } else if (val == 0) {
            // Special value for signal fully off.
            setPWM(num, 0, 4096);
        } else {
            setPWM(num, 0, val);
        }
    }
}

void PCA9685_i2c::writeMicroseconds(uint8_t num, uint16_t Microseconds) {

    double pulse = Microseconds;
    double pulselength;
    pulselength = 1000000; // 1,000,000 us per second

    // Read prescale
    uint16_t prescale = readPrescale();

    // Calculate the pulse for PWM based on Equation 1 from the datasheet section
    // 7.3.5
    prescale += 1;
    pulselength *= prescale;
    pulselength /= oscillator_freq;

    pulse /= pulselength;

    setPWM(num, 0, pulse);
}

uint32_t PCA9685_i2c::getOscillatorFrequency() const {
    return oscillator_freq;
}

void PCA9685_i2c::setOscillatorFrequency(uint32_t freq) {
    oscillator_freq = freq;
}


uint8_t PCA9685_i2c::readByte(uint8_t reg) {
    bus.write( address ).write(reg);
    return bus.read( address ).read_byte();
}

void PCA9685_i2c::writeByte(uint8_t reg, uint8_t d) {
    auto writable = bus.write( address );
    writable.write( reg );
    writable.write( d );
}