#ifndef INCREMENT_H
#define INCREMENT_H

#include <cstdint>
#include "Command.h"
#include "IParam.h"

class Increment : public IParam
{
public:
	Increment(int count, const Command cmd);
private:
	static constexpr int crccycles=163;
	uint8_t increment_values[3];
};

#endif