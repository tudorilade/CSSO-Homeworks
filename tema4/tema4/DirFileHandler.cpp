#include "DirFileHandler.h"


/**
* Method responsible for setting the path to sold or deposit directory
*/
DWORD DirFileHandler::setPathDir(wstring path, DWORD typeOfDir)
{
	switch (typeOfDir)
	{
	case DEPOSIT_DIR:
		this->pathToDeposit = path;
		return 1;
	case SOLD_DIR:
		this->pathToSold = path;
		return 1;
	default:
		return 0;
	}
}

wstring DirFileHandler::getPathDir(DWORD typeOfDir) {
	switch (typeOfDir) {
	case DEPOSIT_DIR: return this->pathToDeposit;
	case SOLD_DIR: return this->pathToSold;
	}
	return NULL;
}

wstring DirFileHandler::getContentForConsole()
{
	wstringstream ss;

	ss << L"Content for console ... " << L"\r\n";
	ss << this->pathToDeposit << L"\r\n";
	ss << this->pathToSold << L"\r\n";

	return ss.str();
}

