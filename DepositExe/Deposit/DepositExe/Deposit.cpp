#include "Deposit.h"



/**
* Method responsible for translating a line from deposit directory files into a dataclass
*/
ProductInfo Deposit::processLineAndGetProduct(const char* line) {
    
    int id_product, expires_in, shelve_id, product_price;
    if (sscanf_s(line, "%d,%d,%d,%d", &id_product, &expires_in, &shelve_id, &product_price) == 4) {
        return ProductInfo(id_product, expires_in, shelve_id, product_price);
    }
    return ProductInfo(-1, -1, -1, -1);
}

/**
* Method responsible for reading the files in deposit directory, line by line and returns a vector of 
* ProductInfo dataclasses.
*/
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

int Deposit::handleMappedFiles(
    LPVOID pViewShelves,
    LPVOID pViewValability,
    LPVOID pViewPrices,
    ProductInfo productInfo
) {

    DWORD* shelvesArray = static_cast<DWORD*>(pViewShelves);
    if (shelvesArray[productInfo.getShelveId()] == 0xFFFFFFFF) {
        shelvesArray[productInfo.getShelveId()] = productInfo.getIdProduct();
        DWORD* valabilityArray = static_cast<DWORD*>(pViewValability);
        DWORD* pricesArray = static_cast<DWORD*>(pViewPrices);

        valabilityArray[productInfo.getIdProduct()] = productInfo.getExpiresIn();
        pricesArray[productInfo.getIdProduct()] = productInfo.getProductPrice();
        
        string logMsg = "Am adaugat pe raftul" + std::to_string(productInfo.getShelveId())
            + " produsul " + std::to_string(productInfo.getIdProduct()) +
            " ce are o valabilitate de " + std::to_string(productInfo.getExpiresIn())
            + " zile si un pret de " + std::to_string(productInfo.getProductPrice());

        if (this->writeToFile(LOGS_FILE, logMsg) == 0) {
            return 0;
        }
            
    }
    else {

        string errorMsg = "S-a încercat adaugarea produsului " + std::to_string(productInfo.getIdProduct())
            + " pe raftul " + std::to_string(productInfo.getShelveId())
            + " care este deja ocupat de " + to_string(shelvesArray[productInfo.getShelveId()]);
        if (this->writeToFile(ERRORS_FILE, errorMsg) == 0) {
            return 0;
        }
    }
    return 1;

}

/**
* Method responsible for processing a file
*/
int Deposit::proccessFile(LPCSTR firstFilePath) {

    if (this->createOrOpenFiles(ERRORS_FILE) == 0)
        return 0;
       
    std::stringstream ss;

    HANDLE hFile = CreateFile(
        firstFilePath, 
        GENERIC_READ, 
        0, 
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    
    if (hFile == INVALID_HANDLE_VALUE) {
        // Failed to open the file
        ss << "Error opening file: " << firstFilePath << ". Error code: " << std::to_string(GetLastError()) << std::endl;
        this->writeToFile(ERRORS_FILE, ss.str());
        return 0;
    }
    std::vector<ProductInfo> FirstFileProducts = this->processLines(hFile);

    //open the memory mapped files
    HANDLE hMarketShelves = getHandleForMappedFile(marketShelves);
    if (hMarketShelves == NULL) {
        ss << "Error opening MarketShelves Mapped File!" << std::to_string(GetLastError()) << std::endl;
        this->writeToFile(ERRORS_FILE, ss.str());
        return 0;
    }
    HANDLE hMarketValability = getHandleForMappedFile(marketValability);
    if (hMarketValability == NULL) {
        DWORD error = GetLastError();
        ss << "Error opening hMarketValability Mapped File!" << std::to_string(GetLastError()) << std::endl;
        this->writeToFile(ERRORS_FILE, ss.str());
        CloseHandle(hMarketShelves);
        return 0;
    }
    HANDLE hProductPrices = getHandleForMappedFile(productPrices);
    if (hProductPrices == NULL) {
        DWORD error = GetLastError();
        ss << "Error opening hMarketValability Mapped File!" << std::to_string(GetLastError()) << std::endl;
        this->writeToFile(ERRORS_FILE, ss.str());
        CloseHandle(hMarketValability);
        CloseHandle(hMarketShelves);
        return 0;
    }

    // Opening memory mapped files
    LPVOID pViewShelves = MapViewOfFile(hMarketShelves, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    LPVOID pViewValability = MapViewOfFile(hMarketValability, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    LPVOID pViewPrices = MapViewOfFile(hProductPrices, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    if (pViewShelves == NULL) {
        ss <<"Error mapping view of file. Error code: " << std::to_string(GetLastError()) << std::endl;
        this->writeToFile(ERRORS_FILE, ss.str());
        CloseHandle(hMarketShelves);
        CloseHandle(hMarketValability);
        CloseHandle(hProductPrices);
        this->closeHandle(ERRORS_FILE);
        return 0;
    }
    if (pViewValability == NULL) {
        ss << "Error mapping view of file. Error code: " << GetLastError() << std::endl;
        this->writeToFile(ERRORS_FILE, ss.str());
        CloseHandle(hMarketShelves);
        CloseHandle(hMarketValability);
        CloseHandle(hProductPrices);
        this->closeHandle(ERRORS_FILE);
        UnmapViewOfFile(pViewShelves);
        return 0;
    }
    if (pViewPrices == NULL) {
        ss << "Error mapping view of file. Error code: " << GetLastError() << std::endl;
        this->writeToFile(ERRORS_FILE, ss.str());
        CloseHandle(hMarketShelves);
        CloseHandle(hMarketValability);
        CloseHandle(hProductPrices);
        this->closeHandle(ERRORS_FILE);
        UnmapViewOfFile(pViewShelves);
        UnmapViewOfFile(pViewValability);
        return 0;
    }

    // Opening Log file and error file
    if (this->createOrOpenFiles(LOGS_FILE) == 0)
    {
        //close handles for memory mapping files and current file
        CloseHandle(hFile);
        CloseHandle(hMarketShelves);
        CloseHandle(hMarketValability);
        CloseHandle(hProductPrices);

        // unmap view of file
        UnmapViewOfFile(pViewPrices);
        UnmapViewOfFile(pViewShelves);
        UnmapViewOfFile(pViewValability);
        ss << "Error opening LOGS file. Error code: " << GetLastError() << std::endl;
        this->writeToFile(ERRORS_FILE, ss.str());

        // closing Logs and Error file
        this->closeHandle(ERRORS_FILE);

        return 0;
    }

    for (ProductInfo& product : FirstFileProducts) { //add each product to the memory mapped files
        if (this->handleMappedFiles(pViewShelves, pViewValability, pViewPrices, product) == 0)
        {
            //close handles for memory mapping files and current file
            CloseHandle(hFile);
            CloseHandle(hMarketShelves);
            CloseHandle(hMarketValability);
            CloseHandle(hProductPrices);

            // closing Logs and Error file
            ss << "Error opening LOGS file. Error code: " << GetLastError() << std::endl;
            this->writeToFile(ERRORS_FILE, ss.str());

            this->closeHandle(LOGS_FILE);
            this->closeHandle(ERRORS_FILE);

            // unmap view of file
            UnmapViewOfFile(pViewPrices);
            UnmapViewOfFile(pViewShelves);
            UnmapViewOfFile(pViewValability);


            return 0;
        }
    }


    //close handles for memory mapping files and current file
    CloseHandle(hFile);
    CloseHandle(hMarketShelves);
    CloseHandle(hMarketValability);
    CloseHandle(hProductPrices);

    // closing Logs and Error file
    this->closeHandle(LOGS_FILE);
    this->closeHandle(ERRORS_FILE);

    // unmap view of file
    UnmapViewOfFile(pViewPrices);
    UnmapViewOfFile(pViewShelves);
    UnmapViewOfFile(pViewValability);
    return 1;
}
