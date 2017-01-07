#include <cstring>
#include <array>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "Command.h"

Command::Command()
{
	std::fill(bytes.begin(),bytes.end(),0);
}

Command::Command(uint8_t bytes[33])
{
	std::copy(bytes,bytes+33,this->bytes.begin());
}

Command::Command(std::array<uint8_t,33> bytes)
{
	this->bytes=bytes;
}

Command operator^(const Command& a, const Command& b)
{
	Command cmd;
	
	for (uint32_t i=0;i<cmd.bytes.size();i++)
		cmd.bytes[i]=a.bytes[i]^b.bytes[i];
	
	return std::move(cmd);
}

void Command::SetDiffCRC16(uint16_t newcrc)
{
	uint8_t b1 = Reflect((newcrc & 0xFF00) >> 8);
	uint8_t b2 = Reflect((newcrc & 0x00FF) >> 0);

	newcrc = ((uint16_t)b1 << 8) + b2;

	bytes[30] = (newcrc & 0xfe00) >> 9;
	bytes[31] = (newcrc & 0x0100) >> 1 | (newcrc & 0x00F8) >> 3;
	bytes[32] = (newcrc & 0x0007) << 5;
}

uint8_t Command::Reflect(uint8_t in) const
{
	uint8_t temp = 0;
	for (int i = 0; i < 8; i++)
	if (in & (1 << i))
		temp |= (1 << (7 - i));

	return temp;
}

std::string Command::GetBytes() const
{
	std::stringstream ss;

	for (auto b : bytes)
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)b;

	return ss.str();
}