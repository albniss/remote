#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

using namespace std;

#define PULSE       ( 560)
#define SPACE_SHORT ( 435)
#define SPACE_LONG  (1444)

string PrintHeader()
{
	return "635 17855 3063 8908 ";
}

string PrintSeparator()
{
	return "582 2922 3040 8970 ";
}

string PrintCommand(unsigned char T, unsigned char M, unsigned char F, unsigned char L, unsigned char S, bool ON)
{
	string ret;

	ret+=PrintHeader();

	unsigned int command[21] = {
		0x02,0x02,0x0F,0x00,0x00,0x00,0xF0,
		0x01,0xD2,0x0F,0x00,0x00,0x00,0x00,
		0x01,0x02,0x00,0x71,0x00,0x00,0xF0
		};

	if (ON)
		command[ 6]=command[20]=0xF0;
	else
		command[ 6]=command[20]=0xC0;

	command[ 5]=L;
	command[16]=S;
	command[18]=T;
	command[19]=M | F;

	for (int i=0;i<3;i++)
	{
		unsigned char nones = __builtin_popcount(command[i*7+0]) + __builtin_popcount(command[i*7+1] & 0x0F) +
			__builtin_popcount(command[i*7+2] & 0xF0) + __builtin_popcount(command[i*7+3]) +
			__builtin_popcount(command[i*7+4]) + __builtin_popcount(command[i*7+5]) +
			__builtin_popcount(command[i*7+6]);

		unsigned char chk_low  = (255-nones) & 0x0F;
		unsigned char chk_high = ((255-nones) & 0xF0) >> 4;

		command[i*7+1] |= chk_low << 4; 
		command[i*7+2] |= chk_high;
	}

	for (int i=0;i<21;i++)
	{
		if (i==7 || i==14)
		{
			ret+="\n";
			ret+=PrintSeparator();
		}

		for (int j=0;j<8;j++)
		{
			if ((command[i] & (1 << j)) != 0)
				ret+= to_string(PULSE) + " " + to_string(SPACE_LONG) + " ";
			else
				ret+= to_string(PULSE) + " " + to_string(SPACE_SHORT) + " ";
		}
	}

	ret+= to_string(PULSE) + "\n" + "\n";

	return ret;
}

int main()
{
	map<string,unsigned char> modos;
	modos["AUTO"]=0x00;
	modos["COOL"]=0x10;
	modos["HEAT"]=0x40;

	map<string,unsigned char> fan;
	fan["AUTO"]  =0x01;
	fan["1"]     =0x05;
	fan["2"]     =0x09;
	fan["3"]     =0x0B;

	map<string,unsigned char> temp;
	for (int T=16;T<=30;T++)
	{
		stringstream ss;
		ss << T;
		temp[ss.str()]=16*(T-16);
	}

	map<string,unsigned char> lownoise;
	lownoise["1"]=0x20;
	lownoise["0"]=0x00;

	map<string,unsigned char> onoff;
	onoff["0"]=0xC0;
	onoff["1"]=0xF0;

	map<string,unsigned char> swing;
	swing["0"]=0xF0;
	swing["1"]=0xA0;

	cout << "Welcome to the AC Encoder" << endl;

	ofstream of;
	of.open ("samsung.conf");

	of << endl;
	of << "begin remote" << endl;
	of << endl;
	of << "name samsung" << endl;
	of << "flags RAW_CODES" << endl;
	of << "eps 30" << endl;
	of << "aeps 100" << endl;
	of << endl;
	of << "repeat 0 0" << endl;
	of << "gap 100000" << endl;
	of << endl;
	of << "begin raw_codes" << endl;
	

	//Imprime Off
	of << "name CMD_OFF" << endl;
	of << PrintCommand(16,0x00,0x0D,0x00,0xF0,false);

	//Imprime todo o resto
	for (auto T : temp)
		for (auto M : modos)
			for (auto F : fan)
				for (auto L : lownoise) 
					for (auto S : swing)
					{
						if (M.first == "AUTO" && F.first != "AUTO")
							continue;

						of << "name CMD_" << T.first << "_" << M.first << "_" << F.first << "_" << L.first << "_" << S.first << endl;
						of << PrintCommand(T.second,M.second,F.second,L.second,S.second,true);
					}

	of << "end raw_codes" << endl << endl;
	of << "end remote" << endl << endl;

	of.close();

	return 0;
}
