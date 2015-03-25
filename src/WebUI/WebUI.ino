#include <avr/io.h>
#include "prescaler.h"

#include "SPI.h"
#include "UIPEthernet.h"
#include "WebServer.h"

// ethernet interface mac address, must be unique on the LAN
//Xen prefix 00:16:3E, random suffix.
static uint8_t mac[] = { 0x00, 0x16, 0x3E, 0x72, 0x6A, 0x48 };

/* CHANGE THIS TO MATCH YOUR HOST NETWORK. Most home networks are in
* the 192.168.0.XXX or 192.168.1.XXX subrange. Pick an address
* that's not in use and isn't going to be automatically allocated by
* DHCP from your router. */
static uint8_t ip[] = { 192, 168, 50, 123 };


/* This creates an instance of the webserver. By specifying a prefix
 * of "", all pages will be at the root of the server. */
#define PREFIX ""
WebServer webserver(PREFIX, 80);

/* commands are functions that get called by the webserver framework
 * they can read any posted data from client, and they output to the
 * server to send data back to the web browser. */
void helloCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
	/* this line sends the standard "we're all OK" headers back to the
		 browser */
	server.httpSuccess();

	/* if we're handling a GET or POST, we can output our data here.
		 For a HEAD request, we just stop after outputting headers. */
	if (type != WebServer::HEAD)
	{
		/* this defines some HTML text in read-only memory aka PROGMEM.
		 * This is needed to avoid having the string copied to our limited
		 * amount of RAM. */
		P(helloMsg) = "<h1>Hello, World!</h1>";

		/* this is a special form of print that outputs from PROGMEM */
		server.printP(helloMsg);
	}
}

void setup()
{
	setClockPrescaler(0x1);  //<8MHz for reliable 3.3V operation. >1MHz recommended.

	Serial.begin(115200);  // Maybe a lot faster over native USB.
	
	int led = A5;
	
	// initialize the digital pin as an output.
	pinMode(led, OUTPUT);
	digitalWrite(led, HIGH);
	trueDelay(100);
	digitalWrite(led, LOW);
	trueDelay(100);
	digitalWrite(led, HIGH);

	trueDelay(1500);
	
	
	/* initialize the Ethernet adapter */
	Ethernet.begin(mac, ip);

	/* setup our default command that will be run when the user accesses
	 * the root page on the server */
	webserver.setDefaultCommand(&helloCmd);

	/* run the same command if you try to load /index.html, a common
	 * default page name */
	webserver.addCommand("index.html", &helloCmd);

	/* start the webserver */
	webserver.begin();
}

void loop()
{
	char buff[64];
	int len = 64;

	/* process incoming connections one at a time forever */
	webserver.processConnection(buff, &len);
}