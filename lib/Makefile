#############################################################################
#
# Project Makefile
#
# (c) Wouter van Ooijen (www.voti.nl) 2016
#
# This file is in the public domain.
# 
#############################################################################

# source files in this project (main.cpp is automatically assumed)
SOURCES := PCA9685.cpp joystick.cpp Transmit433mhzController.cpp Receiver433mhz.cpp

# header files in this project
HEADERS := PCA9685.hpp inputController.hpp joystick.hpp Transmit433mhzController.hpp Receiver433mhz.hpp motorController.hpp MovingAverage.hpp

# set RELATIVE to the next higher directory 
# and defer to the appropriate Makefile.* there
RELATIVE := ..
include $(RELATIVE)/Makefile.due
