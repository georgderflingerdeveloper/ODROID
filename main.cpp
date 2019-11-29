#include <string.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <ctime>
#include <stdfix.h>
#include <string>
#include <sstream>
#include <time.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringSerial.h>
#include <lcd.h>
#include "Analyzing.h"
#include "Analyzing.cpp"
#include <ctime>
#include <boost/chrono.hpp>
#include <boost/asio/ip/detail/endpoint.hpp>
#include <boost/asio/io_service.hpp>
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time


using namespace boost::chrono;
using namespace boost::asio;



constexpr auto DATA_UPDATE_PERIOD = 100;

//------------------------------------------------------------------------------------------------------------
//
// ADC:
//
//------------------------------------------------------------------------------------------------------------
constexpr auto PORT_ADC1 = 0;
constexpr auto PORT_ADC2 = 1;

static int adcValue1 = 0;
static int adcValue2 = 0;




//------------------------------------------------------------------------------------------------------------
//
// LED:
//
//------------------------------------------------------------------------------------------------------------
static int ledPos = 0;

const int ledPorts[] = {
	24,
	23,
	22,
	21,
	14,
	13,
	12,
	3,
	2,
	0,
	7,

	1,
	4,
	5,
	6,
	10,
	11,
	26,
	27,
};

#define MAX_LED_CNT sizeof(ledPorts) / sizeof(ledPorts[0])

//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
//
// system init
//
//------------------------------------------------------------------------------------------------------------
int system_init(void)
{
	int i;

	// GPIO Init(LED Port ALL Output)
	for (i = 0; i < MAX_LED_CNT; i++)
	{
		pinMode(ledPorts[i], OUTPUT);
	}

	return  0;
}

/*std::string return_current_time_and_date()
{
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
	return ss.str();
}
*/

void PrintTimestamp()
{
	auto TimePoint = high_resolution_clock::now();
	auto now = system_clock::now();
	auto in_time_t = system_clock::to_time_t(now);

	milliseconds ms = duration_cast<milliseconds>(TimePoint.time_since_epoch());

	seconds s = duration_cast<seconds>(ms);
	std::time_t t = s.count();
	std::size_t fractional_seconds = ms.count() % 1000;

	//std::cout << put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
	//std::cout << std::ctime(&t) << std::endl;
	//std::cout << fractional_seconds << std::endl;
}


int main()
{
	static int timer = 0;
	static unsigned long LoopCounter = 0;

	boost::asio::io_service io_service;
	std::string Test = "Hallo....";


	Analyzer AnalogDataAnalyzer;

	printf("ADC data: \n");

	wiringPiSetup();

	if (system_init() < 0)
	{
		fprintf(stderr, "%s: System Init failed\n", __func__);     return -1;
	}

	for (;;) 
	{
		usleep(100000);

		if (millis() < timer)
		{
			continue;
		}

		timer = millis() + DATA_UPDATE_PERIOD;

		// All Data update
		// boardDataUpdate();
		AnalogDataAnalyzer.VerifyRawValue(analogRead(PORT_ADC1), adcValue1);
		AnalogDataAnalyzer.VerifyRawValue(analogRead(PORT_ADC2), adcValue2);

		auto Timestamp = system_clock::now();
		auto TimestampMs = time_point_cast<milliseconds>(Timestamp);


		PrintTimestamp();
		printf(" Timestamp = %u", TimestampMs);
		printf(" Actual value ADC1: %u ", adcValue1);
		printf(" Actual value ADC2: %u ", adcValue2);
		printf(" Counter Value: %u", LoopCounter++ );
		printf("\n\r");
		//
		// build JSON telegramm

		// send data via UDP
	}

	return 0;
}