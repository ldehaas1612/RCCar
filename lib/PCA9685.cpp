/*
 *
 * Copyright Luc de Haas
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * https://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "PCA9685.hpp"

/**
 *  \brief  initializes the parameters for the hardware
 *  @param  prescale Sets External Clock (Optional)
 */
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

/**
 *  \brief  Sends a reset command to the PCA9685 chip over I2C
 */
void PCA9685_i2c::reset() {
    writeByte((uint8_t) pca9685_registers::PCA9685_MODE1, (uint8_t) pca9685_bits::MODE1_RESTART);
    hwlib::wait_ms(10);
}

/**
 *  \brief  Puts board into sleep mode
 */
void PCA9685_i2c::sleep() {
    uint8_t awake = readByte((uint8_t) pca9685_registers::PCA9685_MODE1);
    uint8_t sleep = awake | (uint8_t)pca9685_bits::MODE1_SLEEP; // set sleep bit high
    writeByte((uint8_t) pca9685_registers::PCA9685_MODE1, sleep);
    hwlib::wait_ms(5); // wait until cycle ends for sleep to be active
}

/**
 *  \brief  Wakes board from sleep
 */
void PCA9685_i2c::wakeup() {
    uint8_t sleep = readByte((uint8_t) pca9685_registers::PCA9685_MODE1);
    uint8_t wakeup = sleep & ~(uint8_t) pca9685_bits::MODE1_SLEEP; // set sleep bit low
    writeByte((uint8_t) pca9685_registers::PCA9685_MODE1, wakeup);
}

/**
 *  \brief  Sets EXTCLK pin to use the external clock
 *  @param  prescale Configures the prescale value to be used by the external clock
 */
void PCA9685_i2c::setExtClk(uint8_t prescale) {
    uint8_t oldmode = readByte((uint8_t) pca9685_registers::PCA9685_MODE1);
    uint8_t newmode = (oldmode & ~(uint8_t) pca9685_bits::MODE1_RESTART) | (uint8_t) pca9685_bits::MODE1_SLEEP; // sleep
    writeByte((uint8_t) pca9685_registers::PCA9685_MODE1, newmode); // go to sleep, turn off internal oscillator

    // This sets both the SLEEP and EXTCLK bits of the MODE1 register to switch to
    // use the external clock.
    writeByte((uint8_t) pca9685_registers::PCA9685_MODE1, (newmode |= (uint8_t) pca9685_bits::MODE1_EXTCLK));

    writeByte((uint8_t) pca9685_registers::PCA9685_PRESCALE, prescale); // set the prescaler

    hwlib::wait_ms(5);
    // clear the SLEEP bit to start
    writeByte((uint8_t) pca9685_registers::PCA9685_MODE1,
    (newmode & ~(uint8_t) pca9685_bits::MODE1_SLEEP) | (uint8_t) pca9685_bits::MODE1_RESTART | (uint8_t) pca9685_bits::MODE1_AI);
}

/**
 *  \brief  Sets the PWM frequency for the entire chip, up to ~1.6 KHz
 *  @param  freq Floating point frequency that we will attempt to match
 */
void PCA9685_i2c::setPWMFreq(float freq) {
    // Range output modulation frequency is dependant on oscillator
    if (freq < 1)
        freq = 1;
    if (freq > 3500)
        freq = 3500; // Datasheet limit is 3052=50MHz/(4*4096)

    float prescaleval = ((oscillator_freq / (freq * 4096.0)) + 0.5) - 1;
    if (prescaleval < PCA9685_PRESCALE_MIN)
        prescaleval = PCA9685_PRESCALE_MIN;
    if (prescaleval > PCA9685_PRESCALE_MAX)
        prescaleval = PCA9685_PRESCALE_MAX;
    uint8_t prescale = (uint8_t)prescaleval;

    uint8_t oldmode = readByte((uint8_t) pca9685_registers::PCA9685_MODE1);
    uint8_t newmode = (oldmode & ~(uint8_t) pca9685_bits::MODE1_RESTART) | (uint8_t) pca9685_bits::MODE1_SLEEP; // sleep
    writeByte((uint8_t) pca9685_registers::PCA9685_MODE1, newmode);                             // go to sleep
    writeByte((uint8_t) pca9685_registers::PCA9685_PRESCALE, prescale); // set the prescaler
    writeByte((uint8_t) pca9685_registers::PCA9685_MODE1, oldmode);
    hwlib::wait_ms(5);
    // This sets the MODE1 register to turn on auto increment.
    writeByte((uint8_t) pca9685_registers::PCA9685_MODE1,
              oldmode | (uint8_t) pca9685_bits::MODE1_RESTART | (uint8_t) pca9685_bits::MODE1_AI);

}

/**
 *  \brief  Sets the output mode of the PCA9685 to either
 *  open drain or push pull / totempole.
 *  Warning: LEDs with integrated zener diodes should
 *  only be driven in open drain mode.
 *  @param  totempole Totempole if true, open drain if false.
 */
void PCA9685_i2c::setOutputMode(bool totempole) {
    uint8_t oldmode = readByte((uint8_t) pca9685_registers::PCA9685_MODE2);
    uint8_t newmode;
    if (totempole) {
        newmode = oldmode | (uint8_t) pca9685_bits::MODE2_OUTDRV;
    } else {
        newmode = oldmode & ~(uint8_t) pca9685_bits::MODE2_OUTDRV;
    }
    writeByte((uint8_t) pca9685_registers::PCA9685_MODE2, newmode);
}

/**
 *  \brief  Reads set Prescale from PCA9685
 *  @return prescale value
 */
uint8_t PCA9685_i2c::readPrescale(void) {
    return readByte((uint8_t) pca9685_registers::PCA9685_PRESCALE);
}

/**
 *  \brief  Gets the PWM output of one of the PCA9685 pins
 *  @param  num One of the PWM output pins, from 0 to 15
 *  @return requested PWM output value
 */
uint8_t PCA9685_i2c::getPWM(uint8_t num) {
    return readByte((uint8_t) pca9685_registers::PCA9685_LED0_ON_L + 4 * num);
}

/**
 *  \brief  Sets the PWM output of one of the PCA9685 pins
 *  @param  num One of the PWM output pins, from 0 to 15
 *  @param  on At what point in the 4096-part cycle to turn the PWM output ON
 *  @param  off At what point in the 4096-part cycle to turn the PWM output OFF
 */
void PCA9685_i2c::setPWM(uint8_t num, uint16_t on, uint16_t off) {
    uint8_t data [4] = { (uint8_t) on, (uint8_t) (on >> 8), (uint8_t) off, (uint8_t) (off >> 8) };
    auto i2c = bus.write( address );
    i2c.write( (uint8_t) pca9685_registers::PCA9685_LED0_ON_L + 4 * num );
    i2c.write( data, 4 );
}

/**
 *   \brief  Helper to set pin PWM output. Sets pin without having to deal with
 * on/off tick placement and properly handles a zero value as completely off and
 * 4095 as completely on.  Optional invert parameter supports inverting the
 * pulse for sinking to ground.
 *   @param  num One of the PWM output pins, from 0 to 15
 *   @param  val The number of ticks out of 4096 to be active, should be a value
 * from 0 to 4095 inclusive.
 *   @param  invert If true, inverts the output, defaults to 'false'
 */
void PCA9685_i2c::setPin(uint8_t num, uint16_t val, bool invert) {
    // Clamp value between 0 and 4095 inclusive.
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

/**
 *  \brief  Sets the PWM output of one of the PCA9685 pins based on the input
 * microseconds, output is not precise
 *  @param  num One of the PWM output pins, from 0 to 15
 *  @param  Microseconds The number of Microseconds to turn the PWM output ON
 */
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

/**
 *  \brief  Getter for the internally tracked oscillator used for freq
 * calculations
 *  @returns The frequency the PCA9685 thinks it is running at (it cannot
 * introspect)
 */
uint32_t PCA9685_i2c::getOscillatorFrequency() const {
    return oscillator_freq;
}

/**
 *  \brief Setter for the internally tracked oscillator used for freq
 * calculations
 *  @param freq The frequency the PCA9685 should use for frequency calculations
 */
void PCA9685_i2c::setOscillatorFrequency(uint32_t freq) {
    oscillator_freq = freq;
}

/******************* Low level I2C interface */
uint8_t PCA9685_i2c::readByte(uint8_t reg) {
    bus.write( address ).write(reg);
    return bus.read( address ).read_byte();
}

void PCA9685_i2c::writeByte(uint8_t reg, uint8_t d) {
    auto writable = bus.write( address );
    writable.write( reg );
    writable.write( d );
}