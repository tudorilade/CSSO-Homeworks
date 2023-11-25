#include "Deposit.h"
#include <stdio.h>
#include "../../../tema4/tema4/constants.h"
#include <iostream>
#include <vector>



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


int Deposit::CheckForErrors(HANDLE hMarketShelves, HANDLE hMarketValability, HANDLE hProductPrices)
{
    if (hMarketShelves == NULL) {
        DWORD error = GetLastError();
        std::cerr << "Error opening MarketShelves Mapped File!" << error << std::endl;
        return -1;
    }
    if (hMarketValability == NULL) {
        DWORD error = GetLastError();
        std::cerr << "Error opening MarketValability Mapped File!" << error << std::endl;
        return -1;
    }
    if (hProductPrices == NULL) {
        DWORD error = GetLastError();
        std::cerr << "Error opening Product Prices Mapped File!" << error << std::endl;
        return -1;
    }
    return 0;
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



int Deposit::proccessFirstFile(LPCSTR firstFilePath) {
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
        return -1;
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

    for (auto& product : FirstFileProducts) {
        std::cout << "Product: " << product.getIdProduct() << ", " << product.getExpiresIn() << ", "
            << product.getShelveId() << ", " << product.getProductPrice() << std::endl;
    }
    //to add to the memory mapped files

    CloseHandle(firstFile);
    //CloseHandle(hMarketShelves);
    //CloseHandle(hMarketValability);
    //CloseHandle(hProductPrices);
    return 0;
}

void Deposit::startProccessing(LPCSTR directoryDeposit) {
    WIN32_FIND_DATAA fileInfo;
    HANDLE firstFile = FindFirstFile(directoryDeposit, &fileInfo); //get the first file from the directory with deposit
    //prelucrez first
    int k = 1;
    if (firstFile == INVALID_HANDLE_VALUE) {
        cout << "Error opening directory aicisa. Error code: " << GetLastError() << endl;
        return;
    }
    else {
        do {
            if (strcmp(fileInfo.cFileName, ".") == 0) {
                k++;
            }
            else if (strcmp(fileInfo.cFileName, "..") == 0) { // ignore . and .. directories
                k++;
            }
            else if (strcmp(fileInfo.cFileName, "..") != 0 && strcmp(fileInfo.cFileName, ".") != 0 && k == 3) {
                k++;
                cout << directoryDeposit << " " << fileInfo.cFileName << endl;
                if (this->proccessFirstFile(constructPath(directoryDeposit, fileInfo.cFileName)) == -1)
                {
                    std::cerr << "Error opening the first file to parse!" << std::endl;
                    return;
                }
                //set event pentru ca am terminat primul file
            }
            else
                return; //here add the mutex

        } while (FindNextFile(firstFile, &fileInfo));
        if (!FindClose(firstFile)) {
            printf("Error at FindClose.\nError code: %d\n", GetLastError());
        }
    }
    

    //CloseHandle(firstFile);




    /*if (firstFile == INVALID_HANDLE_VALUE) {
        printf("Error at FindFirstFile.\nError code: %d\n", GetLastError());
        return;
    }
    else {



        do {
            HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, CRITICAL_SECTION_MUTEX);

            //memory mapped files that hold info about shelves, valability and prices
            HANDLE hMarketShelves = getHandleForMappedFile(marketShelves);
            HANDLE hMarketValability = getHandleForMappedFile(marketValability);
            HANDLE hProductPrices = getHandleForMappedFile(productPrices);
            //error handling
            if (this->CheckForErrors(hMarketShelves, hMarketValability, hProductPrices) == -1) { //error at opening
                CloseHandle(hMarketShelves);
                CloseHandle(hMarketValability);
                CloseHandle(hProductPrices);
                return;
            }
            if (hMutex == NULL) {
                DWORD error = GetLastError();
                std::cerr << "Error opening mutex: " << error << std::endl;
                return;
            }


            CloseHandle(hMarketShelves);
            CloseHandle(hMarketValability);
            CloseHandle(hProductPrices);
            CloseHandle(hMutex);

        } while (FindNextFile(firstFile, &fileInfo)); // You might want to replace this with a condition based on your logic




        }*/


}