include_directories(${CMAKE_SOURCE_DIR}/include)

#Recursive libraries must be declared.
set(EtherCard_RECURSE True)
set(UIPEthernet_RECURSE True)
set(WebServer_RECURSE True)
set(EEPROMex_RECURSE True)
set(EDB_RECURSE True)


set(ARDUINO_DEFAULT_BOARD leonardo) # Default Board ID, when not specified
set(ARDUINO_DEFAULT_PORT /dev/ttyACM0) # Default Port, when not specified

#generate_arduino_firmware(WebUI
#    SKETCH WebUI
#    PROGRAMMER avrispmkii
#    PORT /dev/ttyACM0
#    SERIAL picocom @SERIAL_PORT@ -b 115200 -l)

#generate_arduino_firmware(RFID
#    SKETCH RFID
#    PROGRAMMER avrispmkii
#    PORT /dev/ttyACM0
#    SERIAL picocom @SERIAL_PORT@ -b 115200 -l)

generate_arduino_firmware(CLI
    SKETCH CLI
    PROGRAMMER avrispmkii
    PORT /dev/ttyACM0
    SERIAL picocom -c  --imap "lfcrlf" @SERIAL_PORT@ -b 115200 -l)