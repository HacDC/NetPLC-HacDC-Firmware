#!/bin/bash
#Simple interactive serial terminal script, with minimial dependencies. Intended for production use from a dedicated workstation.

modprobe cdc_acm

echo "0x2341 0x8036" > /sys/bus/usb/drivers/cdc_acm/new_id

if [[ -e /dev/ttyACM0 ]]
then
	echo "*****Welcome to NetPLC. Ctrl+C to exit."
else
	echo "!!!!!USB device not found."
	sleep 3
fi


stty -F /dev/ttyACM0 -inlcr -onlcr cs8 9600 ignbrk -brkint -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts

cat /dev/ttyACM0 &
replyHandler=$!
trap "kill $replyHandler ; exit" SIGINT SIGTERM

while read line; do echo "$line" > /dev/ttyACM0 ; done
