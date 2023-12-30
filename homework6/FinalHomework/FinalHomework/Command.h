#pragma once
#include <string>
#include <iostream>
#include <windows.h>
#include <sddl.h>

using namespace std;

typedef struct MyPerformances {
	string sidInfo;
	string HTApi;
	string NUMPAApi;
	string CPUApi;

	string toString() {
		string buffer = "SID Info: \r\n";
		buffer += sidInfo;

		buffer += "\r\nHT API Info: \r\n";
		buffer += HTApi;

		buffer += "\r\nNUMA API Info: \r\n";
		buffer += NUMPAApi;

		buffer += "\r\nCPU API Info: \r\n";
		buffer += CPUApi;

		return buffer;
	}

} myPerfResults;


class Command
{
public:
	virtual void execute(myPerfResults&) = 0;
	virtual string getExecuteLog() = 0;
};
