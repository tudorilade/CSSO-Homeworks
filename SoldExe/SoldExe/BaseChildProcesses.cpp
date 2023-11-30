#include "BaseChildProcesses.h"

/**
* Helper method used for sorting a vector of strings of file name based on day / month / year
*/
bool BaseChildProcesses::compareFileNames(string& fileName1, string& fileName2) {
    int year1, month1, day1;
    int year2, month2, day2;

    if (sscanf_s(fileName1.c_str(), "%d.%d.%d", &year1, &month1, &day1) != 3) {
        cerr << "Error parsing date from file name: " << fileName1 << endl;
        return false;
    }

    if (sscanf_s(fileName2.c_str(), "%d.%d.%d", &year2, &month2, &day2) != 3) {
        cerr << "Error parsing date from file name: " << fileName2 << endl;
        return false;
    }

    // Compare dates
    if (year1 != year2) {
        return year1 < year2;
    }
    if (month1 != month2) {
        return month1 < month2;
    }
    return day1 < day2;
}

HANDLE BaseChildProcesses::getHandleForMappedFile(LPCSTR mappedFileName) {
    HANDLE hMap = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, mappedFileName);

    if (hMap == NULL) {
        printf("Error opening the memory-mapped file. Error code: %d\n", GetLastError());
        return NULL;
    }
    else {
        return hMap;
    }
}

/**
* Method responsible for constructing a path between directory and file which represents the absolute path to the file
*/
char* BaseChildProcesses::constructPath(const char* directory, const char* file) { //construct the full path of the file
    size_t dirLength = strlen(directory);
    size_t fileLength = strlen(file);

    char* pathToFile = (char*)malloc(dirLength + fileLength + 1);
    if (pathToFile == NULL) {
        return NULL;
    }
    if (strcpy_s(pathToFile, dirLength + fileLength + 1, directory) != 0) {
        free(pathToFile);
        return NULL;
    }
    pathToFile[dirLength - 1] = '\0';
    // Use strcat_s to concatenate the file
    if (strcat_s(pathToFile, dirLength + fileLength + 1, file) != 0) {
        free(pathToFile);
        return NULL;
    }

    return pathToFile;
}


BOOL BaseChildProcesses::getDepositFlag()
{
    return isDeposit;
}


void BaseChildProcesses::startProccessing(LPCSTR directoryDeposit) {

    // 1. Open Mutex, Events
    HANDLE hMutexCriticalSection = OpenMutex(SYNCHRONIZE, FALSE, CRITICAL_SECTION_MUTEX);
    HANDLE hMasterEvent = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, MASTER_EVENT); // for the master event
    HANDLE hDepositEvent = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, DEPOSIT_EVENT); // for the deposit event
    HANDLE hSoldEvent = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, SOLD_EVENT); // for the sold event
    HANDLE hDonationEvent = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, DONATION_EVENT); // for the donation event
    HANDLE hEventFirstFile = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, FIRST_DAY_EVENT);
    HANDLE hSoldFinalEvent = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, FINISHED_SOLD_EVENT);
    HANDLE hDepositFinalEvent = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, FINISHED_DEPOSIT_EVENT);
    HANDLE hDonationFinalEvent = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, FINISHED_DONATION_EVENT);
    // Check if the mutex handle is valid
    if (hMutexCriticalSection == NULL || hMutexCriticalSection == INVALID_HANDLE_VALUE) {
        cout << "CRITICAL_SECTION_MUTEX failed to open" << endl;
        Sleep(10000);
    }

    // Check if each event handle is valid
    if (hMasterEvent == NULL || hMasterEvent == INVALID_HANDLE_VALUE) {
        cout << "MASTER_EVENT failed to open" << endl;
        Sleep(10000);
    }

    if (hDepositEvent == NULL || hDepositEvent == INVALID_HANDLE_VALUE) {
        cout << "DEPOSIT_EVENT failed to open" << endl;
        Sleep(10000);
    }

    if (hSoldEvent == NULL || hSoldEvent == INVALID_HANDLE_VALUE) {
        cout << "SOLD_EVENT failed to open" << endl;
        Sleep(10000);
    }

    if (hDonationEvent == NULL || hDonationEvent == INVALID_HANDLE_VALUE) {
        cout << "DONATION_EVENT failed to open" << endl;
        Sleep(10000);
    }

    if (hEventFirstFile == NULL || hEventFirstFile == INVALID_HANDLE_VALUE) {
        cout << "FIRST_DAY_EVENT failed to open" << endl;
        Sleep(10000);
    }

    if (hSoldFinalEvent == NULL || hSoldFinalEvent == INVALID_HANDLE_VALUE) {
        cout << "FINISHED_SOLD_EVENT failed to open" << endl;
        Sleep(10000);
    }

    if (hDepositFinalEvent == NULL || hDepositFinalEvent == INVALID_HANDLE_VALUE) {
        cout << "FINISHED_DEPOSIT_EVENT failed to open" << endl;
        Sleep(10000);
    }

    if (hDonationFinalEvent == NULL || hDonationFinalEvent == INVALID_HANDLE_VALUE) {
        cout << "FINISHED_DONATION_EVENT failed to open" << endl;
        Sleep(10000);
    }


    // 2. return a vector with the name of files
    vector<string> filesInOrder = this->preprocessingFiles(directoryDeposit);

    cout << "Sold " <<  "Astept FIRST FILE!" << endl;
    WaitForSingleObject(hEventFirstFile, INFINITE);
       
    cout << " AM TRECUT DE FIRST FILE" << endl;
    for (auto it = filesInOrder.begin() ; it != filesInOrder.end(); ++it) {
        string& fileName = *it;

        WaitForSingleObject(hMutexCriticalSection, INFINITE);
        // Critical section

        if (this->proccessFile(constructPath(directoryDeposit, fileName.c_str())) == 0)
        {
            SetEvent(hSoldEvent);
            SetEvent(hDepositEvent);
            SetEvent(hDonationEvent); // make sure that master is noticed that a criticalError has happened, to shut down all processes
            SetEvent(hSoldFinalEvent);
            SetEvent(hDepositFinalEvent);
            SetEvent(hDonationFinalEvent); // make sure that master is noticed that a criticalError has happened, to shut down all processes
            cerr << "SOLD " << "Error opening the first file to parse!" << endl;
            ExitProcess(0);
        }

        ReleaseMutex(hMutexCriticalSection);

        // signals master that deposit.exe finished processing 1st day
        if (it == filesInOrder.end() - 1) {
            SetEvent(hSoldFinalEvent);
            SetEvent(hSoldEvent);
        }
        else {
            SetEvent(hSoldEvent);
        }

        cout << "Acesta este file - ul terminat: " << fileName << endl;
        DWORD waitResult = WaitForSingleObject(hMasterEvent, INFINITE);
        Sleep(1000);
    }

    cout << "Am terminat de procesat !" << endl;
    SetEvent(hSoldFinalEvent);
    CloseHandle(hMutexCriticalSection);

    CloseHandle(hMasterEvent);
    CloseHandle(hDepositEvent);
    CloseHandle(hSoldEvent);
    CloseHandle(hDonationEvent);
    CloseHandle(hEventFirstFile);

    // CloseHandle(hCriticalError);

    CloseHandle(hSoldFinalEvent);
    CloseHandle(hDepositFinalEvent);
    CloseHandle(hDonationFinalEvent);
}

/**
* Method responsible for returning a sorted vector of filenames based on compareFileNames comparator
*/
vector<string> BaseChildProcesses::preprocessingFiles(LPCSTR directoryPath) {
    WIN32_FIND_DATAA fileInfo;
    vector<string> fileNames;
    HANDLE firstFile = FindFirstFile(directoryPath, &fileInfo);
    if (firstFile == INVALID_HANDLE_VALUE) {
        printf("Error opening directory. Error code: %d\n", GetLastError());
        return fileNames;
    }

    do {
        if (strcmp(fileInfo.cFileName, "..") != 0 && strcmp(fileInfo.cFileName, ".") != 0) { //use the next file
            fileNames.push_back(fileInfo.cFileName);
        }
    } while (FindNextFile(firstFile, &fileInfo));
    if (!FindClose(firstFile)) {
        printf("Error at FindClose.\nError code: %d\n", GetLastError());
        return fileNames;
    }

    sort(fileNames.begin(), fileNames.end(), compareFileNames);

    return fileNames;
}