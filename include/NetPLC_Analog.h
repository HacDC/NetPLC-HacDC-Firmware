//Analog
#define lowPass(newFloat, returnFloat, oldFloat, inertiaFloat) {\
returnFloat = oldFloat + (inertiaFloat * (newFloat - oldFloat));\
oldFloat = returnFloat;\
}
PROGMEM const float IIRinertia = 0.001;

#define Ex1_A1_raw 6
#define Ex1_A2_raw 12
#define Ex1_A3_raw 8
#define Ex1_A4_raw 4

#define Ex2_P1_raw A2
#define Ex2_P2_raw A1

#define Ex3_P1_raw A4
#define Ex3_P2_raw A3

void configSense(int pin) {
	pinMode(pin, INPUT);
	digitalWrite(pin, HIGH);
}

void configProtected(int pin) {
	//Fault-resistant short to ground.
	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);
}

#define samplePinFunction(functionName, rawPin) \
float functionName() {\
	configSense(rawPin);\
	\
	static float oldVal = 0;\
	static float filteredVal = 0;\
	lowPass((float)(analogRead(rawPin)), filteredVal, oldVal, IIRinertia);\
	\
	configProtected(rawPin);\
	\
	return filteredVal;\
}\

samplePinFunction(sample_Ex1_A1, Ex1_A1_raw);
samplePinFunction(sample_Ex1_A2, Ex1_A2_raw);
samplePinFunction(sample_Ex1_A3, Ex1_A3_raw);
samplePinFunction(sample_Ex1_A4, Ex1_A4_raw);

samplePinFunction(sample_Ex2_P1, Ex2_P1_raw);
samplePinFunction(sample_Ex2_P2, Ex2_P2_raw);

samplePinFunction(sample_Ex3_P1, Ex3_P1_raw);
samplePinFunction(sample_Ex3_P2, Ex3_P2_raw);


void printAll() {
	Serial.print(F("Ex1_A1="));
	Serial.print(sample_Ex1_A1());
	Serial.print(";");
	
	Serial.print(F("Ex1_A2="));
	Serial.print(sample_Ex1_A2());
	Serial.print(";");
	
	Serial.print(F("Ex1_A3="));
	Serial.print(sample_Ex1_A3());
	Serial.print(";");
	
	Serial.print(F("Ex1_A4="));
	Serial.print(sample_Ex1_A4());
	Serial.print(";");
	
	Serial.print(F("Ex2_P1="));
	Serial.print(sample_Ex2_P1());
	Serial.print(";");
	
	Serial.print(F("Ex2_P2="));
	Serial.print(sample_Ex2_P2());
	Serial.print(";");
	
	Serial.print(F("Ex3_P1="));
	Serial.print(sample_Ex3_P1());
	Serial.print(";");
	
	Serial.print(F("Ex3_P2="));
	Serial.print(sample_Ex3_P2());
	Serial.print(";");
	
	Serial.print("\n");
}

void sampleAll() {
	sample_Ex1_A1();
	sample_Ex1_A2();
	sample_Ex1_A3();
	sample_Ex1_A4();
	
	sample_Ex2_P1();
	sample_Ex2_P2();
	
	sample_Ex3_P1();
	sample_Ex3_P2();
}