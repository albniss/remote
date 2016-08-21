#include <iostream>
#include <iomanip>
#include <string>
#include <ctime>

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

int GetHumidity(void)
{
	for (int i=0;i<20;i++)
	{
		int fp=open("/sys/bus/iio/devices/iio:device0/in_humidityrelative_input",O_RDONLY ); 
		char buf[50];
		int count=read(fp,buf,50);
		close(fp);

		if (count > 0)
			return atoi(buf);
	}
	return 0;
}

int GetTemp(void)
{
	for (int i=0;i<20;i++)
	{
		int fp=open("/sys/bus/iio/devices/iio:device0/in_temp_input",O_RDONLY ); 
		char buf[50];
		int count=read(fp,buf,50);
		close(fp);

		if (count > 0)
			return atoi(buf);
	}
	return 0;
}

int main(void)
{
	time_t t=time(0);
	struct tm *now = localtime(&t);

	cout << fixed << setprecision(1) << GetHumidity()/1000.0 << ";";
	cout << fixed << setprecision(1) << GetTemp()/1000.0 << ";";
	cout << now->tm_year+1900 << ";";
	cout << setw(2) << setfill('0') << now->tm_mon+1 << ";";
	cout << setw(2) << setfill('0') << now->tm_mday << ";";
	cout << setw(2) << setfill('0') << now->tm_hour << ";";
	cout << setw(2) << setfill('0') << now->tm_min  << "\n";

	return 0;
}