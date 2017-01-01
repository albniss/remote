#include "Serial.h"

Serial::Serial(uint8_t serial[2], const Command cmd) :
	IParam(cmd) 
{
	//Calculate the message diff
	uint8_t xor_byte0 = serial[0] ^ cmd.bytes[17];
	uint8_t xor_byte1 = serial[1] ^ cmd.bytes[18];
	
	//Calculate the diff message CRC
	uint16_t rem=crc.UpdateByte(0,xor_byte1);
	rem=crc.UpdateByte(rem,xor_byte0);
	for (int i=0;i<crccycles;i++)
		rem=crc.UpdateBit(rem,0);

	//Update the diff content
	cmd_diff.bytes[17]=xor_byte0;
	cmd_diff.bytes[18]=xor_byte1;
	//Update the diff crc
	cmd_diff.SetDiffCRC16(rem);	
}