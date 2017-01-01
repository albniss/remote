#include "Message.h"
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstring>
#include <string>
#include <sstream>

#include "CRC.h"
#include "Increment.h"
#include "Serial.h"
#include "DimValue.h"

std::string Message::SetVal(uint16_t serial, uint8_t val)
{
	/*
	 * Fluxo de operação:
	 * 1) Inicializa o parâmetro desejado passando a mensagem original
	 * 2) Pega o diffCRC e o diffMessage do parâmetro e aplica na mensagem original
	 * 3) Retorna a nova mensagem
	 */
	Increment i(counter++,cmd);
	DimValue dv(val, cmd);
	Serial sn(0,cmd);
	
	val = Reflect(val);

	uint8_t original_msb3 = (bytes_val[22] & 0x07) >> 0;
	uint8_t original_lsb5 = (bytes_val[23] & 0xf8) >> 3;
	uint8_t original_val = (original_msb3 << 5) + (original_lsb5 << 0);

	//Update new Val
	bytes_val[22] = ((val & 0xe0) >> 5) | 0x10;
	bytes_val[23] = ((val & 0x1f) << 3) | 0x05;

	//Update new CRC
	uint8_t xor_val = val^original_val;
	CRC crc(0xCA0F, 0x0000, 0x0000);
	uint8_t diff_message[6];
	memset(diff_message, 0, sizeof(diff_message));
	diff_message[0] = xor_val;

	uint16_t diff_crc = GetCRC16() ^ crc.Calculate(diff_message, sizeof(diff_message));
	
	UpdateCRC16(diff_crc);

	std::cout << "CRC: " << GetCRC16() << " Diff CRC: " << diff_crc << '\n';
	
	return GetBytes();
}

void Message::UpdateCRC16(uint16_t newcrc)
{
	uint8_t b1 = Reflect((newcrc & 0xFF00) >> 8);
	uint8_t b2 = Reflect((newcrc & 0x00FF) >> 0);

	newcrc = ((uint16_t)b1 << 8) + b2;

	bytes_val[30] = (newcrc & 0xfe00) >> 9 | 0x00;
	bytes_val[31] = (newcrc & 0x0100) >> 1 | (newcrc & 0x00F8) >> 3 | 0x40;
	bytes_val[32] = (newcrc & 0x0007) << 5 | 0x10;
}

std::string Message::GetBytes()
{
	std::stringstream ss=std::stringstream();

	for (uint32_t i = 0; i < sizeof(bytes_val); i++)
		ss << std::hex << std::setw(2) << std::setfill('0') << (int) bytes_val[i];

	return ss.str();
}

uint16_t Message::GetCRC16()
{
	uint8_t b1, b2;

	b1 = ((bytes_val[30] & 0x7f) << 1) + ((bytes_val[31] & 0x80) >> 7);
	b2 = ((bytes_val[31] & 0x1f) << 3) + ((bytes_val[32] & 0xe0) >> 5);

	return ((uint16_t)Reflect(b1) << 8) + Reflect(b2);
}

uint8_t Message::Reflect(uint8_t in)
{
	uint8_t temp = 0;
	for (int i = 0; i < 8; i++)
	if (in & (1 << i))
		temp |= (1 << (7 - i));

	return temp;
}