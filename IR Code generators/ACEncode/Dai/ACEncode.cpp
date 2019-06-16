#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

using namespace std;

#define PULSE       ( 479)
#define SPACE_SHORT ( 389)
#define SPACE_LONG  (1260)

string PrintHeader()
{
	return "479 389 479 389 479 389 479 389 479 389 479 25175 3524 1692 ";
}

string PrintCommand(unsigned char Temp, unsigned char Mode, unsigned char Fan, unsigned char Eco, unsigned char Swing, unsigned char Onoff)
{
	string ret;

	ret+=PrintHeader();

	unsigned char command[19] = {0x11,0xDA,0x27,0x00,0x00,0x00,0x32,0x00,0xB0,0x00,0x00,0x00,0x00,0x00,0x00,0xC5,0x40,0x00,0xF9};

	command[ 5]=(Mode << 4) + Onoff;
	command[ 6]=Temp;
	command[ 8]=(Fan << 4) + Swing;
	command[16]=Eco;

	int chksum=0;
	for (int i=0;i<19-1;i++)
		chksum+=command[i];
	command[18]=chksum & 0xff;

	for (int i=0;i<19;i++)
	{
		for (int j=0;j<8;j++)
		{
			if ((command[i] & (1 << j)) != 0)
				ret+= to_string(PULSE) + " " + to_string(SPACE_LONG) + " ";
			else
				ret+= to_string(PULSE) + " " + to_string(SPACE_SHORT) + " ";
		}
		ret+="\n";
	}

	ret+= to_string(PULSE) + "\n" + "\n";

	return ret;
}

int main()
{
	map<string,unsigned char> temp;
	for (int T=18;T<=30;T++)
	{
		stringstream ss;
		ss << T;
		temp[ss.str()]=0x32+((T-25)*2);
	}

	map<string,unsigned char> modos = {
		{"AUTO", 0x0},
		{"COOL", 0x3},
		{"HEAT", 0x4},
	};

	map<string,unsigned char> fan = {
		{"AUTO", 0xA},
		{"NIGHT", 0xB},
		{"1", 0x3},
		{"2", 0x4},
		{"3", 0x5},
		{"4", 0x6},
		{"5", 0x7},
	};

	map<string,unsigned char> eco = {
		{"0", 0x00},
		{"1", 0x04},
	};

	map<string,unsigned char> swing = {
		{"0", 0x0},
		{"1", 0xF},
	};

	map<string,unsigned char> onoff = {
		{"ON", 0x0},
		{"OFF", 0x1},
	};

	cout << "Welcome to the AC Encoder" << endl;

	ofstream of;
	of.open ("daikin.conf");

	of << endl;
	of << "begin remote" << endl;
	of << endl;
	of << "name daikin" << endl;
	of << "flags RAW_CODES" << endl;
	of << "eps 30" << endl;
	of << "aeps 100" << endl;
	of << endl;
	of << "repeat 0 0" << endl;
	of << "gap 100000" << endl;
	of << endl;
	of << "begin raw_codes" << endl;
	

	//Imprime Off - O código de eco é especial (0x40) para OFF
	of << "name CMD_OFF" << endl;
	of << PrintCommand(temp["25"],modos["AUTO"],fan["AUTO"],0x40,swing["0"],onoff["OFF"]);

	//Imprime todo o resto
	for (auto T : temp)
		for (auto M : modos)
			for (auto F : fan)
				for (auto E : eco) 
					for (auto S : swing)
					{
						of << "name CMD_" << T.first << "_" << M.first << "_" << F.first << "_" << E.first << "_" << S.first << endl;
						of << PrintCommand(T.second,M.second,F.second,E.second,S.second,onoff["ON"]);
					}

	of << "end raw_codes" << endl << endl;
	of << "end remote" << endl << endl;

	of.close();

	return 0;
}
