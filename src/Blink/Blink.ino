//http://arduino.cc/en/tutorial/blink
/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */

#include <avr/io.h>
#include "prescaler.h"

#include <EtherCard.h>

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = A5;

void setup() {
	setClockPrescaler(0x1);  //<8MHz for reliable 3.3V operation. >1MHz recommended.
	
	Serial.begin(115200);  // Maybe a lot faster over native USB.
	
	// initialize the digital pin as an output.
	pinMode(led, OUTPUT);   
}

void loop() {
	digitalWrite(led, HIGH);
	trueDelay(1000);
	Serial.println("HIGH");
	
	digitalWrite(led, LOW);
	trueDelay(1000);
	Serial.println("LOW");
}

