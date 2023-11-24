#include "Manager.h"


/**
* Main method responsible for providing the functionalities 
*/
DWORD Manager::startManagement(DirFileHandler& handler)
{
	if (handler.getPathDir(SOLD_DIR).empty())
	{
		this->lastError = L"Path to Sold directory is not given\r\n";
		return 0;
	}

	if (handler.getPathDir(DEPOSIT_DIR).empty())
	{
		this->lastError = L"Path to Deposit directory is not given\r\n";
		return 0;
	}

	if (!handler.setUpDirAndFiles())
	{
		this->lastError = handler.getLastError();
		return 0;
	}

	return 1;

}

wstring Manager::getError()
{
	return this->lastError;
}

