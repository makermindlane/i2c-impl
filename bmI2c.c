#include "bmI2c.h"


/*
	Function prototypes
*/
static bool i2cStart();
static bool i2cSlaveWrite(uint8_t slaveAddr);
static bool i2cSlaveRead(uint8_t slaveAddr);
static bool i2cSendData(uint8_t *data, uint8_t size);
static bool i2cReceiveData(uint8_t *data, uint8_t size);
//static bool i2cRestart();
static void i2cStop();


static bool i2cStart() {
	
	TWCR = (1 << TWEN) | (1 << TWSTA) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT)));
	if ((TWSR & 0xf8) != TW_START)
		return false;
		
	return true;
}


static bool i2cSlaveWrite(uint8_t slaveAddr) {
	
	if (!(TWCR & (1 << TWINT)))
		return false;
	TWDR = (slaveAddr << 1);
	TWCR = (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT)));
	if ((TWSR & 0xf8) != TW_MT_SLA_ACK)
		return false;
		
	return true;
}


static bool i2cSlaveRead(uint8_t slaveAddr) {
	
	if (!(TWCR & (1 << TWINT)))
		return false;
	TWDR = ((slaveAddr << 1) | 1);
	TWCR = (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT)));
	if ((TWSR & 0xf8) != TW_MR_SLA_ACK)
		return false;
		
	return true;
}


static bool i2cSendData(uint8_t *data, uint8_t size) {
	
	for (uint8_t i = 0; i < size; ++i) {
		if (!(TWCR & (1 << TWINT))) 
			return false;
		TWDR = data[i];
		TWCR = (1 << TWEN) | (1 << TWINT);
		while (!(TWCR & (1 << TWINT)));
		if ((TWSR & 0xf8) != TW_MT_DATA_ACK)
		return false;
	}
	return true;
}


static bool i2cReceiveData(uint8_t *data, uint8_t size) {
	
	for (uint8_t i = 0; i < size - 1; ++i) {
		if (!(TWCR & (1 << TWINT)))
			return false;
		TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
		while (!(TWCR & (1 << TWINT)));
		if ((TWSR & 0xf8) != TW_MR_DATA_ACK)
			return false;
		data[i] = TWDR;
	}
	TWCR = (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT)));
	if ((TWSR & 0xf8) != TW_MR_DATA_NACK)
		return false;
	data[size - 1] = TWDR;
	
	return true;
}


//static bool i2cRestart() {
	//return 0;
//}


static void i2cStop() {
	TWCR = (1 << TWEN) | (1 << TWSTO) | (1 << TWINT);
}


void i2cInit(I2cFreq i2cFreq, bool enPup) {
	
	if (enPup) {
		PORTC |= (1 << PORTC5) | (1 << PORTC4);
	}
	DDRC &= ~((1 << PORTC5) | (1 << PORTC4));
	
	switch (i2cFreq) {
		case I2c_FREQ_100K:
		/* Set bit rate register 72 and prescaler to 1 resulting in
		SCL_freq = 16MHz/(16 + 2*72*1) = 100KHz	*/
		TWBR = 72;
		break;
		
		case I2c_FREQ_250K:
		/* Set bit rate register 24 and prescaler to 1 resulting in
		SCL_freq = 16MHz/(16 + 2*24*1) = 250KHz	*/
		TWBR = 24;
		break;
		
		case I2c_FREQ_400K:
		/* Set bit rate register 12 and prescaler to 1 resulting in
		SCL_freq = 16MHz/(16 + 2*12*1) = 400KHz	*/
		TWBR = 12;
		break;
		
		default: break;
	}
	
}


bool i2cMasterWrite(uint8_t slaveAddr, uint8_t *data, uint8_t size) {
	
	if (i2cStart() != true) return false;
	if (i2cSlaveWrite(slaveAddr) != true) return false;
	if (i2cSendData(data, size) != true) return false;
	i2cStop();
	
	return true;
}


bool i2cMasterRead(uint8_t slaveAddr, uint8_t *data, uint8_t size) {
	
	if (i2cStart() != true) return false;
	if (i2cSlaveRead(slaveAddr) != true) return false;
	if (i2cReceiveData(data, size) != true) return false;
	i2cStop();
	
	return 0;
}
