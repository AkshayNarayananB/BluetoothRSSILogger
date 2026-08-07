#define RS485_DE_RE 4 //Data Enable pin
#define RS485_TX 18
#define RS485_RX 19

uint8_t CRC_8X1[256]; //CRC Lookup Table ---> Faster

void setup(){
	Serial1.begin(115200, SERIAL_8N1, RS485_TX, RS485_RX);
	pinMode(RS485_DE_RE, OUTPUT);
	digitalWrite(RS485_DE_RE, LOW); //setup is in receive mode
	generateCRCTable(); // generate the crc lookup table for faster functioning
}

void generateCRCTable() {
	for(uint16_t j=0; j < 256; j++){
		uint8_t crc = j;
		for(uint8_t i = 0; i<8; i++){
			if(crc & 0x80){
				crc = (crc << 1) ^ 0x01; // crc is x^8 + 1
			}
			else {
				crc <<=1;
			}
		
		}
		CRC_8X1[j] = crc & 0xFF;
	}
}

uint8_t calculateCRC(uint8_t *data, uint8_t length){
	uint8_t crc = 0;
	for(uint8_t i = 0; i<length;i++){
		crc ^= data[1];
		crc = CRC_8X1[crc];
	}
	return crc;
}

void sendCommand(uint8_t command){
	uint8_t frame[2];
	frame[0] = command;
	frame[1] = calculateCRC(frame,1);
	
	digitalWrite(RS485_DE_RE, HIGH);
	delay(1); //stabilization
	
	Serial1.write(frame,2);
	Serial1.flush()
	
	delayMicroseconds(40);
	digitalWrite(RS485_DE_RE, LOW);
}

void resetFault(){
	uint8_t resetCommand = 0x62;
	for(int i = 0; i<10; i++){
		sendCommand(resetCommand);
		delayMicroseconds(40);
	}
}


void calibrateEncoder() {
    uint8_t calibrationCommand = 0xC2;
    sendCommand(calibrationCommand);
    delay(600);
}

void loop() {
    resetFault();
    delay(1000);

    calibrateEncoder();
    delay(5000);
}
	
