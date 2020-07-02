#include "hwlib.hpp"
#include "PCA9685.hpp"
#include "motorController.hpp"
#include "Receiver433mhz.hpp"

int main() {

    // wait for the PC console to start
    hwlib::wait_ms(500);

    namespace target = hwlib::target;

    auto receiverPin = target::pin_in(target::pins::d2);
    auto receiver = Receiver433mhz(receiverPin);
    auto message = messageReceiver(receiver);

    auto scl = target::pin_oc(target::pins::scl);
    auto sda = target::pin_oc(target::pins::sda);
    auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);
    auto PCA = PCA9685_i2c(i2c_bus);

    //  Initialize the PCA9685 with the values for this project
    PCA.begin();
    PCA.setOscillatorFrequency(27000000);
    PCA.setPWMFreq(50);
    hwlib::wait_ms(10);

    //Servo min and max values;
    uint16_t USMIN = 500;
    uint16_t USMAX = 2500;
    int16_t rangeMin = -512;
    int16_t rangeMax = 511;

    const uint8_t FORWARDDIRPIN    = 3;
    const uint8_t BACKWARDDIRPIN    = 2;
    //const uint8_t DIRPIN    = 2;
    const uint8_t PWMPIN    = 1;
    const uint8_t SERVOPIN  = 0;

    // Motordriver controller
    //genericDriver motor( PCA, PWMPIN, DIRPIN);
    IBT_2 motor( PCA, PWMPIN, FORWARDDIRPIN, BACKWARDDIRPIN);

    // Servodriver controller
    servo ser( PCA, SERVOPIN, rangeMin, rangeMax, USMIN, USMAX);


    volatile bool _true = true;
    while (_true) {

        message.checkLoop();

        if (message.msgAvailable()){

            motor.setDirection(message.getMotorDir());
            motor.setSpeed(message.getY()*4);

            ser.setPosition(ser.mapInverse(message.getX() * (message.getServoDir() == 0 ? -1 : 1)));
        }
    }
}