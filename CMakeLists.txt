# Copy this CMakeLists.txt file to your project folder
project(RCCar)
cmake_minimum_required(VERSION 3.10)

set(HCT $ENV{HU_ROOT_DIR}/HCT)
include(${HCT}/HCT.cmake)
hct_autoload(${CMAKE_CURRENT_SOURCE_DIR})
