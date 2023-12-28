#include "Manager.h"


Manager::Manager(HWND pWnd)
{
	this->logger = GetDlgItem(pWnd, ID_LOG_WINDOW_P);
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

	this->LOG(this->getLogger(), "There you go ! Need to implement the actual computation using Command execute pattern", TRUE);
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
