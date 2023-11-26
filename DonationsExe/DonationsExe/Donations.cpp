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
        cerr << "Donations " << "Error opening MarketShelves Mapped File!" << std::to_string(GetLastError()) << std::endl;
        return 0;
    }
    HANDLE hMarketValability = getHandleForMappedFile(marketValability);
    if (hMarketValability == NULL) {
        DWORD error = GetLastError();
        cerr << "Donations " << "Error opening hMarketValability Mapped File!" << std::to_string(GetLastError()) << std::endl;
        CloseHandle(hMarketShelves);
        return 0;
    }
    HANDLE hProductPrices = getHandleForMappedFile(productPrices);
    if (hProductPrices == NULL) {
        DWORD error = GetLastError();
        cerr << "Donations " << "Error opening hMarketValability Mapped File!" << std::to_string(GetLastError()) << std::endl;
        CloseHandle(hMarketValability);
        CloseHandle(hMarketShelves);
        return 0;
    }

    // Opening memory mapped files
    LPVOID pViewShelves = MapViewOfFile(hMarketShelves, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    LPVOID pViewValability = MapViewOfFile(hMarketValability, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    LPVOID pViewPrices = MapViewOfFile(hProductPrices, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    if (pViewShelves == NULL) {
        cerr << "Error mapping view of file. Error code: " << std::to_string(GetLastError()) << std::endl;
        CloseHandle(hMarketShelves);
        CloseHandle(hMarketValability);
        CloseHandle(hProductPrices);
        return 0;
    }
    if (pViewValability == NULL) {
        cerr << "Error mapping view of file. Error code: " << GetLastError() << std::endl;
    
        CloseHandle(hMarketShelves);
        CloseHandle(hMarketValability);
        CloseHandle(hProductPrices);
        UnmapViewOfFile(pViewShelves);

        return 0;
    }
    if (pViewPrices == NULL) {
        cerr << "Error mapping view of file. Error code: " << GetLastError() << std::endl;
        
        CloseHandle(hMarketShelves);
        CloseHandle(hMarketValability);
        CloseHandle(hProductPrices);
        UnmapViewOfFile(pViewShelves);
        UnmapViewOfFile(pViewValability);

        return 0;
    }

    if (this->createOrOpenFiles(LOGS_FILE) == 0)
    {
        CloseHandle(hMarketShelves);
        CloseHandle(hMarketValability);
        CloseHandle(hProductPrices);
        UnmapViewOfFile(pViewShelves);
        UnmapViewOfFile(pViewValability);
        UnmapViewOfFile(pViewPrices);
        return 0;
    }

    int number = 0; // total donations

    DWORD* valabilityArray = static_cast<DWORD*>(pViewValability);
    DWORD* pricesArray = static_cast<DWORD*>(pViewPrices);
    DWORD* shelvesArray = static_cast<DWORD*>(pViewShelves);
    size_t arraySize = 10000;
    
    for (size_t i = 0; i < arraySize; ++i) {
        if (valabilityArray[i] == 0) {
            number = number + pricesArray[i];
            ss << "Produsul cu id-ul : " << i << "a fost donat " << endl;
            this->appendToFile(LOGS_FILE, ss.str());
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

    this->total = number;

    if (!this->writeTotalSales()) {
        cerr << "Donations " << "Error writing to sold file. Error code: " << GetLastError() << endl;
        return 0;
    }

    CloseHandle(hMarketShelves);
    CloseHandle(hMarketValability);
    CloseHandle(hProductPrices);

    // closing Logs and Error file
    this->closeHandle(LOGS_FILE);

    // unmap view of file
    UnmapViewOfFile(pViewPrices);
    UnmapViewOfFile(pViewShelves);
    UnmapViewOfFile(pViewValability);
    return 1;
}

DWORD Donations::writeTotalSales() {
    HANDLE hFile = CreateFile(
        this->getPathFor(DONATION_FILE),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING, // Overwrite existing file
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        // Handle error
        return 0;
    }

    DWORD sold;
    DWORD bytesWritten;

    ReadFile(hFile, &sold, sizeof(DWORD), &bytesWritten, NULL);

    sold += total;

    WriteFile(hFile, &sold, sizeof(DWORD), &bytesWritten, NULL);
    CloseHandle(hFile);
    total = 0;
    return 1;
}
