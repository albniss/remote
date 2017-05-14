#ifndef SERIAL_H
#define SERIAL_H

#include <cstdint>
#include "Command.h"
#include "IParam.h"

class Serial : public IParam
{
public:
	Serial(uint8_t serial[4], const Command cmd);
private:
	static constexpr int crccycles=67-16;
};

#endif