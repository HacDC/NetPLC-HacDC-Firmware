#include "Arduino.h"
#include <avr/io.h>
#include "prescaler.h"

#include "SPI.h"
#include "UIPEthernet.h"

#include <EEPROM.h>
//#include "EEPROMex.h"
#include "EDB.h"

//Hash summary function to meaningfully reduce ID tag length.
uint16_t pearsonHash(int input[], uint8_t inputLen) {
	// 256 values 0-255 in any (random) order suffices
	PROGMEM const unsigned char permutation[256] = {
	98,  6, 85,150, 36, 23,112,164,135,207,169,  5, 26, 64,165,219, //  1
	61, 20, 68, 89,130, 63, 52,102, 24,229,132,245, 80,216,195,115, //  2
	90,168,156,203,177,120,  2,190,188,  7,100,185,174,243,162, 10, //  3
	237, 18,253,225,  8,208,172,244,255,126,101, 79,145,235,228,121, //  4
	123,251, 67,250,161,  0,107, 97,241,111,181, 82,249, 33, 69, 55, //  5
	59,153, 29,  9,213,167, 84, 93, 30, 46, 94, 75,151,114, 73,222, //  6
	197, 96,210, 45, 16,227,248,202, 51,152,252,125, 81,206,215,186, //  7
	39,158,178,187,131,136,  1, 49, 50, 17,141, 91, 47,129, 60, 99, //  8
	154, 35, 86,171,105, 34, 38,200,147, 58, 77,118,173,246, 76,254, //  9
	133,232,196,144,198,124, 53,  4,108, 74,223,234,134,230,157,139, // 10
	189,205,199,128,176, 19,211,236,127,192,231, 70,233, 88,146, 44, // 11
	183,201, 22, 83, 13,214,116,109,159, 32, 95,226,140,220, 57, 12, // 12
	221, 31,209,182,143, 92,149,184,148, 62,113, 65, 37, 27,106,166, // 13
	3, 14,204, 72, 21, 41, 56, 66, 28,193, 40,217, 25, 54,179,117, // 14
	238, 87,240,155,180,170,242,212,191,163, 78,218,137,194,175,110, // 15
	43,119,224, 71,122,142, 42,160,104, 48,247,103, 15, 11,138,239  // 16
	};
	
	uint8_t i;
	uint8_t initializationVector;
	initializationVector = 0;
	uint16_t hashOne = permutation[(input[0] + initializationVector) % 256];

	for (i = 0; i < inputLen; i++) {
		hashOne = permutation[hashOne ^ input[i]];
	}

	initializationVector = 1;
	uint16_t hashTwo = permutation[(input[0] + initializationVector) % 256];

	for (i = 0; i < inputLen; i++) {
		hashTwo = permutation[hashTwo ^ input[i]];
	}
	
	return hashOne + (hashTwo << 8);
}

//RFID membership database parameters.
#define memberDB_TableSize 480	//60 8-byte entries
struct hacdcMember {
  char shortName[4];
  uint16_t tagID;
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
			"IP address: "
			"N/A");
		/*
		for (byte thisByte = 0; thisByte < 4; thisByte++) {
			// print the value of each byte of the IP address:
			Serial.print(Ethernet.localIP()[thisByte], DEC);
			Serial.print(".");
		}
		*/
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
	
	Serial1.begin(9600);
	
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
	
}

void loop() {
	size_t size;
	char* completeLine;
	
	//Serial CLI. May conflict with network CLI.
	//Serial port use may reset platform, and data may be interleaved if both serial and ethernet CLI are used simultaneously.
	while (Serial.available() > 0)
		if ( (completeLine = bufferLine(Serial.read())) != NULL)
				processLineSerial(completeLine);
	
	//RFID
	if (Serial1.available() > 0) {
		delay(100); // needed to allow time for the data to come in from the serial
		
		//14 characters. Characters 1 and 14 are start/stop codes. Characters 12 and 13 are checksums.
		int readTag[14];
		
		if (Serial1.read() == '3') {
			readTag[13] = '3';
			for (int i = (13 - 1) ; i >= 0 ; i--) { // read the rest of the tag
				readTag[i] = Serial1.read();
			}
		}
		Serial1.flush(); // stops multiple reads... may also frustrate brute-force attacks
		
		uint16_t hashTag = pearsonHash(readTag, 14);
		
		//If RFID code ends with valid character.
		if (readTag[0] == '3')
			Serial.println(hashTag);
		
	}
	
	//Serial.println();
	
}