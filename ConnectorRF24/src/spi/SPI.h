/*
 * SPI.h
 *
 *  Created on: Mar 23, 2018
 *      Author: damian
 */

#ifndef SPI_SPI_H_
#define SPI_SPI_H_

#define USE_SOFTWARE_SPI 0

#include "../io/io.h"

class SPI {
public:
	static SPI *getInstance();
	uint8_t writeReadByte(uint8_t data);
	void writeByte(uint8_t data);
	void writeDataBuffer(uint8_t *dataBuferOut, uint8_t length);
	void writeReadDataBuffer(uint8_t *dataBufferIn, uint8_t *dataBufferOut, uint8_t length);

private:
	//Cnstructors
	SPI();	//Prevent consttuction but allow in getInstance
	SPI(const SPI&);	//Prevent construction by copying
	SPI& operator=(const SPI&);	//Prevent assigment
	~SPI();					//Prevent unwanted destruction

	void init();
	void initPins();

private:
	static SPI * m_sSpiInstance;
	static bool m_bIsCreated;
};

#endif /* SPI_SPI_H_ */
