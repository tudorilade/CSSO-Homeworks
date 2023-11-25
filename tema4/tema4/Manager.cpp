#include "Manager.h"


/**
* Main method responsible for providing the functionalities:
* 
* -> check if sold and deposit directory are successfully passed
* -> setup the directory, files and memory mapping needed for solving the homework
* -> launch the 3 processes
* -> wait for their completion
* -> read the result in order to populate the content 
* -> close the memory handles
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

	if (!handler.createMemoryMapping())
	{
		this->lastError = handler.getLastError();
		return 0;
	}

	if (!this->execute(handler)) {
        handler.closeMemoryHandles();
		return 0;
	}

	if (!handler.readResult()) {
		this->lastError = handler.getLastError();
        handler.closeMemoryHandles();
		return 0;
	}
	
	handler.closeMemoryHandles();

	return 1;

}

/**
* Starting the 3 processes in paralel
*/
BOOL Manager::execute(DirFileHandler& handler) {
    STARTUPINFO si[3];
    PROCESS_INFORMATION pi[3];
    LPCSTR paths[3] = {
        handler.getPathToExecutable(DEPOSIT_EXE),
        handler.getPathToExecutable(SOLD_EXE),
        handler.getPathToExecutable(DONATE_EXE)
    };

    // 1. Create MUTEX for sync the access to memory mapping
    
    // 2. 


    for (int i = 0; i < 3; ++i) {
        ZeroMemory(&si[i], sizeof(si[i]));
        ZeroMemory(&pi[i], sizeof(pi[i]));
        si[i].cb = sizeof(si[i]);
    }

    for (int i = 0; i < 3; ++i) {
        if (!CreateProcess(
            paths[i],
            NULL,  
            NULL,   
            NULL,   
            FALSE, 
            0,     
            NULL,   
            NULL,   
            &si[i],
            &pi[i] 
        )) {
            wstring wideString = handler.ConvertToWString(paths[i]);
            this->lastError = L"Process creation for " + wideString + L" failed: " + to_wstring(GetLastError())  +L"\r\n";
            // Clean up and return FALSE
            for (int j = 0; j <= i; j++) {
                CloseHandle(pi[j].hProcess);
                CloseHandle(pi[j].hThread);
            }
            return FALSE;
        }
    }

    HANDLE handles[] = { pi[0].hProcess, pi[1].hProcess, pi[2].hProcess };
    WaitForMultipleObjects(3, handles, TRUE, INFINITE);

    for (int i = 0; i < 3; ++i) {
        CloseHandle(pi[i].hProcess);
        CloseHandle(pi[i].hThread);
    }

    return TRUE;
}

/**
* Getter for retrieving the last error
*/
wstring Manager::getError()
{
	return this->lastError;
}

