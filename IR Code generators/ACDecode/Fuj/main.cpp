#include <iostream>
#include <iomanip>
#include <vector>

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>

#define PULSE_BIT       0x01000000
#define PULSE_MASK      0x00FFFFFF

using namespace std;

struct Pulse {
	unsigned int length : 24;
	unsigned char type : 1;
	unsigned char dummy : 7;
};

class ACRemote {
private:
	static constexpr int RETRYCOUNT = 3;
	unsigned int header_avgpulse;
	unsigned int header_avgspace;
	unsigned int avgpulse;
	unsigned int avgspace;

	vector< vector<Pulse> > readings;
	vector<Pulse> ReadPulses();
public:
	int GetHeader();
	int GetAverageLengths();
	void PrintBits();
};

void ACRemote::PrintBits()
{
	for (int i=0;i<readings.size();i++)
	{
		vector<unsigned char> bytes;

		int nbits=0;
		unsigned int byte=0;

		//start from first space
		for (int j=1;j<readings[i].size();j+=2)
		{
			if (readings[i][j].length > avgspace)
			{
				//cout << "1";
				byte+=(1 << (nbits%8));
			}
			else
			{
				//cout << "0";
			}

			nbits++;
			if (nbits%8==0)
			{
				bytes.push_back(byte);
				byte=0;
			}
		}

		for (auto b : bytes)
			cout << setfill('0') << setw(3) << (int)b << " ";

		cout << " --- Read " << nbits << " bits!" << endl;
	}
}

int ACRemote::GetAverageLengths()
{
	avgspace=0;
	avgpulse=0;
	int npulses=0;
	int nspaces=0;

	for (auto reading : readings)
		for (auto p : reading) 
		{
			if (p.length>4000) {
				cout << "/!\\ Ignoring " << (p.type ? "pulse" : "space") << " larger than 10ms" << endl;
				continue;
			}
			if (p.type) {
				npulses++;
				avgpulse+=p.length;
			}
			else {
				nspaces++;
				avgspace+=p.length;
			}
		}

	avgpulse/=npulses;
	avgspace/=nspaces;

	cout << "Got it! Average Pulse: " << avgpulse << " Average Space: " << avgspace << endl;
}

int ACRemote::GetHeader() 
{
	header_avgspace=0;
	header_avgpulse=0;

	for (int i=0;i<RETRYCOUNT;i++) 
	{
		cout << "Reading number " << i+1 << ". Please press any key once (and only once)" << endl;
		readings.push_back(ReadPulses());
		if (readings[i].size()==0) {
			cerr << "Why didn't you press a key? It is very easy to do so you know..." << endl;
			return -1;
		}
	}

	for (int i=0;i<RETRYCOUNT;i++) 
	{
		if (readings[i][0].type==1) {
			cerr << "Error reading header. The first line on the buffer should be a space!" << endl;
			return -1;
		}
		//Purge the first space
		readings[i].erase(readings[i].begin());

		header_avgpulse+=readings[i][0].length;
		header_avgspace+=readings[i][1].length;

		//Purge the header from the data bits
		readings[i].erase(readings[i].begin());
		readings[i].erase(readings[i].begin());
	}
	header_avgpulse/=RETRYCOUNT;
	header_avgspace/=RETRYCOUNT;

	cout << "Got it! Header Average Pulse: " << header_avgpulse << " Header Average Space: " << header_avgspace << endl;
}

vector<Pulse> ACRemote::ReadPulses()
{
	vector<Pulse> pulses;

	int fd = open("/dev/lirc0", O_RDONLY);
	if (fd<0) {
		cerr << "Error opening /dev/lirc0!" << endl;
		exit(-1);
	}

	bool flag=true;
	bool gotsomething=false;

	while (flag)
	{
		fd_set set;
		FD_ZERO(&set); 
		FD_SET(fd, &set);

		struct timeval timeout;

		if (gotsomething) {
			timeout.tv_sec = 0;
			timeout.tv_usec = 100000;
		}
		else {
			timeout.tv_sec = 10;
			timeout.tv_usec = 0;
		}
		
		int rv = select(fd + 1, &set, NULL, NULL, &timeout);
		if(rv == -1) {
			cerr << "Select error!" << endl;
			exit(-1);
		}
		else if(rv == 0) {
			close (fd);
			return pulses;
		}
		else {
			Pulse p;

			read(fd,&p,sizeof(Pulse));
//			cout << (p.type ? "PULSE: " : "SPACE: ");
//			cout << p.length << endl ;

			pulses.push_back(p);

			gotsomething=true;
		}
	}
}

int main()
{
	cout << "Welcome to the AC Decoder" << endl;

	ACRemote acr;
	if (acr.GetHeader()!=0)
		exit(-1);
	if (acr.GetAverageLengths()!=0)
		exit(-1);
	acr.PrintBits();

	return 0;
}
