#ifndef RCCAR_PCA9685_HPP
#define RCCAR_PCA9685_HPP

#include <hwlib.hpp>

/**
 * \struct Registers. this struct exists of all the registers the PCA9685 needs for all of it's functions
 */
struct pca9685Registers {
    uint8_t MODE1;
    uint8_t MODE2;
    uint8_t SUBADR1;
    uint8_t SUBADR2;
    uint8_t SUBADR3;
    uint8_t ALLCALLADR;
    uint8_t LED0_ON_L;
    uint8_t LED0_ON_H;
    uint8_t LED0_OFF_L;
    uint8_t LED0_OFF_H;
    uint8_t ALLLED_ON_L;
    uint8_t ALLLED_ON_H;
    uint8_t ALLLED_OFF_L;
    uint8_t ALLLED_OFF_H;
    uint8_t PRESCALE;
    uint8_t TESTMODE;

    constexpr pca9685Registers(
        uint8_t MODE1           = 0x00,
        uint8_t MODE2           = 0x01,
        uint8_t SUBADR1         = 0x02,
        uint8_t SUBADR2         = 0x03,
        uint8_t SUBADR3         = 0x04,
        uint8_t ALLCALLADR      = 0x05,
        uint8_t LED0_ON_L       = 0x06,
        uint8_t LED0_ON_H       = 0x07,
        uint8_t LED0_OFF_L      = 0x08,
        uint8_t LED0_OFF_H      = 0x09,
        uint8_t ALLLED_ON_L     = 0xFA,
        uint8_t ALLLED_ON_H     = 0xFB,
        uint8_t ALLLED_OFF_L    = 0xFC,
        uint8_t ALLLED_OFF_H    = 0xFD,
        uint8_t PRESCALE        = 0xFE,
        uint8_t TESTMODE        = 0xFF
    ):
        MODE1(MODE1),
        MODE2(MODE2),
        SUBADR1(SUBADR1),
        SUBADR2(SUBADR2),
        SUBADR3(SUBADR3),
        ALLCALLADR(ALLCALLADR),
        LED0_ON_L(LED0_ON_L),
        LED0_ON_H(LED0_ON_H),
        LED0_OFF_L(LED0_OFF_L),
        LED0_OFF_H(LED0_OFF_H),
        ALLLED_ON_L(ALLLED_ON_L),
        ALLLED_ON_H(ALLLED_ON_H),
        ALLLED_OFF_L(ALLLED_OFF_L),
        ALLLED_OFF_H(ALLLED_OFF_H),
        PRESCALE(PRESCALE),
        TESTMODE(TESTMODE)
    {}
};

/**
 * \struct Bits. this struct exists of all the bits needed for the functions of the PCA9685 library
 */
struct pca9685Bits {
    uint8_t MODE1_ALLCAL;
    uint8_t MODE1_SUB3;
    uint8_t MODE1_SUB2;
    uint8_t MODE1_SUB1;
    uint8_t MODE1_SLEEP;
    uint8_t MODE1_AI;
    uint8_t MODE1_EXTCLK;
    uint8_t MODE1_RESTART;
    uint8_t MODE2_OUTNE_0;
    uint8_t MODE2_OUTNE_1;
    uint8_t MODE2_OUTDRV;
    uint8_t MODE2_OCH;
    uint8_t MODE2_INVRT;

    constexpr pca9685Bits(
        uint8_t MODE1_ALLCAL        = 0x01,
        uint8_t MODE1_SUB3          = 0x02,
        uint8_t MODE1_SUB2          = 0x04,
        uint8_t MODE1_SUB1          = 0x08,
        uint8_t MODE1_SLEEP         = 0x10,
        uint8_t MODE1_AI            = 0x20,
        uint8_t MODE1_EXTCLK        = 0x40,
        uint8_t MODE1_RESTART       = 0x80,
        uint8_t MODE2_OUTNE_0       = 0x01,
        uint8_t MODE2_OUTNE_1       = 0x02,
        uint8_t MODE2_OUTDRV        = 0x04,
        uint8_t MODE2_OCH           = 0x08,
        uint8_t MODE2_INVRT         = 0x10
    ):
        MODE1_ALLCAL(MODE1_ALLCAL),
        MODE1_SUB3(MODE1_SUB3),
        MODE1_SUB2(MODE1_SUB2),
        MODE1_SUB1(MODE1_SUB1),
        MODE1_SLEEP(MODE1_SLEEP),
        MODE1_AI(MODE1_AI),
        MODE1_EXTCLK(MODE1_EXTCLK),
        MODE1_RESTART(MODE1_RESTART),
        MODE2_OUTNE_0(MODE2_OUTNE_0),
        MODE2_OUTNE_1(MODE2_OUTNE_1),
        MODE2_OUTDRV(MODE2_OUTDRV),
        MODE2_OCH(MODE2_OCH),
        MODE2_INVRT(MODE2_INVRT)
    {}
};


// ==========================================================================
//
// PCA9685, accessed by i2c
//
// ==========================================================================
/**
 * \class this class implements most of the functions described by the datasheet of the PCA9685
 */
class PCA9685_i2c {
protected:

    /// the i2c channel
    hwlib::i2c_bus & bus;    /**< PCA9685 works with hwlib its build in i2c bus */
    uint_fast8_t address;    /**< address of the PCA9685. This can be selected by the pads on the board */

    const pca9685Registers registers;   /**< implement all the necessary registers from the struct */
    const pca9685Bits bits;             /**< implement all the necessary bits from the struct */

    uint_fast32_t oscillator_freq;      /**< the frequency the PCA9685 refreshes it's pins at */

    uint8_t PRESCALE_MIN = 3;   /**< minimum prescale value */
    uint8_t PRESCALE_MAX = 255; /**< maximum prescale value */

    /**
     * \brief protected function to stop unauthorised reads from happening
     * this function reads a whole byte at once
     * @param reg register to read the byte from
     */
    uint8_t readByte(uint8_t reg);

    /**
     * \brief protected function to stop unauthorised writes from happening
     * this function writes a whole byte at once
     * @param reg register to write to
     * @param d data to write to the register
     */
    void writeByte(uint8_t reg, uint8_t d);

public:

    /**
     * \brief Standard constructor
     *
     * @param bus reference to an hwlib i2c bus to use for it's protocol
     * @param address provide the chip its address to the class
     * the constructor also creates the registers and bits structs with it's default values
     * the oscillator frequency is set to a default value because it cannot stay empty
     */
    constexpr PCA9685_i2c( hwlib::i2c_bus & bus, uint_fast8_t address = 0x40):
            bus( bus ),
            address( address ),
            registers( pca9685Registers() ),
            bits( pca9685Bits() ),
            oscillator_freq( 25000000 )
    {
        // wait for the controller to be ready for the initialization
        hwlib::wait_ms( 20 );
    }

    /**
     * \brief function to initialize the class after construction
     * @param  prescale Sets External Clock (Optional)
     */
    void begin(uint8_t prescale = 0);

    /**
     * \brief  Sends a reset command to the PCA9685 chip over I2C
     */
    void reset();

    /**
     * \brief  Puts board into sleep mode
     */
    void sleep();

    /**
     * \brief  Wakes board from sleep
     */
    void wakeup();

    /**
     * \brief  Sets EXTCLK pin to use the external clock
     * @param  prescale Configures the prescale value to be used by the external clock
     */
    void setExtClk(uint8_t prescale);

    /**
     * \brief  Sets the PWM frequency for the entire chip, up to 1.6 KHz
     * @param  freq floating point frequency could be needed for precision tasks
     */
    void setPWMFreq(float freq);

    /**
     * \brief  Reads set Prescale from PCA9685
     * @return prescale value
     */
    uint8_t readPrescale();

    /**
     * \brief  Gets the PWM output of one of the PCA9685's pins
     * @param  num Pin of the PCA9685, from 0 to 15
     * @return requested PWM output value of the pin
     */
    uint8_t getPWM(uint8_t num);

    /**
     * \brief  Sets the PWM output of one of the PCA9685 pins
     * @param  num Pin of the PCA9685, from 0 to 15
     * @param  on At what point in the 4096-part cycle to turn the PWM output ON
     * @param  off At what point in the 4096-part cycle to turn the PWM output OFF
     */
    void setPWM(uint8_t num, uint16_t on, uint16_t off);

    /**
     * \brief function that allows the PWM of a pin to be set without needing to set
     * the 0 timing. It also can be used to invert the output.
     * @param  num Pin of the PCA9685, from 0 to 15
     * @param  val number of ticks out of 4096 to be active.
     * should be a value from 0 to 4095.
     * @param  invert If true, inverts the output, defaults to false
     */
    void setPin(uint8_t num, uint16_t val, bool invert = false);

    /**
     * \brief  Sets the PWM output of one of the PCA9685 pins based on the input
     * microseconds, useful for servo's
     * @param  num Pin of the PCA9685, from 0 to 15
     * @param  Microseconds The number of Microseconds to turn the PWM output ON
     */
    void writeMicroseconds(uint8_t num, uint16_t Microseconds);

    /**
     * \brief  Getter for the internal oscillator used for freq calculations
     * @returns The frequency the PCA9685 is using
     */
    uint32_t getOscillatorFrequency() const;

    /**
     * \brief Setter for the internal oscillator used for freq calculations
     * @param freq The frequency the PCA9685 should use
     */
    void setOscillatorFrequency(uint32_t freq);
};


/**
 * \class this class describes an hobby servo and makes it work with the PCA9685 library
 */
class servo {
private:
    PCA9685_i2c & PCA;
    const uint16_t Pin;
    const int16_t rangeMin;
    const int16_t rangeMax;
    uint16_t USMIN;
    uint16_t USMAX;

public:
    /**
     * \brief servo Constructor
     *
     * @param pca reference to existing PCA child to inherrit it's functions
     * @param pin number of the servo pin on the PCA board
     * @param rangeMin minimum analog input value to use with map function
     * @param rangeMax maximum analog input value to use with map function
     * @param USMIN minimum pulse duration for full motion of standard servo. Usually (and default) 500us
     * @param USMAX maximum pulse duration for full motion of standard servo. Usually (and default) 2500us
     */
    servo( PCA9685_i2c & pca, const uint16_t pin, const uint16_t & rangeMin, const uint16_t & rangeMax, const uint16_t & USMIN = 500, const uint16_t & USMAX = 2500):
            PCA( pca ),
            Pin( pin ),
            rangeMin( rangeMin ),
            rangeMax( rangeMax ),
            USMIN( USMIN ),
            USMAX( USMAX )
    {}

    /**
     * \brief function to set position based on analog value remapped to microseconds
     *
     * @param pos the position in microseconds
     */
    void setPosition( uint16_t pos ){
        PCA.writeMicroseconds( Pin, pos );
    }

    /**
     * \brief function that maps the given value to the previously defined usmin/usmax range
     *
     * @param val input value to be mapped
     */
    int16_t map(const int_fast16_t val) const {
        return USMIN + (float)(USMAX - USMIN) * ((float)(val - rangeMin) / (float)(rangeMax - rangeMin));
    }

    /**
     * \brief function that inverts the map function and returns the output of the map
     *
     * @param val input value to be mapped
     */
    int16_t mapInverse(const int_fast16_t val) {
        uint16_t temp = USMAX;
        USMAX = USMIN;
        USMIN = temp;
        int16_t returnValue = map(val);
        USMIN = USMAX;
        USMAX = temp;
        return returnValue;
    }

};

#endif //RCCAR_PCA9685_HPP