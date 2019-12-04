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
#include <boost/chrono.hpp>
#include <boost/asio/ip/detail/endpoint.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio.hpp>
#include <string>
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <thread>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#pragma warning(disable : 4996)

using namespace boost::chrono;
using namespace boost::asio;
using namespace std;


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

long long PrintTimestamp()
{
	auto TimePoint = high_resolution_clock::now();
	auto now = system_clock::now();
	//auto in_time_t = system_clock::to_time_t(now);
	auto in_time_t = system_clock::to_time_t(now);

	milliseconds ms = duration_cast<milliseconds>(TimePoint.time_since_epoch());

	seconds s = duration_cast<seconds>(ms);
	time_t t = s.count();
	size_t fractional_seconds = ms.count() % 1000;

	cout << put_time(localtime(&in_time_t), "%Y-%m-%d %X:");
	cout << fractional_seconds << endl;

	auto TimestampFullms = (in_time_t * 1000) + fractional_seconds;

	return(TimestampFullms);
}

struct TelegrammItems
{
	boost::property_tree::ptree TelegrammTree;
	long long timestamp;
	int  AnalogValue1;
	int  AnalogValue2;
	long SentCounter;
};

std::stringstream BuildTelegrammTree(TelegrammItems* pItems)
{
	std::stringstream Telegramm;

	pItems->TelegrammTree.put("Timestamp", pItems->timestamp);
	pItems->TelegrammTree.put("AnalogValue1", pItems->AnalogValue1);
	pItems->TelegrammTree.put("AnalogValue2", pItems->AnalogValue2);
	pItems->TelegrammTree.put("SentCounter", pItems->SentCounter);

	boost::property_tree::json_parser::write_json(Telegramm, pItems->TelegrammTree);

	return(Telegramm);
}

class UdpSender {

private:
	boost::asio::io_service io_service;
	boost::asio::ip::udp::socket socket;
	boost::asio::ip::udp::endpoint remote_endpoint;

	void handle_send(boost::shared_ptr<std::string> /*message*/,
		const boost::system::error_code& /*error*/,
		std::size_t /*bytes_transferred*/)
	{
	}


public:

	UdpSender(const std::string& ip_address, const int port, const bool broadcast = false) : socket(io_service) {

		// Open socket
		socket.open(boost::asio::ip::udp::v4());

		// I wouldn't recommend broadcasting unless you are
		// in complete control of your subnet and know
		// what's on it and how it will react
		if (broadcast) {
			boost::asio::socket_base::broadcast option(true);
			socket.set_option(option);
		}

		// make endpoint
		remote_endpoint = boost::asio::ip::udp::endpoint(boost::asio::ip::make_address(ip_address.c_str()), port);
	}



	// Send a string to the preconfigured endpoint
	// via the open socket.
	void send(const std::string& message) {
		boost::system::error_code ignored_error;
		boost::shared_ptr<std::string> message_(
			new std::string(message));

		socket.async_send_to(boost::asio::buffer(*message_), remote_endpoint,
			boost::bind(&UdpSender::handle_send, this, message_,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));


	}

	// Send some binary data to the preconfigured endpoint
	// via the open socket.
	void send(const unsigned char* data, const int len) {
		boost::system::error_code ignored_error;
		socket.send_to(boost::asio::buffer(data, len), remote_endpoint, 0, ignored_error);
	}
};



int main()
{
	static int timer = 0;
	static unsigned long LoopCounter = 0;
	//auto now = system_clock::now();
	auto TimeStamp = 0; //= system_clock::to_time_t(now);

	Analyzer AnalogDataAnalyzer;

	wiringPiSetup();


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
		
		TimeStamp = PrintTimestamp();

		cout <<"Timestamp:" << TimeStamp << endl;

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