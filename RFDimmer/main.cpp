#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstdlib>

#include "CRC.h"
#include "Message.h"

int main()
{
	std::cout << "===============================================================================" << '\n';
	std::cout << "==========      Software para calculo de Lutron - 2016.12.30          =========" << '\n';
	std::cout << "===============================================================================" << '\n';
	std::cout << '\n';

	CRC crc(0xCA0F, 0x0000,0x0000);

	/*
	uint8_t temp[100];
	memset(temp, 0, sizeof(temp));
	temp[0] = 0xC0;
	crc.Calculate(temp, sizeof(temp));
	*/
	
	/*
	//Validador de incremento
	{
		uint16_t rem = 0;
		rem=crc.UpdateByte(rem, 0xFE);
		rem=crc.UpdateByte(rem, 0x01);
		//rem=crc.UpdateByte(rem, 0x0b);
		for (int i=1; i < 200; i++)
		{
			rem=crc.UpdateBit(rem,0);
			if (rem == 0xda06 )
				std::cout << "Achei! i=" << i << '\n';
		}
	}
	*/
	
	int count=0;
	for (int val=0x0;val<0xff;val+=0x40)
	{
		Command change_dim({ 0x2f, 0xcf, 0x76, 0x0d, 0x01, 0x42, 0xda, 0xd4, 0x6c, 0x01, 0x42, 0x4e, 0x10, 0x05, 0x01, 0x6e, 0x4c, 0x13, 0x44, 0xff, 0x01, 0x48, 0x17, 0x55, 0xff, 0x00, 0x40, 0x10, 0x04, 0x01, 0x73, 0xc7, 0x51 });
		uint8_t sn[2]={0x13,0x44};
		Serial s(sn,change_dim);
		Increment i(count,change_dim);
		DimValue dv(val, change_dim);
		std::cout << (change_dim ^ i.cmd_diff ^ dv.cmd_diff ^ s.cmd_diff).GetBytes() << '\n';

		uint8_t sn2[2]={0x11,0x84};
		Serial s2(sn2,change_dim);
		Increment i2(count,change_dim);
		DimValue dv2(val, change_dim);
		std::cout << (change_dim ^ i2.cmd_diff ^ dv2.cmd_diff ^ s2.cmd_diff).GetBytes() << '\n';	
		
		std::cout << '\n';
		
		count++;
	}
	
	
	/*
	Message m1(0xfe);
	std::cout << m1.GetBytes() << '\n';
	*/

	system("pause");
	return 0;
}

