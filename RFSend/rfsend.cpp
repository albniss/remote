#include <iostream>
#include <iomanip>
#include <vector>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "Command.h"
#include "Serial.h"
#include "DimValue.h"
#include "Increment.h"

using namespace std;

class SPI {
public:
	enum Registers {
		IOCFG2=0,
		IOCFG1,
		IOCFG0,
		FIFOTHR,
		SYNC1,
		SYNC0,
		PKTLEN,
		PKTCTRL1,
		PKTCTRL0,
		ADDR,
		CHANNR,
		FSCTRL1,
		FSCTRL0,
		FREQ2,
		FREQ1,
		FREQ0,
		MDMCFG4,
		MDMCFG3,
		MDMCFG2,
		MDMCFG1,
		MDMCFG0,
		DEVIATN,
		MCSM2,
		MCSM1,
		MCSM0,
		FOCCFG,
		BSCFG,
		AGCCTRL2,
		AGCCTRL1,
		AGCCTRL0,
		WOREVT1,
		WOREVT0,
		WORCTRL,
		FREND1,
		FREND0,
		FSCAL3,
		FSCAL2,
		FSCAL1,
		FSCAL0,
		RCCTRL1,
		RCCTRL0,
		FSTEST,
		PTEST,
		AGCTEST,
		TEST2,
		TEST1,
		TEST0,
		REG_RESERVED,
		PARTNUM,
		VERSION,
		FREQEST,
		LQI,
		RSSI,
		MARCSTATE,
		WORTIME1,
		WORTIME0,
		PKTSTATUS,
		VCO_VC_DAC,
		TXBYTES,
		RXBYTES,
		RCCTRL1_STATUS,
		RCCTRL0_STATUS
	};
	enum Commands {
		SRES=0x30,
		SFSTXON,
		SXOFF,
		SCAL,
		SRX,
		STX,
		SIDLE,
		COM_RESERVED,
		SWOR,
		SPWD,
		SFRX,
		SFTX,
		SWORRST,
		SNOP
	};
	SPI();
	unsigned char ReadReg(Registers reg);
	void          WriteReg(Registers reg, unsigned char val);
	void          SendCommand(Commands cmd);
	void          ReadFIFO(unsigned int size);
	void          WriteFIFO(unsigned char* buf, int size);
private:
	int spiDev;
};

SPI::SPI()
{
	if ((spiDev = open("/dev/spidev0.0", O_RDWR)) < 0)
	{
		cout << "Erro abrindo arquivo!\n";
		exit(-1);
	}
		
	cout << "SPI aberta com sucesso!\n";
}

void SPI::SendCommand(Commands cmd)
{
	unsigned char txBuffer[1]={cmd};
	unsigned char rxBuffer[1]={0};

	struct spi_ioc_transfer xfer;
	memset(&xfer, 0, sizeof(xfer));
	xfer.tx_buf = (unsigned long)txBuffer;
	xfer.rx_buf = (unsigned long)rxBuffer;
	xfer.len = 1;

	int res = ioctl(spiDev, SPI_IOC_MESSAGE(1), &xfer);
	if (res <=0)
	{
		cout << "Erro escrevendo registrador!\n";
		exit(-1);
	}

	return;
}

void SPI::WriteReg(Registers reg, unsigned char val)
{
	unsigned char txBuffer[2]={reg,val};
	unsigned char rxBuffer[2]={0,0};

	struct spi_ioc_transfer xfer;
	memset(&xfer, 0, sizeof(xfer));
	xfer.tx_buf = (unsigned long)txBuffer;
	xfer.rx_buf = (unsigned long)rxBuffer;
	xfer.len = 2;

	int res = ioctl(spiDev, SPI_IOC_MESSAGE(1), &xfer);
	if (res <=0)
	{
		cout << "Erro escrevendo registrador!\n";
		exit(-1);
	}

	return;
}

unsigned char SPI::ReadReg(Registers reg)
{
	unsigned char txBuffer[2]={0,0};
	unsigned char rxBuffer[2]={0,0};

	txBuffer[0] = 0x80 | reg;
	if (reg >= 0x30)
		txBuffer[0] |= 0x40;

	struct spi_ioc_transfer xfer;
	memset(&xfer, 0, sizeof(xfer));
	xfer.tx_buf = (unsigned long)txBuffer;
	xfer.rx_buf = (unsigned long)rxBuffer;
	xfer.len = 2;

	int res = ioctl(spiDev, SPI_IOC_MESSAGE(1), &xfer);
	if (res <=0)
	{
		cout << "Erro lendo registrador!\n";
		exit(-1);
	}
	
	return rxBuffer[1];
}

void SPI::WriteFIFO(unsigned char* buf, int size)
{
	unsigned char *txBuffer=new unsigned char[size+1];
	unsigned char *rxBuffer=new unsigned char[size+1];

	txBuffer[0]=0x7f;

	for (int i=0;i<size;i++)
		txBuffer[i+1]=buf[i];

	struct spi_ioc_transfer xfer;
	memset(&xfer, 0, sizeof(xfer));
	xfer.tx_buf = (unsigned long)txBuffer;
	xfer.rx_buf = (unsigned long)rxBuffer;
	xfer.len = size+1;

	int res = ioctl(spiDev, SPI_IOC_MESSAGE(1), &xfer);
	if (res <=0)
	{
		cout << "Erro escrevendo registrador!\n";
		exit(-1);
	}

	delete[] txBuffer;
	delete[] rxBuffer;
}

void SPI::ReadFIFO(unsigned int size)
{
	unsigned char *txBuffer=new unsigned char[size+1];
	unsigned char *rxBuffer=new unsigned char[size+1];

	memset(txBuffer,0,size+1);
	txBuffer[0] = 0xFF;
	
	struct spi_ioc_transfer xfer;
	memset(&xfer, 0, sizeof(xfer));
	xfer.tx_buf = (unsigned long)txBuffer;
	xfer.rx_buf = (unsigned long)rxBuffer;
	xfer.len = size+1;

	int res = ioctl(spiDev, SPI_IOC_MESSAGE(1), &xfer);
	if (res <=0)
	{
		cout << "Erro lendo registrador!\n";
		exit(-1);
	}

	if (rxBuffer[1]==0x2f && rxBuffer[2]==0xcf && rxBuffer[5]==0x01)
	{
		for (unsigned int i=1;i<size+1-2;i++)
		{
			cout << std::setw(2) << (int)rxBuffer[i] << " ";
		}
		cout << '\n';
	}

	delete[] txBuffer;
	delete[] rxBuffer;
}

int main(int argc, char* argv[])
{
	const int pktlen = 33;
	bool rx=false;
	vector<unsigned char> tx_packet;
	

	if (argc == 1) {
		cout << "Iniciando em recepção..." << '\n';
		rx=true;
	}
	else if (argc == 4) {
		cout << "Iniciando TX de pacote..." << '\n';

		//Get count
		uint32_t count = strtol(argv[1],NULL,10);		
		//Get serial
		uint8_t sn[4];
		std::string string_serial=std::string(argv[2]);
		if (string_serial.length() != 8) {
			cout << "Invalid serial!\n";
			exit(-1);
		}
		for (unsigned int i = 0; i < string_serial.length(); i += 2) {
			std::string byteString = string_serial.substr(i, 2);
			sn[i/2] = (uint8_t) strtol(byteString.c_str(), NULL, 16);
		}
		//Get DimValue
		uint32_t val = strtol(argv[3],NULL,10);		
		
		//Calculate final packet
		Command change_dim({ 0x2f, 0xcf, 0x76, 0x0d, 0x01, 0x42, 0xda, 0xd4, 0x6c, 0x01, 0x42, 0x4e, 0x10, 0x05, 0x01, 0x6e, 0x4c, 0x13, 0x44, 0xff, 0x01, 0x48, 0x17, 0x55, 0xff, 0x00, 0x40, 0x10, 0x04, 0x01, 0x73, 0xc7, 0x51 });
		Serial serial(sn,change_dim);
		Increment i(count,change_dim);
		DimValue dv(val, change_dim);
		std::string s=(change_dim ^ i.cmd_diff ^ dv.cmd_diff ^ serial.cmd_diff).GetBytes();
		
		//Finish TX packet
		s=string("55555555ff")+s;
		if (s.length() % 2 != 0) {
			cout << "Impossivel transmitir hex impar!";
			exit(-1);
		}
		for (unsigned int i = 0; i < s.length(); i += 2) {
			std::string byteString = s.substr(i, 2);
			unsigned char byte = (unsigned char) strtol(byteString.c_str(), NULL, 16);
			tx_packet.push_back(byte);
		}
	}
	else {
		cout << "Numero de parametros invalido!\n";
		exit(-1);
	} 

	cout << std::hex << std::setfill('0'); 
	
	SPI spi;

	spi.SendCommand(SPI::SRES);
	usleep(1000);

	cout <<  "F0:" << std::setw(2) << (unsigned int)spi.ReadReg(SPI::PARTNUM) <<
		" F1:" << std::setw(2) << (unsigned int)spi.ReadReg(SPI::VERSION) << "\n";

	spi.WriteReg(SPI::IOCFG0,0x06);  //GDO0 Output Pin Configuration
	spi.WriteReg(SPI::FIFOTHR,0x47); //RX FIFO and TX FIFO Thresholds

	spi.WriteReg(SPI::SYNC1,0x55);
	spi.WriteReg(SPI::SYNC0,0xFF);
	spi.WriteReg(SPI::PKTLEN,pktlen);
	spi.WriteReg(SPI::PKTCTRL0,0x00);

	spi.WriteReg(SPI::FSCTRL1,0x06); //Frequency Synthesizer Control
	spi.WriteReg(SPI::FREQ2,0x10);   //Frequency Control Word, High Byte
	spi.WriteReg(SPI::FREQ1,0xAD);   //Frequency Control Word, Middle Byte
	spi.WriteReg(SPI::FREQ0,0x52);   //Frequency Control Word, Low Byte
	spi.WriteReg(SPI::MDMCFG4,0x0B); //Modem Configuration
	spi.WriteReg(SPI::MDMCFG3,0x3B); //Modem Configuration

	spi.WriteReg(SPI::MDMCFG2,0x12);

	spi.WriteReg(SPI::DEVIATN,0x41); //Modem Deviation Setting
	spi.WriteReg(SPI::MCSM0,0x18);   //Main Radio Control State Machine Configuration
	spi.WriteReg(SPI::FOCCFG,0x16);  //Frequency Offset Compensation Configuration
	spi.WriteReg(SPI::WORCTRL,0xFB); //Wake On Radio Control
	spi.WriteReg(SPI::FSCAL3,0xA9);  //Frequency Synthesizer Calibration
	spi.WriteReg(SPI::FSCAL2,0x2A);  //Frequency Synthesizer Calibration
	spi.WriteReg(SPI::FSCAL1,0x00);  //Frequency Synthesizer Calibration
	spi.WriteReg(SPI::FSCAL0,0x0D);  //Frequency Synthesizer Calibration
	spi.WriteReg(SPI::TEST2,0x81);   //Various Test Settings
	spi.WriteReg(SPI::TEST1,0x35);   //Various Test Settings
	spi.WriteReg(SPI::TEST0,0x09);   //Various Test Settings

	spi.SendCommand(SPI::SCAL);
	usleep(1000);

	if (rx) {
		spi.SendCommand(SPI::SRX);

		while (1)
		{
			int rxbytes = spi.ReadReg(SPI::RXBYTES);
			if (rxbytes == pktlen + 2) {
				spi.ReadFIFO(rxbytes);
				spi.SendCommand(SPI::SRX);
			}
		}
	}
	else {
		for (int i=0;i<3;i++)
		{
			cout << "Transmitindo " << std::dec << tx_packet.size() << " bytes!\n";
			spi.SendCommand(SPI::SFTX);
			spi.WriteReg(SPI::PKTLEN,39);
			spi.WriteReg(SPI::MDMCFG2,0x10); //No sync or preamble on TX
			spi.WriteFIFO(&tx_packet[0],38);	
			spi.SendCommand(SPI::STX);
			usleep(20000);
		}
	}

	return 0;
}
