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
} myPerfResults;


class Command
{
public:
	virtual void execute(myPerfResults&) = 0;
	virtual string getExecuteLog() = 0;
};
