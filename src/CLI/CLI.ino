#include "Arduino.h"
#include <avr/io.h>
#include "prescaler.h"

#include "SPI.h"
#include "UIPEthernet.h"

#include <EEPROM.h>
//#include "EEPROMex.h"
#include "EDB.h"

//RFID membership database parameters.
#define memberDB_TableSize 480	//60 8-byte entries
struct hacdcMember {
  char shortName[4];
  int tagID;
  uint8_t enabled;	//Binary, true/false, 1/0.
} hacdcMember;

// The read and write handlers for using the EEPROM Library
void writer(unsigned long address, byte data)
{
  EEPROM.write(address, data);
}

byte reader(unsigned long address)
{
  return EEPROM.read(address);
}

// Create an EDB object with the appropriate write and read handlers
EDB memberDB(&writer, &reader);

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
			"formatMembers\n"
			"countMembers\n"
			"showMembers\n"
			"addMember <shortName> <tagID> <1/0>\n"
			"delMember <recno>\n"
			"enableMember <recno>\n"
			"disableMember <recno>\n"
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
	
	if (strncmp("formatMembers", line, 13) == 0)
		memberDB.create(0, memberDB_TableSize, sizeof(hacdcMember));
	
	if (strncmp("countMembers", line, 12) == 0) {
		Serial.print( "\n" );
		Serial.print(memberDB.count());
		Serial.print( "\n" );
	}
	
	if (strncmp("showMembers", line, 11) == 0) {
		struct hacdcMember hacdcMemberInProgress;
		
		
		for (int i = 1; i <= memberDB.count(); i++)
		{
			memberDB.readRec(i, EDB_REC hacdcMemberInProgress);
			
			Serial.print("addMember	");
			Serial.print("	");
			for (int i=0; i < 4; i++)
				Serial.print(hacdcMemberInProgress.shortName[i]);
			Serial.print("	");
			Serial.print(hacdcMemberInProgress.tagID);
			Serial.print("	");
			Serial.print(hacdcMemberInProgress.enabled);
			Serial.print("	#");
			Serial.print(i);
			Serial.print("\n");
		}
	}
	
	if (strncmp("addMember", line, 9) == 0) {
		char *token;
		strtok(line, "	");	//Discard known command token.
		
		struct hacdcMember hacdcMemberInProgress;
		
		strncpy(hacdcMemberInProgress.shortName, strtok(NULL, "	"), 4);
		hacdcMemberInProgress.tagID = atoi((strtok(NULL, "	")));
		hacdcMemberInProgress.enabled = atoi((strtok(NULL, "	")));
		
		memberDB.appendRec(EDB_REC hacdcMemberInProgress);
	}
	
	if (strncmp("delMember", line, 9) == 0) {
		char *token;
		strtok(line, "	");	//Discard known command token.
		
		memberDB.deleteRec(atoi((strtok(NULL, "	"))));
		
	}
	
	if (strncmp("enableMember", line, 9) == 0) {
		char *token;
		strtok(line, "	");	//Discard known command token.
		
		struct hacdcMember hacdcMemberInProgress;
		
		int recno=atoi(strtok(NULL, "	"));
		
		memberDB.readRec(recno, EDB_REC hacdcMemberInProgress);
		hacdcMemberInProgress.enabled = 1;
		memberDB.updateRec(recno,EDB_REC hacdcMemberInProgress);
	}
	
	if (strncmp("disableMember", line, 9) == 0) {
		char *token;
		strtok(line, "	");	//Discard known command token.
		
		struct hacdcMember hacdcMemberInProgress;
		
		int recno=atoi(strtok(NULL, "	"));
		
		memberDB.readRec(recno, EDB_REC hacdcMemberInProgress);
		hacdcMemberInProgress.enabled = 0;
		memberDB.updateRec(recno,EDB_REC hacdcMemberInProgress);
	}
	
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
	
	memberDB.open(0);
	
	//LED startup indicator, and delay for host serial console.
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