#include "ComputePerfomancesCommands.h"

string SIDInfo::getSIDInfo()
{
	return "TODO: Info about SID here :)";
}

void SIDInfoCommand::execute(myPerfResults& result)
{
	result.sidInfo = this->sidInfo.getSIDInfo();
}


string HTInfo::getLogicalProcessorInformation()
{
	return "TODO: Info about Logical processor here :)";
}

void HTInfoCommand::execute(myPerfResults& result)
{
	result.HTApi = this->htInfo.getLogicalProcessorInformation();
}


string NUMAInfo::getNumaInfo()
{
	return "TODO: Info about NUMA here :)";
}

void NUMAInfoCommmand::execute(myPerfResults& result)
{
	result.NUMPAApi = this->numaInfo.getNumaInfo();
}


string CPUInfo::getCPUInfo()
{
	return "TODO: Info about CPU here :)";
}

void CPUInfoCommand::execute(myPerfResults& result)
{
	result.CPUApi = this->cpuInfo.getCPUInfo();
}

