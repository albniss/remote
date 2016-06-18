#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <map>


using namespace std;

int main()
{
	map<string,unsigned char> modos;
	modos["AUTO"]=0;
	modos["COOL"]=1;
	//modos["DRY"] =2;
	//modos["FAN"] =3;
	modos["HEAT"]=4;

	map<string,unsigned char> fan;
	fan["AUTO"]=0;
	fan["1"]   =4;
	fan["2"]   =3;
	fan["3"]   =2;
	fan["4"]   =1;

	map<string,unsigned char> temp;
	for (int T=18;T<=30;T++)
	{
		stringstream ss;
		ss << T;
		temp[ss.str()]=16*(T-16);
	}

	map<string,unsigned char> lownoise;
	lownoise["0"]=32;
	lownoise["1"]=160;

	map<string,unsigned char> onoff;
	onoff["0"]=0;
	onoff["1"]=1;

	cout << "Welcome to the AC Encoder" << endl;

	ofstream of;
	of.open ("fujitsu.conf");

	of << endl;
	of << "begin remote" << endl;
	of << endl;
	of << "name fujitsu" << endl;
	of << "flags RAW_CODES" << endl;
	of << "eps 30" << endl;
	of << "aeps 100" << endl;
	of << endl;
	of << "repeat 0 0" << endl;
	of << "gap 100000" << endl;
	of << endl;
	of << "begin raw_codes" << endl;
	
	of << "name CMD_OFF" << endl;
	of << "3322 1579 "; //header

	unsigned char command[] = {20,99,0,16,16,2,253};
        for (int i=0;i<7;i++)
        {
	        for (int j=0;j<8;j++)
        	{
                	if ((command[i] & (1 << j)) != 0)
                        	of << "421 1210 ";
                        else
                        	of << "421 421 ";
                }
	}
        of << "466";

	for (auto T : temp)
		for (auto M : modos)
			for (auto F : fan)
				for (auto L : lownoise) 
					for (auto O : onoff)
					{
						of << endl << "name CMD_" << T.first << "_" << M.first << "_" << F.first << "_" << L.first << "_" << O.first << endl;
	
						unsigned char command[] = {20,99,0,16,16,254,9,48,80,0,0,0,0,0,160,0};
						command[ 8]=T.second | O.second;
						command[ 9]=M.second;
						command[10]=F.second;
						command[14]=L.second;
	
						unsigned int sum=0;
						for (int i=7;i<15;i++)
							sum+=command[i];
	
						command[15]=((256-(sum%256)) & 0xff);
	
						of << "3322 1579"; //header
						

						cout << "name CMD_" << T.first << "_" << M.first << "_" << F.first << "_" << L.first << "_" << O.first << ":";
						for (int i=0;i<16;i++)
						{
							cout << setw(3) << setfill('0') << (unsigned int)command[i] << " ";
							of << endl;
							for (int j=0;j<8;j++)
							{
								if ((command[i] & (1 << j)) != 0)
									of << "421 1210 ";  //of << "466 1187 ";
								else
									of << "421 421 "; //of << "466  377 ";
							}
						}
						of << "421" << endl; //of << endl << "466";
						cout << endl;
					}

	of << endl << "end raw_codes" << endl;
	of << endl;
	of << "end remote" << endl;
	of << endl;

	of.close();

	return 0;
}
