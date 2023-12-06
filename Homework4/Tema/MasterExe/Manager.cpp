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
	
    // child processes have finished to execute

	handler.closeMemoryHandles();

	return 1;

}

/**
* Starting the 3 processes in paralel
*/
BOOL Manager::execute(DirFileHandler& handler) {
    STARTUPINFOW si[3];
    PROCESS_INFORMATION pi[3];
    LPCSTR paths[3] = {
        handler.getPathToExecutable(DEPOSIT_EXE),
        handler.getPathToExecutable(SOLD_EXE),
        handler.getPathToExecutable(DONATE_EXE)
    };

    // 1. Create MUTEX for sync the access to memory mapping
    HANDLE hMutex = CreateMutex(NULL, FALSE, CRITICAL_SECTION_MUTEX);

    // 2. Create the following events: Donation, Deposit, Sold, FinishedDonation, FinishedDeposit, FinishedSold, Master
    LPCSTR processingEventsNames[] = {
        DONATION_EVENT,
        DEPOSIT_EVENT,
        SOLD_EVENT
    };
    HANDLE processingEvents[3];

    if (!this->populateEventHandles(processingEvents, processingEventsNames, 3)) {
        this->lastError = L"Failed to create events: " + to_wstring(GetLastError()) + L"\r\n";
        return FALSE;
    }

    LPCSTR finalEventsNames[] = {
        FINISHED_DONATION_EVENT,
        FINISHED_DEPOSIT_EVENT,
        FINISHED_SOLD_EVENT
    };
    HANDLE finalEvents[3];

    if (!this->populateEventHandles(finalEvents, finalEventsNames, 3)) {
        this->lastError = L"Failed to create final events: " + to_wstring(GetLastError()) + L"\r\n";
        return FALSE;
    }

    HANDLE hMaster = CreateEvent(NULL, TRUE, FALSE, MASTER_EVENT);

    if (hMaster == NULL)
    {
        this->lastError = L"Failed to create maste event: " + to_wstring(GetLastError()) + L"\r\n";
        return FALSE;
    }

    // 3. Setting timetable

    HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, TIME_TABLE_EVENT);
    if (hTimer == NULL) {
        this->lastError = L"Failed to create timer: " + to_wstring(GetLastError()) + L"\r\n";
        return FALSE;
    }

    LARGE_INTEGER liDueTime;
    liDueTime.QuadPart = -600000000LL; // 60 seconds

    if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, FALSE)) {
        this->lastError = L"Failed to set timer: " + to_wstring(GetLastError()) + L"\r\n";
        return FALSE;
    }
    
    // Finished setup for events; Starting the processes;

    for (int i = 0; i < 3; ++i) {
        ZeroMemory(&si[i], sizeof(si[i]));
        ZeroMemory(&pi[i], sizeof(pi[i]));
        si[i].cb = sizeof(si[i]);

        wstring cmdLine = handler.getCLIArguments(paths[i]);
        wchar_t cmdLineArgs[4096];
        wcsncpy_s(cmdLineArgs, cmdLine.c_str(), _TRUNCATE);

        if (!CreateProcessW(
            NULL,
            cmdLineArgs,
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

    BOOL allProcessesCompleted = FALSE;
    while (!allProcessesCompleted) {
        // wait for all child processes to signal completion of a file
        WaitForMultipleObjects(3, processingEvents, TRUE, INFINITE);
        for (int i = 0; i < 3; ++i) {
            ResetEvent(processingEvents[i]);
        }

        // check if all child processes have completed all files
        if (WaitForMultipleObjects(3, finalEvents, TRUE, 0) == WAIT_OBJECT_0) {
            allProcessesCompleted = true;
        }
        else {
            // Allow children to move to the next file if they did not finish
            SetEvent(hMaster);
            ResetEvent(hMaster);
        }
    }

    WaitForMultipleObjects(3, handles, TRUE, INFINITE);

    for (int i = 0; i < 3; ++i) {
        // joining the processes
        CloseHandle(pi[i].hProcess);
        CloseHandle(pi[i].hThread);
        CloseHandle(finalEvents[i]);
        CloseHandle(processingEvents[i]);
    }

    // 1. closing the mutex
    CloseHandle(hMutex);
    CloseHandle(hMaster);
    CloseHandle(hTimer);

    return TRUE;
}

/**
* Getter for retrieving the last error
*/
wstring Manager::getError()
{
	return this->lastError;
}


BOOL Manager::populateEventHandles(HANDLE* eventHandles, LPCSTR* eventNames, int size) {

    eventHandles[0] = CreateEvent(NULL, TRUE, FALSE, eventNames[0]);
    eventHandles[1] = CreateEvent(NULL, TRUE, FALSE, eventNames[1]);
    eventHandles[2] = CreateEvent(NULL, TRUE, FALSE, eventNames[2]);

    for (int i = 0; i < 3; ++i) {
        if (eventHandles[i] == NULL) {
            return FALSE; 
        }
    }

    return TRUE;
}
