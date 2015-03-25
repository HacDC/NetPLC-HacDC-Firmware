#include <avr/io.h>
#include "prescaler.h"

#include "SPI.h"
#include "UIPEthernet.h"

#include "EEPROMex.h"
#include "EDB.h"

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

void processLineEthernet(char line[], EthernetClient client) {
	if (strncmp("commands", line, 8) == 0)
		client.write( "\n"
			"commands\n"
			"readout\n"
			"exit\n"
		);
	
	if (strncmp("readout", line, 7) == 0)
		client.write( "\n"
			"analog stuff\n"
		);
	
	if (strncmp("exit", line, 4) == 0)
		client.stop();
}

void processLineSerial(char line[]) {
	if (strncmp("commands", line, 8) == 0)
		Serial.print( "\n"
			"commands\n"
			"shownet\n"
			"readout\n"
		);
	
	if (strncmp("shownet", line, 7) == 0) {
		Serial.print("\n"
			"IP address: ");
		for (byte thisByte = 0; thisByte < 4; thisByte++) {
			// print the value of each byte of the IP address:
			Serial.print(Ethernet.localIP()[thisByte], DEC);
			Serial.print(".");
		}
		Serial.println();
	}
	
	if (strncmp("readout", line, 7) == 0)
		Serial.print( "\n"
			"stuff\n"
		);
}

char* bufferLine(char newChar) {
	static char completeLine[264];
	static char line[256];
	static int pos=0;
	
	//Clear overloaded buffer.
	if (pos >= 256)
		pos=0;
	
	//Recognize end of command input.
	if (newChar == '\n' || newChar == '\r') {
		
		for(int i=0; i<pos; i++)
			completeLine[i] = line[i];
		completeLine[pos] = '\0';
		
		pos=0;
		return completeLine;
	}
	
	line[pos]=newChar;
	pos++;
	
	return NULL;
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
	
	
	Ethernet.begin(mac);
	
	// print your local IP address:
	Serial.print("IP address: ");
	for (byte thisByte = 0; thisByte < 4; thisByte++) {
		// print the value of each byte of the IP address:
		Serial.print(Ethernet.localIP()[thisByte], DEC);
		Serial.print(".");
	}
	Serial.println();

	server.begin();
}

void loop() {
	size_t size;
	char* completeLine;
	
	//Primary CLI interface. Use: nc <ip.ip.ip.ip> <port>
	if (EthernetClient client = server.available()) {
		while((client.available()) > 0)
			if ( (completeLine = bufferLine(client.read())) != NULL)
				processLineEthernet(completeLine, client);
	}
	
	//WARNING. Serial CLI is intended as recovery fallback only.
	//Serial port use may reset platform, and data may be interleaved if both serial and ethernet CLI are used simultaneously.
	while (Serial.available() > 0)
		if ( (completeLine = bufferLine(Serial.read())) != NULL)
				processLineSerial(completeLine);
	
	
}