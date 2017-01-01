#include "Increment.h"

Increment::Increment(int count, const Command cmd) : 
	IParam(cmd), 
	increment_values{ 0x76, 0x74, 0x72 }
{
	uint8_t increment_value = increment_values[count % sizeof(increment_values)];
	
	//Calculate the message diff
	uint8_t xor_byte = increment_value ^ cmd.bytes[2];
	
	//Calculate the diff message CRC
	uint16_t rem=crc.UpdateByte(0,xor_byte);
	for (int i=0;i<crccycles;i++)
		rem=crc.UpdateBit(rem,0);

	//Update the diff content
	cmd_diff.bytes[2]=xor_byte;
	//Update the diff crc
	cmd_diff.SetDiffCRC16(rem);	
}