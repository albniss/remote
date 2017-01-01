#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <cstdint>
#include <array>

class Command {
public:
	std::array<uint8_t,33> bytes;
	
	Command();
	Command(uint8_t bytes[33]);
	Command(std::array<uint8_t,33> bytes);
	friend Command operator^(const Command& a, const Command& b);

	void SetDiffCRC16(uint16_t newcrc);
	std::string GetBytes() const;
	uint8_t Reflect(uint8_t in) const;
};

#endif