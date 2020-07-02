#ifndef RCCAR_PCA9685_HPP
#define RCCAR_PCA9685_HPP

#include <hwlib.hpp>

enum class pca9685_registers : uint8_t {
    PCA9685_MODE1                         = 0x00,
    PCA9685_MODE2                         = 0x01,
    PCA9685_SUBADR1                       = 0x02,
    PCA9685_SUBADR2                       = 0x03,
    PCA9685_SUBADR3                       = 0x04,
    PCA9685_ALLCALLADR                    = 0x05,
    PCA9685_LED0_ON_L                     = 0x06,
    PCA9685_LED0_ON_H                     = 0x07,
    PCA9685_LED0_OFF_L                    = 0x08,
    PCA9685_LED0_OFF_H                    = 0x09,
    PCA9685_ALLLED_ON_L                   = 0xFA,
    PCA9685_ALLLED_ON_H                   = 0xFB,
    PCA9685_ALLLED_OFF_L                  = 0xFC,
    PCA9685_ALLLED_OFF_H                  = 0xFD,
    PCA9685_PRESCALE                      = 0xFE,
    PCA9685_TESTMODE                      = 0xFF
};

enum class pca9685_bits : uint8_t {
    MODE1_ALLCAL                          = 0x01,
    MODE1_SUB3                            = 0x02,
    MODE1_SUB2                            = 0x04,
    MODE1_SUB1                            = 0x08,
    MODE1_SLEEP                           = 0x10,
    MODE1_AI                              = 0x20,
    MODE1_EXTCLK                          = 0x40,
    MODE1_RESTART                         = 0x80,
    MODE2_OUTNE_0                         = 0x01,
    MODE2_OUTNE_1                         = 0x02,
    MODE2_OUTDRV                          = 0x04,
    MODE2_OCH                             = 0x08,
    MODE2_INVRT                           = 0x10
};

#define PCA9685_PRESCALE_MIN 3   /**< minimum prescale value */
#define PCA9685_PRESCALE_MAX 255 /**< maximum prescale value */


// ==========================================================================
//
// PCA9685, accessed by i2c
//
// ==========================================================================

class PCA9685_i2c {
protected:

    /// the i2c channel
    hwlib::i2c_bus & bus;
    uint_fast8_t address;
    uint_fast32_t oscillator_freq;

    uint8_t readByte(uint8_t reg);
    void writeByte(uint8_t reg, uint8_t d);

public:

    /// construct by providing the i2c channel
    PCA9685_i2c( hwlib::i2c_bus & bus, uint_fast8_t address = 0x40 ):
            bus( bus ),
            address( address ),
            oscillator_freq( 25000000 )
    {
        // wait for the controller to be ready for the initialization
        hwlib::wait_ms( 20 );
    }

    void begin(uint8_t prescale = 0);
    void reset();
    void sleep();
    void wakeup();
    void setExtClk(uint8_t prescale);
    void setPWMFreq(float freq);
    void setOutputMode(bool totempole);
    uint8_t getPWM(uint8_t num);
    void setPWM(uint8_t num, uint16_t on, uint16_t off);
    void setPin(uint8_t num, uint16_t val, bool invert = false);
    uint8_t readPrescale();
    void writeMicroseconds(uint8_t num, uint16_t Microseconds);

    void setOscillatorFrequency(uint32_t freq);
    uint32_t getOscillatorFrequency() const;


};



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