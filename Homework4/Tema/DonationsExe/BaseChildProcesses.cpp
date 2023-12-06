#include "BaseChildProcesses.h"

/**
* Helper method used for sorting a vector of strings of file name based on day / month / year
*/
bool BaseChildProcesses::compareFileNames(std::string& fileName1, std::string& fileName2) {
    int year1, month1, day1;
    int year2, month2, day2;

    if (sscanf_s(fileName1.c_str(), "%d.%d.%d", &year1, &month1, &day1) != 3) {
        std::cerr << "Error parsing date from file name: " << fileName1 << std::endl;
        return false;
    }

    if (sscanf_s(fileName2.c_str(), "%d.%d.%d", &year2, &month2, &day2) != 3) {
        std::cerr << "Error parsing date from file name: " << fileName2 << std::endl;
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
        cerr << "Donations " << "Error opening the memory-mapped file. Error code: " << GetLastError();
        //printf("Error opening the memory-mapped file. Error code: %d\n", GetLastError());
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

    // 1. Open Mutex, Events and Timer
    HANDLE hMutexCriticalSection = OpenMutex(SYNCHRONIZE, FALSE, CRITICAL_SECTION_MUTEX);
    HANDLE hMasterEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, MASTER_EVENT); // for the master event
    HANDLE hDepositEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, DEPOSIT_EVENT); // for the deposit event
    HANDLE hSoldEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, SOLD_EVENT); // for the sold event
    HANDLE hDonationEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, DONATION_EVENT); // for the donation event
    HANDLE hEventFirstFile = OpenEvent(EVENT_MODIFY_STATE, FALSE, FIRST_DAY_EVENT);
    HANDLE hCriticalError = OpenEvent(EVENT_MODIFY_STATE, FALSE, ABORT_EVENT);
    HANDLE hFinal = OpenEvent(EVENT_MODIFY_STATE, FALSE, FINISHED_DONATION_EVENT);


    // 1. return a vector with the name of files
    vector<string> filesInOrder = this->preprocessingFiles(directoryDeposit);

    auto it = filesInOrder.begin(); // get an iterator from the begining of vector

    if (!filesInOrder.empty() && this->getDepositFlag()) { // process the first file
        string& firstFileName = filesInOrder.front();
        if (this->proccessFile(constructPath(directoryDeposit, firstFileName.c_str())) == 0)
        {
            SetEvent(hCriticalError);
            SetEvent(hSoldEvent);
            SetEvent(hDepositEvent);
            SetEvent(hDonationEvent); // make sure that master is noticed that a criticalError has happened, to shut down all processes
            std::cerr << "Error opening the first file to parse!" << std::endl;
            ExitProcess(0);
        }
        //set event pentru ca am terminat primul file
        SetEvent(hEventFirstFile); // am anuntat celelalte 2 procese ca s a terminat cu prima zi
        it++;
    }
    else {
        cout << "donations " << "Astept event!" << endl;
        Sleep(3000);
        WaitForSingleObject(hEventFirstFile, INFINITE);
    }


    // 1. Open the mutex for critical section

    for (; it != filesInOrder.end(); ++it) {
        string& fileName = *it;

        WaitForSingleObject(hMutexCriticalSection, INFINITE);
        // Critical section

     //   cout << fileName << endl;
        if (this->proccessFile(constructPath(directoryDeposit, fileName.c_str())) == 0)
        {
            SetEvent(hCriticalError);
            SetEvent(hSoldEvent);
            SetEvent(hDepositEvent);
            SetEvent(hDonationEvent); // make sure that master is noticed that a criticalError has happened, to shut down all processes
            std::cerr << "Donations " << "Error opening the first file to parse!" << GetLastError() << std::endl;

            //std::cerr << "Error opening the first file to parse aici!" << std::endl;
            ExitProcess(0);
        }
        ReleaseMutex(hMutexCriticalSection);

        // signals master that deposit.exe finished processing 1st day
        SetEvent(hDonationEvent);
        cout << "SOLD " << "Am terminat al doilea file, acum astept un input ca sa simulez wait!Acesta este file - ul: " << fileName << endl;
        Sleep(3000);
        WaitForSingleObject(hMasterEvent, INFINITE); // Waits signal from master until all silblings finish to process the first day.
    }

    cout << "Donations " << "Am TERMINAT" << endl;
    SetEvent(hFinal);
}

/**
* Method responsible for returning a sorted vector of filenames based on compareFileNames comparator
*/
std::vector<string> BaseChildProcesses::preprocessingFiles(LPCSTR directoryPath) {
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