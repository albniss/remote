#include <iostream>
#include <iomanip>
#include <vector>

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>

#define PULSE_BIT       0x01000000
#define PULSE_MASK      0x00FFFFFF

//#define PRINT_DEBUG

using namespace std;

struct Pulse {
	unsigned int length : 24;
	unsigned char type : 1;
	unsigned char dummy : 7;
};

class ACRemote {
private:
	static constexpr int RETRYCOUNT = 1;
	unsigned int header_avgpulse[2];
	unsigned int header_avgspace[2];
	unsigned int separator_avgpulse[2];
	unsigned int separator_avgspace[2];
	unsigned int avgpulse;
	unsigned int avgspace;

	vector< vector<Pulse> > readings;
	vector<Pulse> ReadPulses();
public:
	int GetHeader();
	int GetSeparator();
	int GetAverageLengths();
	void PrintBits();
};

void ACRemote::PrintBits()
{
	for (unsigned int i=0;i<readings.size();i++)
	{
		vector<unsigned char> bytes;

		int nbits=0;
		unsigned int byte=0;

		//start from first space
		for (unsigned int j=1;j<readings[i].size();j+=2)
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

		for (unsigned int j=0; j < bytes.size() ; j++) {
			cout << setfill('0') << setw(3) << (int)bytes[j] << " ";
		}

		int nones=0;
		for (int i=7;i<14;i++)
		{
			if (i==8)
				continue;
			for (int j=0;j<8;j++)
				if ((bytes[i] & 1<<j) != 0)
					nones++;
		}
		cout << "| chk1: " << setw(2) << ((bytes[8] & 0xF0)>>4) << 
			" | n1: " << setw(2) << nones << 
			" | chk1+n1: " << setw(2) << ((bytes[8] & 0xF0)>>4) + nones;

		if (bytes.size() > 20) 
		{
			nones=0;
			for (int i=14;i<21;i++)
			{
				if (i==15)
					continue;
				for (int j=0;j<8;j++)
					if ((bytes[i] & 1<<j) != 0)
						nones++;
			}
			cout <<  "| chk2: " << setw(2) << ((bytes[15] & 0xF0)>>4) << 
					 " | n2: " << setw(2) << nones << 
					 " | chk2+n2: " << setw(2) << ((bytes[15] & 0xF0)>>4) + nones;			
		}

		cout << " --- Read " << nbits << endl;
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

#ifdef PRINT_DEBUG
	cout << "Pulse: " << avgpulse << "Space: " << avgspace << endl;
#endif

	return 0;
}

int ACRemote::GetHeader() 
{
	header_avgpulse[0]=header_avgpulse[1]=0;
	header_avgspace[0]=header_avgspace[1]=0;

	for (int i=0;i<RETRYCOUNT;i++) 
	{
#ifdef PRINT_DEBUG
		cout << "Reading number " << i+1 << ". Please press any key once (and only once)" << endl;
#endif
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

		header_avgpulse[0]+=readings[i][0].length;
		header_avgspace[0]+=readings[i][1].length;
		header_avgpulse[1]+=readings[i][2].length;
		header_avgspace[1]+=readings[i][3].length;

		//Purge the header from the data bits
		readings[i].erase(readings[i].begin());
		readings[i].erase(readings[i].begin());
		readings[i].erase(readings[i].begin());
		readings[i].erase(readings[i].begin());
	}
	header_avgpulse[0]/=RETRYCOUNT;
	header_avgspace[0]/=RETRYCOUNT;
	header_avgpulse[1]/=RETRYCOUNT;
	header_avgspace[1]/=RETRYCOUNT;

#ifdef PRINT_DEBUG
	cout << "Got it! Header: " << 
		"  Pulse:" << header_avgpulse[0] << " Space: " << header_avgspace[0] << 
		"  Pulse:" << header_avgpulse[1] << " Space: " << header_avgspace[1] << endl;
#endif

	return 0;
}

int ACRemote::GetSeparator() 
{
	separator_avgpulse[0]=separator_avgpulse[1]=0;
	separator_avgspace[0]=separator_avgspace[1]=0;

	int n=0;

	for (int i=0;i<RETRYCOUNT;i++) 
	{
		if (readings[i][0].type==0) {
			cerr << "Error reading separator. The first line on the buffer should be a pulse!" << endl;
			return -1;
		}

		n++;

		separator_avgpulse[0]+=readings[i][112].length;
		separator_avgspace[0]+=readings[i][113].length;
		separator_avgpulse[1]+=readings[i][114].length;
		separator_avgspace[1]+=readings[i][115].length;

		if (readings[i].size() > 250)
		{
			n++;

			separator_avgpulse[0]+=readings[i][228].length;
			separator_avgspace[0]+=readings[i][229].length;
			separator_avgpulse[1]+=readings[i][230].length;
			separator_avgspace[1]+=readings[i][231].length;

			//Purge the separator from the data bits
			readings[i].erase(readings[i].begin()+231);
			readings[i].erase(readings[i].begin()+230);
			readings[i].erase(readings[i].begin()+229);
			readings[i].erase(readings[i].begin()+228);
		}

		//Purge the separator from the data bits
		readings[i].erase(readings[i].begin()+115);
		readings[i].erase(readings[i].begin()+114);
		readings[i].erase(readings[i].begin()+113);
		readings[i].erase(readings[i].begin()+112);
	}
	separator_avgpulse[0]/=n;
	separator_avgspace[0]/=n;
	separator_avgpulse[1]/=n;
	separator_avgspace[1]/=n;

#ifdef PRINT_DEBUG
	cout << "Got it! Separator: " << 
		"  Pulse:" << separator_avgpulse[0] << " Space: " << separator_avgspace[0] << 
		"  Pulse:" << separator_avgpulse[1] << " Space: " << separator_avgspace[1] << endl;
#endif

	return 0;
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

	//Leave the compiler happy
	return pulses;
}

int main()
{
	cout << "Welcome to the AC Decoder" << endl;

	while (1) {
		ACRemote acr;
		if (acr.GetHeader()!=0) {
			cerr << "Get header failed!" << endl;
			exit(-1);
		}
		if (acr.GetSeparator()!=0) {
			cerr << "Get separator failed!" << endl;
			exit(-1);
		}
		if (acr.GetAverageLengths()!=0) {
			cerr << "Get average lengths failed!" << endl;
			exit(-1);
		}
		acr.PrintBits();
	}

	return 0;
}