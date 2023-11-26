#include "Deposit.h"
#include <stdio.h>
#include "../../../tema4/tema4/constants.h"
#include <iostream>
#include <vector>
#include <algorithm>

bool compareFileNames( std::string& fileName1,  std::string& fileName2) {
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

HANDLE getHandleForMappedFile(LPCSTR mappedFileName) {
    HANDLE hMap = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, mappedFileName);

    if (hMap == NULL) {
        printf("Error opening the memory-mapped file. Error code: %d\n", GetLastError());
        return NULL;
    }
    else {
        return hMap;
    }
}

char* constructPath(const char* directory, const char* file) { //construct the full path of the file
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

std::vector<string> Deposit::preprocessingFiles(LPCSTR directoryPath) {
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


ProductInfo Deposit::processLineAndGetProduct(const char* line) {
    
    int id_product, expires_in, shelve_id, product_price;
    if (sscanf_s(line, "%d,%d,%d,%d", &id_product, &expires_in, &shelve_id, &product_price) == 4) {
        return ProductInfo(id_product, expires_in, shelve_id, product_price);
    }
    return ProductInfo(-1, -1, -1, -1);
}

std::vector<ProductInfo> Deposit::processLines(HANDLE fileToParse)
{

    vector<ProductInfo> productInformation;

    DWORD dwBytesRead;
    const DWORD bufferSize = 1024;
    char buffer[bufferSize];

    char currentLine[bufferSize];
    size_t currentLineIndex = 0;

    while (ReadFile(fileToParse, buffer, bufferSize - 1, &dwBytesRead, NULL) && dwBytesRead > 0) {
        buffer[dwBytesRead] = '\0'; // Null-terminate the buffer

        for (char* p = buffer; *p; ++p) {
            if (*p == '\n') {
                currentLine[currentLineIndex] = '\0';

                ///add to vector of products 
                ProductInfo product = processLineAndGetProduct(currentLine);
                if (product.getIdProduct() != -1) {
                    // Only add valid products to the vector
                    productInformation.push_back(product);
                }
                currentLineIndex = 0;
            }
            else {
                currentLine[currentLineIndex++] = *p;
            }
        }
    }

    if (currentLineIndex > 0) {
        // Null-terminate the last line if not terminated with '\n'
        currentLine[currentLineIndex] = '\0';
        productInformation.push_back(processLineAndGetProduct(currentLine));
    }

    return productInformation;

}



int Deposit::proccessFile(LPCSTR firstFilePath) {
    HANDLE firstFile = CreateFile(
        firstFilePath, 
        GENERIC_READ, 
        0, 
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    
    if (firstFile == INVALID_HANDLE_VALUE) {
        // Failed to open the file
        DWORD error = GetLastError();
        std::cerr << "Error opening file: " << firstFilePath << ". Error code: " << error << std::endl;
        return 0;
    }
    //HANDLE hMarketShelves = getHandleForMappedFile(marketShelves);
   // HANDLE hMarketValability = getHandleForMappedFile(marketValability);
   // HANDLE hProductPrices = getHandleForMappedFile(productPrices);
    /*if (this->CheckForErrors(hMarketShelves, hMarketValability, hProductPrices) == -1) { //error at opening
        CloseHandle(hMarketShelves);
        CloseHandle(hMarketValability);
        CloseHandle(hProductPrices);
        return -1;
    }*/
    std::vector<ProductInfo> FirstFileProducts = this->processLines(firstFile);

    //open the memory mapped files
    /*HANDLE hMarketShelves = getHandleForMappedFile(marketShelves);
    if (hMarketShelves == NULL) {
        DWORD error = GetLastError();
        std::cerr << "Error opening MarketShelves Mapped File!" << error << std::endl;
        return 0;
    }
    HANDLE hMarketValability = getHandleForMappedFile(marketValability);
    if (hMarketValability == NULL) {
        DWORD error = GetLastError();
        std::cerr << "Error opening MarketValability Mapped File!" << error << std::endl;
        CloseHandle(hMarketShelves);
        return 0;
    }
    HANDLE hProductPrices = getHandleForMappedFile(productPrices);
    if (hProductPrices == NULL) {
        DWORD error = GetLastError();
        std::cerr << "Error opening Product Prices Mapped File!" << error << std::endl;
        CloseHandle(hMarketValability);
        CloseHandle(hMarketShelves);
        return 0;
    }*/

    /*for (auto& product : FirstFileProducts) {
        std::cout << "Product: " << product.getIdProduct() << ", " << product.getExpiresIn() << ", "
            << product.getShelveId() << ", " << product.getProductPrice() << std::endl;
    }*/
    //to add to the memory mapped files


    //close handles
    CloseHandle(firstFile);
    //CloseHandle(hMarketShelves);
    //CloseHandle(hMarketValability);
    //CloseHandle(hProductPrices);
    return 1;
}

void Deposit::startProccessing(LPCSTR directoryDeposit) {

    // 1. Open Mutex, Events and Timer
    HANDLE hMutexCriticalSection = OpenMutex(SYNCHRONIZE, FALSE, CRITICAL_SECTION_MUTEX);
    HANDLE hMasterEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, MASTER_EVENT); // for the master event
    HANDLE hDepositEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, DEPOSIT_EVENT); // for the deposit event
    HANDLE hSoldEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, SOLD_EVENT); // for the sold event
    HANDLE hDonationEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, DONATION_EVENT); // for the donation event
    HANDLE hEventFirstFile = OpenEvent(EVENT_MODIFY_STATE, FALSE, FIRST_DAY_EVENT);
    HANDLE hDepositEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, DEPOSIT_EVENT); // for the deposit event
    HANDLE hCriticalError = OpenEvent(EVENT_MODIFY_STATE, FALSE, ABORT_EVENT);


    // 1. return a vector with the name of files
    vector<string> filesInOrder = this->preprocessingFiles(directoryDeposit);

    if (!filesInOrder.empty()) { // process the first file
        string& firstFileName = filesInOrder.front();
        cout << firstFileName << endl;
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
    }

    // 1. Open the mutex for critical section


    for (auto it = filesInOrder.begin() + 1; it != filesInOrder.end(); ++it) {
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
            std::cerr << "Error opening the first file to parse!" << std::endl;
            ExitProcess(0);
        }

        ReleaseMutex(hMutexCriticalSection);

        // signals master that deposit.exe finished processing 1st day
        SetEvent(hDepositEvent);

        cout << "Am terminat al doilea file, acum astept un input ca sa simulez wait! Acesta este file-ul: " << fileName << endl;
        WaitForSingleObject(hMasterEvent, INFINITE); // Waits signal from master until all silblings finish to process the first day.
        cout << "Am ramas blocat!" << endl;
    }
}
