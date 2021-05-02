#pragma once
#include <util/twi.h>
#include <avr/io.h>
#include <stdbool.h>

typedef enum {
	I2c_FREQ_100K,
	I2c_FREQ_250K,
	I2c_FREQ_400K
} I2cFreq;

void i2cInit(I2cFreq i2cFreq, bool enPup);

bool i2cMasterWrite(uint8_t slaveAddr, uint8_t *data, uint8_t size);

bool i2cMasterRead(uint8_t slaveAddr, uint8_t *data, uint8_t size);