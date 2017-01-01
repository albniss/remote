#include "DimValue.h"

DimValue::DimValue(uint8_t val, const Command cmd) : 
	IParam(cmd)
{
	uint16_t rem1=0, rem2=0;
	
	val = cmd.Reflect(val);
	
	uint8_t original_msb3 = (cmd.bytes[22] & 0x07) >> 0;
	uint8_t original_lsb5 = (cmd.bytes[23] & 0xf8) >> 3;
	uint8_t original_val = (original_msb3 << 5) + (original_lsb5 << 0);
	
	//Update new CRC
	uint8_t xor_byte = val^original_val;
	//Calculate the diff message CRC
	rem1=crc.UpdateByte(0,xor_byte);
	for (int i=0;i<crccycles;i++)
		rem1=crc.UpdateBit(rem1,0);
	
	//Update the diff content
	cmd_diff.bytes[22] = (((val & 0xe0) >> 5) | 0x10) ^ cmd.bytes[22];
	cmd_diff.bytes[23] = (((val & 0x1f) << 3) | 0x05) ^ cmd.bytes[23];
	
	//Special case of 0:
	if (val==0)
	{
		//Calculate the message diff
		uint8_t xor_byte0=cmd.bytes[23] ^ cmd_diff.bytes[23] ^ 0x04;
		uint8_t xor_byte1=cmd.bytes[24] ^ cmd_diff.bytes[24] ^ 0x01;
		
		//Calculate the diff message CRC
		rem2=crc.UpdateByte(0,xor_byte1);
		rem2=crc.UpdateByte(rem2,xor_byte0);
		for (int i=0;i<crccycles_off;i++)
			rem2=crc.UpdateBit(rem2,0);
		
		//Update the diff content
		cmd_diff.bytes[23] ^= xor_byte0;
		cmd_diff.bytes[24] ^= xor_byte1;
	}
	
	//Update the diff crc
	cmd_diff.SetDiffCRC16(rem1 ^ rem2);	
}
