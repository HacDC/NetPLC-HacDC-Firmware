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
//Xen prefix 00:16:3E, random suffix.
static byte mymac[] = { 0x00,0x16,0x3E,0x72,0x6A,0x48 };

byte Ethernet::buffer[1024];
BufferFiller bfill;

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = A5;


int xyz = 0;


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
    //"<meta http-equiv='refresh' content='1'/>"
    "<title>ENC28J60 Web Interface - HacDC NetPLC</title>"
    "<p>Uptime: $D$D:$D$D:$D$</p>"
    "<p>Ready to authorize: <input type='text' id='myText' value='anon'></p>"
    "<p>Internal LED: <form action='' method='get'><button name='LED' type='submit' value='ON'>ON</button><button name='LED' type='submit' value='OFF'>OFF</button></form></p>"
    ""),
      h/10, h%10, m/10, m%10, s/10, s%10);
  return bfill.position();
}

void processData(char* data) {
	
	if (strncmp("GET / xyz", data, 10) != 0)
		xyz = 1;
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
	
	trueDelay(1500);
	
	  Serial.println("\n[getDHCPandDNS]");
  
	if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
	Serial.println( "Failed to access Ethernet controller");

	if (!ether.dhcpSetup())
	Serial.println("DHCP failed");

	ether.printIp("My IP: ", ether.myip);
	// ether.printIp("Netmask: ", ether.mymask);
	ether.printIp("GW IP: ", ether.gwip);
	ether.printIp("DNS IP: ", ether.dnsip);
}

void loop() {
	word len = ether.packetReceive();
	word pos = ether.packetLoop(len);
	
	if (pos)  {// check if valid tcp data is received
		
		//http://192.168.50.118/?LED=OFF
		if (strstr((char *)Ethernet::buffer + pos, "GET /?LED=OFF") != 0) {
			digitalWrite(led, LOW);
		}
		else if (strstr((char *)Ethernet::buffer + pos, "GET /?LED=ON") != 0) {
			digitalWrite(led, HIGH);
		}
		
		Serial.println((char *)Ethernet::buffer + pos);
		
		ether.httpServerReply(homePage()); // send web page data
		
		
		
		
		
		
		
		
		
		
		
		
	}
}

