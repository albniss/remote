#ifndef DIMVALUE_H
#define DIMVALUE_H

#include <cstdint>
#include "Command.h"
#include "IParam.h"

class DimValue : public IParam
{
public:
	DimValue(uint8_t val, const Command cmd);
private:
	static constexpr int crccycles=40;
	static constexpr int crccycles_off=25;
};

#endif