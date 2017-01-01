#ifndef IPARAM_H
#define IPARAM_H

#include <cstdint>
#include "Command.h"
#include "CRC.h"

class IParam {
public:
	IParam(const Command cmd) : cmd(cmd),crc(CRC(0xCA0F, 0x0000, 0x0000)) {};
	Command cmd;
	Command cmd_diff;
protected:
	CRC crc;
private:

};

#endif