set(CMAKE_TOOLCHAIN_FILE cmake/ArduinoToolchain.cmake) # Arduino Toolchain

cmake_minimum_required (VERSION 2.8) # Required cmake version.
project (Display C CXX)

print_board_list()
print_programmer_list()

# Colorize GCC output. Taken from http://stackoverflow.com/questions/14399984/make-cmake-use-gccfilter . Public domain or fair use assumed.
set_directory_properties(PROPERTIES RULE_LAUNCH_COMPILE
   "${PROJECT_SOURCE_DIR}/gccfilter_wrap")

#Default CMAKE_GENERATOR should be the quite standard and ubiquitous Unix Makefiles.
if(NOT(DEFINED CMAKE_GENERATOR))
SET(CMAKE_GENERATOR "Unix Makefiles")
endif(NOT(DEFINED CMAKE_GENERATOR))

# Set default compiler flags for extra warnings in eclipse-friendly format.
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -fmessage-length=0")

# Set compiler flags used for debugging.
SET(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG -DVERBOSE")

# Set compiler flags for pedantic compilation.
SET (CMAKE_C_FLAGS_PEDANTIC "-g -pedantic")

#Allow eclipse to discover include directories.
SET(CMAKE_VERBOSE_MAKEFILE ON)

# Find dependencies.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules/")
#find_package (LibSndFile)

#Find Arduino Libraries. Note recursive libraries (eg. U8glib) will need to be declared as such here or in the firmware-associated CMakeLists.txt file.
link_directories(${CMAKE_SOURCE_DIR}/lib)

# Process CMakeLists files in these subdirectories.
add_subdirectory (include)
add_subdirectory (src)
