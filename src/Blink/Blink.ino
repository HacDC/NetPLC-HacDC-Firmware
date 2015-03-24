//http://arduino.cc/en/tutorial/blink
/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */

#include <avr/io.h>
#include "prescaler.h"

#include <EtherCard.h>

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
static byte myip[] = { 192,168,50,123 };

byte Ethernet::buffer[500];
BufferFiller bfill;

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = A5;


static word homePage() {
  long t = trueMillis() / 1000;
  word h = t / 3600;
  byte m = (t / 60) % 60;
  byte s = t % 60;
  bfill = ether.tcpOffset();
  bfill.emit_p(PSTR(
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Pragma: no-cache\r\n"
    "\r\n"
    "<meta http-equiv='refresh' content='1'/>"
    "<title>RBBB server</title>"
    "<h1>$D$D:$D$D:$D$D</h1>"),
      h/10, h%10, m/10, m%10, s/10, s%10);
  return bfill.position();
}

void setup() {
	setClockPrescaler(0x1);  //<8MHz for reliable 3.3V operation. >1MHz recommended.
	
	Serial.begin(115200);  // Maybe a lot faster over native USB.
	
	// initialize the digital pin as an output.
	pinMode(led, OUTPUT);
	digitalWrite(led, HIGH);
	trueDelay(100);
	digitalWrite(led, LOW);
	trueDelay(100);
	digitalWrite(led, HIGH);
	
	if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
		Serial.println( "Failed to access Ethernet controller");
	ether.staticSetup(myip);
}

void loop() {
	word len = ether.packetReceive();
	word pos = ether.packetLoop(len);

	if (pos)  // check if valid tcp data is received
		ether.httpServerReply(homePage()); // send web page data
}

