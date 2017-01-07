#include "CRC.h"
#include <iostream>
#include <iomanip>

uint16_t CRC::UpdateBit(uint16_t rem, bool bit)
{
	bool topbit = (rem & 0x8000) != 0;

	rem = rem << 1;

	if (bit)
		rem ^= 1;

	if (topbit)
		rem ^= poly;

	return rem;
}

uint16_t CRC::UpdateByte(uint16_t rem, uint8_t byte)
{
	for (int i = 0; i < 8; i++)
	{
		bool topbit = (rem & 0x8000) != 0;

		rem = rem << 1;

		uint8_t in = byte;
		if (in & (1 << (i % 8)))
			rem ^= 1;

		if (topbit)
			rem ^= poly;
	}
	return rem;
}

uint16_t CRC::Calculate(uint8_t* buf, int size)
{
	uint16_t rem = xorin;

	for (int i = 0; i < 8 * size; i++)
	{
		bool topbit = (rem & 0x8000) != 0;

		rem = rem << 1;
		
		uint8_t in = buf[i / 8];
		if (in & (1 << (i % 8)))
			rem ^= 1;

		if (topbit)
			rem ^= poly;

		std::cout << "REM: " << std::hex << std::setw(4) << std::setfill('0') << rem << '\n';
	}

	std::cout << "FIM: " << std::hex << std::setw(4) << std::setfill('0') << rem << '\n';
	
	return rem ^ xorout;
}
