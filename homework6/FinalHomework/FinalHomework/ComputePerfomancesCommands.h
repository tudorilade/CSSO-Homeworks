#pragma once

#include "Command.h"


//
// Command to extract SID Info
//

class SIDInfo {

public:
	string getSIDInfo();
};

class SIDInfoCommand : public Command
{
private:
	SIDInfo sidInfo = SIDInfo();
public:
	void execute(myPerfResults&) override;
};


//
// Command to extract information about Hyperthreaded system
//

class HTInfo {

public:
	string getLogicalProcessorInformation();
};

class HTInfoCommand : public Command {
private:
	HTInfo htInfo = HTInfo();

public:
	void execute(myPerfResults&) override;
};


//
// Command to extract information using NUMA API
//

class NUMAInfo{

public:
	string getNumaInfo();
};


class NUMAInfoCommmand: public Command {
private:
	NUMAInfo numaInfo = NUMAInfo();

public:
	void execute(myPerfResults&) override;
};


//
// Command to extract information about CPU
//


class CPUInfo {

public:
	string getCPUInfo();
};


class CPUInfoCommand: public Command {
private:
	CPUInfo cpuInfo = CPUInfo();
public:
	void execute(myPerfResults&) override;
};
