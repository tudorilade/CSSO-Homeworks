#pragma once

#include "Command.h"

#include <sstream>
#include <bitset>
#include <vector>


//
// Command to extract SID Info
//

class SIDInfo {

public:
	string getSIDInfo();
	string getCurrentUserSID();
	string getEveryoneGroupSID();
	string getAdminGroupSID();
};

class SIDInfoCommand : public Command
{
private:
	SIDInfo sidInfo = SIDInfo();
public:
	void execute(myPerfResults&) override;
	void execute(evPerfResults&) override {};
	string getExecuteLog() override {return "Collecting SID Info...";};
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
	void execute(evPerfResults&) override {}; 
	void execute(myPerfResults&) override;
	string getExecuteLog() override { return "Collecting HT Info..."; };
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
	void execute(evPerfResults&) override {}; 
	void execute(myPerfResults&) override;
	string getExecuteLog() override { return "Collecting NUMA Info..."; };
};


//
// Command to extract information about CPU
//


class CPUInfo {

public:
	string getCPUInfo();
	string getProcessDefaultCpuSetIDs();
	string getSystemCpuSetInformation();
};


class CPUInfoCommand: public Command {
private:
	CPUInfo cpuInfo = CPUInfo();
public:
	void execute(evPerfResults&) override {}; 
	void execute(myPerfResults&) override;
	string getExecuteLog() override { return "Collecting CPU Info..."; };
};
