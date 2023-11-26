#include "Donations.h"
ProductInfo Donations::processLineAndGetProduct(const char* line) {
    return ProductInfo(1, 1, 1, 1);
}
/*
Process the memory mapped files to calculate the donations
*/
int Donations::proccessFile(LPCSTR filePath) {    
    std::stringstream ss;
    HANDLE hMarketShelves = getHandleForMappedFile(marketShelves);
    if (hMarketShelves == NULL) {
        ss << "Error opening MarketShelves Mapped File!" << std::to_string(GetLastError()) << std::endl;
        this->appendToFile(ERRORS_FILE, ss.str());
        return 0;
    }
    HANDLE hMarketValability = getHandleForMappedFile(marketValability);
    if (hMarketValability == NULL) {
        DWORD error = GetLastError();
        ss << "Error opening hMarketValability Mapped File!" << std::to_string(GetLastError()) << std::endl;
        this->appendToFile(ERRORS_FILE, ss.str());
        CloseHandle(hMarketShelves);
        return 0;
    }
    HANDLE hProductPrices = getHandleForMappedFile(productPrices);
    if (hProductPrices == NULL) {
        DWORD error = GetLastError();
        ss << "Error opening hMarketValability Mapped File!" << std::to_string(GetLastError()) << std::endl;
        this->appendToFile(ERRORS_FILE, ss.str());
        CloseHandle(hMarketValability);
        CloseHandle(hMarketShelves);
        return 0;
    }

    // Opening memory mapped files
    LPVOID pViewShelves = MapViewOfFile(hMarketShelves, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    LPVOID pViewValability = MapViewOfFile(hMarketValability, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    LPVOID pViewPrices = MapViewOfFile(hProductPrices, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    if (pViewShelves == NULL) {
        ss << "Error mapping view of file. Error code: " << std::to_string(GetLastError()) << std::endl;
        this->appendToFile(ERRORS_FILE, ss.str());

        CloseHandle(hMarketShelves);
        CloseHandle(hMarketValability);
        CloseHandle(hProductPrices);
        this->closeHandle(ERRORS_FILE);
        return 0;
    }
    if (pViewValability == NULL) {
        ss << "Error mapping view of file. Error code: " << GetLastError() << std::endl;
        this->appendToFile(ERRORS_FILE, ss.str());

        CloseHandle(hMarketShelves);
        CloseHandle(hMarketValability);
        CloseHandle(hProductPrices);
        this->closeHandle(ERRORS_FILE);
        UnmapViewOfFile(pViewShelves);

        return 0;
    }
    if (pViewPrices == NULL) {
        ss << "Error mapping view of file. Error code: " << GetLastError() << std::endl;
        this->appendToFile(ERRORS_FILE, ss.str());

        CloseHandle(hMarketShelves);
        CloseHandle(hMarketValability);
        CloseHandle(hProductPrices);
        this->closeHandle(ERRORS_FILE);
        UnmapViewOfFile(pViewShelves);
        UnmapViewOfFile(pViewValability);

        return 0;
    }
    if (this->createOrOpenFiles(DONATION_FILE) == 0) {
        return 0;
        CloseHandle(hMarketShelves);
        CloseHandle(hMarketValability);
        CloseHandle(hProductPrices);
        this->closeHandle(ERRORS_FILE);
        UnmapViewOfFile(pViewShelves);
        UnmapViewOfFile(pViewValability);
        UnmapViewOfFile(pViewPrices);
    }
        
    if (this->createOrOpenFiles(LOGS_FILE) == 0)
    {
        return 0;
        CloseHandle(hMarketShelves);
        CloseHandle(hMarketValability);
        CloseHandle(hProductPrices);
        this->closeHandle(ERRORS_FILE);
        UnmapViewOfFile(pViewShelves);
        UnmapViewOfFile(pViewValability);
        UnmapViewOfFile(pViewPrices);
    }

    const DWORD bufferSize = 1024;
    char buffer[bufferSize];
    DWORD dwBytesRead;
    int number = 0; // Initialize the integer to 0

    if (ReadFile(this->getHandle(DONATION_FILE), buffer, bufferSize - 1, &dwBytesRead, NULL)) {
        buffer[dwBytesRead] = '\0';

        if (dwBytesRead > 0) {
            if (sscanf_s(buffer, "%d", &number) != 1) {

                ss << "Error reading number from file." << std::endl;
                this->appendToFile(ERRORS_FILE, ss.str());

                CloseHandle(hMarketShelves);
                CloseHandle(hMarketValability);
                CloseHandle(hProductPrices);
                this->closeHandle(ERRORS_FILE);
                this->closeHandle(DONATION_FILE);
                UnmapViewOfFile(pViewShelves);
                UnmapViewOfFile(pViewValability);
                UnmapViewOfFile(pViewPrices);
            }
        }
    }
    else {
        ss << "Error reading file: " << std::endl;
        this->appendToFile(ERRORS_FILE, ss.str());

        CloseHandle(hMarketShelves);
        CloseHandle(hMarketValability);
        CloseHandle(hProductPrices);
        this->closeHandle(ERRORS_FILE);
        this->closeHandle(DONATION_FILE);
        UnmapViewOfFile(pViewShelves);
        UnmapViewOfFile(pViewValability);
        UnmapViewOfFile(pViewPrices);
    }



    DWORD* valabilityArray = static_cast<DWORD*>(pViewValability);
    DWORD* pricesArray = static_cast<DWORD*>(pViewPrices);
    DWORD* shelvesArray = static_cast<DWORD*>(pViewShelves);
    size_t arraySize = 10000;

    for (size_t i = 0; i < arraySize; ++i) {
        // Access each element using valabilityArray[i]
        if (valabilityArray[i] == 0) {
            number = number + pricesArray[i];
            this->writeToFile(DONATION_FILE, to_string(number));
            ss << "Produsul cu id-ul : " << i << "a fost donat " << endl;
            this->writeToFile(LOGS_FILE, ss.str());
            valabilityArray[i] = 0xFFFFFFFF;
            pricesArray[i] = 0xFFFFFFFF;
            for (size_t j = 0; j < arraySize; ++j) {
                if (shelvesArray[j] == i) {
                    shelvesArray[j] = 0xFFFFFFFF;
                }
            }
        }
        else if (valabilityArray[i] > 0 && valabilityArray[i] < 0xFFFFFFFF) {
            valabilityArray[i] = valabilityArray[i] - 1;
        }
    }

    CloseHandle(hMarketShelves);
    CloseHandle(hMarketValability);
    CloseHandle(hProductPrices);

    // closing Logs and Error file
    this->closeHandle(LOGS_FILE);
    this->closeHandle(ERRORS_FILE);
    this->closeHandle(DONATION_FILE);

    // unmap view of file
    UnmapViewOfFile(pViewPrices);
    UnmapViewOfFile(pViewShelves);
    UnmapViewOfFile(pViewValability);
    return 1;
}