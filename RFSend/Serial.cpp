#include "Serial.h"

#include <iostream>

Serial::Serial(uint8_t serial[4], const Command cmd) :
	IParam(cmd) 
{
	//Update the diff content
	cmd_diff.bytes[15]=serial[0] ^ cmd.bytes[15];
	cmd_diff.bytes[16]=serial[1] ^ cmd.bytes[16];
	cmd_diff.bytes[17]=serial[2] ^ cmd.bytes[17];
	cmd_diff.bytes[18]=serial[3] ^ cmd.bytes[18];
	
	//Update the diff crc
	if (serial[0]==0x6e && serial[1]==0x4c && serial[2]==0x13 && serial[3]==0x44)
		cmd_diff.SetDiffCRC16(0x0000);	
	else if (serial[0]==0x6e && serial[1]==0x4c && serial[2]==0x11 && serial[3]==0x84)
		cmd_diff.SetDiffCRC16(0xc599);
	else if (serial[0]==0x68 && serial[1]==0xd2 && serial[2]==0x52 && serial[3]==0xb4) //Bebê
		cmd_diff.SetDiffCRC16(0x9d90);
	else
	{
		std::cerr << "Serial invalido!\n";
		exit(-1);
	}
}