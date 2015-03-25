#include <avr/io.h>
#include "prescaler.h"

#include "SPI.h"
#include "UIPEthernet.h"

// ethernet interface mac address, must be unique on the LAN
//Xen prefix 00:16:3E, random suffix.
static uint8_t mac[] = { 0x00, 0x16, 0x3E, 0x72, 0x6A, 0x48 };

/* CHANGE THIS TO MATCH YOUR HOST NETWORK. Most home networks are in
* the 192.168.0.XXX or 192.168.1.XXX subrange. Pick an address
* that's not in use and isn't going to be automatically allocated by
* DHCP from your router. */
static uint8_t ip[] = { 192, 168, 50, 123 };

//Telnet style server, port 1000.
EthernetServer server = EthernetServer(1000);

void processLine(char line[]) {
	if (strncmp("commands", line, 8) == 0)
		server.write("commands");
}

void bufferLine(char newChar) {
	char completeLine[264];
	static char line[256];
	static int pos=0;
	
	//Clear overloaded buffer.
	if (pos >= 256)
		pos=0;
	
 	//Recognize end of command input.
	if (newChar == '\n') {
		
		for(int i=0; i<pos; i++)
			completeLine[i] = line[i];
		completeLine[pos] = '\0';
		
		processLine(completeLine);
		
		pos=0;
		return;
	}
	
	line[pos]=newChar;
	pos++;
}

void setup() {
	setClockPrescaler(0x1);	//<8MHz for reliable 3.3V operation. >1MHz recommended.

	Serial.begin(115200);	// Maybe a lot faster over native USB.
	
	int led = A5;
	
	// initialize the digital pin as an output.
	pinMode(led, OUTPUT);
	digitalWrite(led, HIGH);
	trueDelay(100);
	digitalWrite(led, LOW);
	trueDelay(100);
	digitalWrite(led, HIGH);

	trueDelay(1500);
	
	
	Ethernet.begin(mac,ip);

	server.begin();
}

void loop() {
	size_t size;
	
	if (EthernetClient client = server.available()) {
		while((client.available()) > 0)
			bufferLine(client.read());
		//client.stop();
	}
}