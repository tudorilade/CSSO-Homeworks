#include "Manager.h"


Manager::Manager(HWND pWnd)
{
	this->fileHandler = DirFileHandler();
	this->mainWindow = pWnd;
}

/**
* Helper method for setting up the fielhandler class. 
*/
DWORD Manager::setupFileHandler()
{
	
	if (!fileHandler.setUpDirAndFiles())
	{
		this->LOG(this->getLogger(), this->fileHandler.getLastError().c_str(), TRUE);
		return FALSE;
	}

	this->LOG(this->getLogger(), "Successfully created the initial setup", TRUE);
	return TRUE;
}

HWND Manager::getLogger()
{
	return logger;
}


void Manager::LOG(HWND logger, LPSTR buffer, BOOL addNewLine)
{
	int length = GetWindowTextLength(logger);
	SendMessage(logger, EM_SETSEL, (WPARAM)length, (LPARAM)length);
	SendMessage(logger, EM_REPLACESEL, FALSE, (LPARAM)buffer);
	if (addNewLine) {
		SendMessage(logger, EM_REPLACESEL, FALSE, (LPARAM)"\r\n");
	}
}

void Manager::LOG(HWND logger,LPCSTR buffer, BOOL addNewLine)
{
	int length = GetWindowTextLength(logger);
	SendMessage(logger, EM_SETSEL, (WPARAM)length, (LPARAM)length);
	SendMessage(logger, EM_REPLACESEL, FALSE, (LPARAM)buffer);
	if (addNewLine) {
		SendMessage(logger, EM_REPLACESEL, FALSE, (LPARAM)"\r\n");
	}
}

void Manager::LOG(HWND logger, LPWSTR buffer, BOOL addNewLine)
{
	int length = GetWindowTextLength(logger);
	SendMessageW(logger, EM_SETSEL, (WPARAM)length, (LPARAM)length);
	SendMessageW(logger, EM_REPLACESEL, FALSE, (LPARAM)buffer);
	if (addNewLine) {
		SendMessageW(logger, EM_REPLACESEL, FALSE, (LPARAM)"\r\n");
	}
}

void Manager::LOG(HWND logger, LPCWSTR buffer, BOOL addNewLine)
{
	int length = GetWindowTextLength(logger);
	SendMessageW(logger, EM_SETSEL, (WPARAM)length, (LPARAM)length);
	SendMessageW(logger, EM_REPLACESEL, FALSE, (LPARAM)buffer);
	if (addNewLine) {
		SendMessageW(logger, EM_REPLACESEL, FALSE, (LPARAM)"\r\n");
	}
}
