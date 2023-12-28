#pragma once

#include <vector>

#include "Manager.h"
#include "ComputePerfomancesCommands.h"

using namespace std;

class ComputePerformancesManager : public Manager
{
private:
	myPerfResults performancesResults;
	vector<Command*> commands = {
		new SIDInfoCommand(),
		new HTInfoCommand(),
		new NUMAInfoCommmand(),
		new CPUInfoCommand()
	};

public:
	ComputePerformancesManager() : Manager() {};
	ComputePerformancesManager(HWND hWnd) : Manager(hWnd) {};
	void execute() override;
	void processCommands();
	void displayInformations();
};
