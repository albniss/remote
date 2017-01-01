#ifndef CRC_H
#define CRC_H

#include <cstdint>

class CRC
{
public:
	CRC(uint16_t poly, uint16_t xorin, uint16_t xorout) : poly(poly), xorin(xorin), xorout(xorout) {};
	uint16_t Calculate(uint8_t* buf, int size);
	uint16_t UpdateBit(uint16_t rem, bool bit);
	uint16_t UpdateByte(uint16_t rem, uint8_t byte);
private:
	bool reflectin;
	bool reflectout;
	uint16_t poly;
	uint16_t xorin;
	uint16_t xorout;
};

#endif