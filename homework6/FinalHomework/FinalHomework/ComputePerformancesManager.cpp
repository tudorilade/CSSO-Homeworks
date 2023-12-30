#include "ComputePerformancesManager.h"

ComputePerformancesManager::ComputePerformancesManager(HWND hWnd) : Manager(hWnd)
{
	this->logger = GetDlgItem(hWnd, ID_LOG_WINDOW_P);
}

void ComputePerformancesManager::execute()
{
	if (!this->setupFileHandler())
		return;
	this->processCommands();
	this->displayInformations();
	this->writeInfoToFile();
}

/**
* Method responsible for invoking the commands
*/
void ComputePerformancesManager::processCommands()
{
	for (Command* command : commands)
	{
		this->LOG(this->logger, command->getExecuteLog().c_str(), TRUE);
		command->execute(this->performancesResults);
	}
	this->LOG(this->logger, "Finalized collection of information\r\n", TRUE);
}

/**
* Method responsible for displaying the informantions after 
* the commands are executed to the corresponding window
*/
void ComputePerformancesManager::displayInformations()
{
	this->LOG(this->logger, "Displaying information\r\n", TRUE);
	// geting SID window
	HWND sidWindow = GetDlgItem(mainWindow, ID_SID_DISPLAY);
	SetWindowText(sidWindow, this->performancesResults.sidInfo.c_str());

	// geting HT window
	HWND HTWindow = GetDlgItem(mainWindow, ID_HT_DISPLAY);
	SetWindowText(HTWindow, this->performancesResults.HTApi.c_str());

	// geting NUMA window
	HWND numaWindow = GetDlgItem(mainWindow, ID_NUMA_DISPLAY);
	SetWindowText(numaWindow, this->performancesResults.NUMPAApi.c_str());

	// geting CPU window
	HWND cpuWindow = GetDlgItem(mainWindow, ID_CPU_DISPLAY);
	SetWindowText(cpuWindow, this->performancesResults.CPUApi.c_str());
}

/**
* Method responsible for writing the results to info.txt file
*/
void ComputePerformancesManager::writeInfoToFile()
{
	LOG(logger, "\r\nStarting to write info.txt info....", TRUE);
	
	HANDLE hInfo = this->fileHandler.openFile(this->fileHandler.getPath(INFO_FILE));

	if (hInfo == INVALID_HANDLE_VALUE)
	{
		LOG(logger, "Couldn't create / open ", FALSE);
		LOG(logger, this->fileHandler.getPath(INFO_FILE), TRUE);
		return;
	}

	string buffer = this->performancesResults.toString();
	if (!this->fileHandler.writeToFile(hInfo, buffer.c_str(), buffer.size())) {
		LOG(logger, "Couldn't write to", FALSE);
		LOG(logger, this->fileHandler.getPath(INFO_FILE), TRUE);
	}
	else {
		LOG(logger, "Successfully wrote info to", FALSE);
		LOG(logger, this->fileHandler.getPath(INFO_FILE), TRUE);
	}

	CloseHandle(hInfo);
}
