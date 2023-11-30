#include "Sold.h"


ProductInfo Sold::processLineAndGetProduct(const char* line){
    int shelve_id;
    if (sscanf_s(line, "%d", &shelve_id) == 4) {
        return ProductInfo(shelve_id);
    }
    return ProductInfo(-1);
}

vector<ProductInfo> Sold::processLines(HANDLE hFile) {
    vector<ProductInfo> products;
    char buffer[1024];
    DWORD bytesRead;
    BOOL readResult;
    string currentLine;

    while (true) {
        // Read file content into the buffer
        readResult = ReadFile(hFile, buffer, sizeof(buffer) - 1, &bytesRead, NULL);

        // Check for end of file or error
        if (!readResult || bytesRead == 0) {
            break;
        }

        // Null-terminate the buffer
        buffer[bytesRead] = '\0';

        // Process buffer line by line
        istringstream stream(buffer);
        string line;
        while (getline(stream, line)) {
            if (line.empty()) continue; // Skip empty lines

            // Convert line to DWORD
            istringstream lineStream(line);
            DWORD value;
            if (lineStream >> value) {
                ProductInfo product = ProductInfo(value);
                products.push_back(product);
            }
        }
    }

    return products;
}


int Sold::proccessFile(LPCSTR firstFilePath)
{
    stringstream ss;

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
        cerr << "SOLD " << "Error opening file: " << firstFilePath << ". Error code: " << to_string(GetLastError()) << endl;
        return 0;
    }
    vector<ProductInfo> FirstFileProducts = this->processLines(hFile);

    //open the memory mapped files
    HANDLE hMarketShelves = getHandleForMappedFile(marketShelves);
    if (hMarketShelves == NULL) {
        cerr << "SOLD " << "Error opening MarketShelves Mapped File!" << to_string(GetLastError()) << endl;
        return 0;
    }
    HANDLE hMarketValability = getHandleForMappedFile(marketValability);
    if (hMarketValability == NULL) {
        DWORD error = GetLastError();
        cerr << "SOLD "  "Error opening hMarketValability Mapped File!" << to_string(GetLastError()) << endl;
        CloseHandle(hMarketShelves);
        return 0;
    }
    HANDLE hProductPrices = getHandleForMappedFile(productPrices);
    if (hProductPrices == NULL) {
        DWORD error = GetLastError();
        cerr << "SOLD " << "Error opening hMarketValability Mapped File!" << to_string(GetLastError()) << endl;
        CloseHandle(hMarketValability);
        CloseHandle(hMarketShelves);
        return 0;
    }

    //// Opening memory mapped files
    LPVOID pViewShelves = MapViewOfFile(hMarketShelves, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    LPVOID pViewValability = MapViewOfFile(hMarketValability, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    LPVOID pViewPrices = MapViewOfFile(hProductPrices, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    if (pViewShelves == NULL) {
        cerr << "SOLD " << "Error mapping view of file. Error code: " << to_string(GetLastError()) << endl;
        CloseHandle(hMarketShelves);
        CloseHandle(hMarketValability);
        CloseHandle(hProductPrices);
        return 0;
    }
    if (pViewValability == NULL) {
        cerr << "SOLD " << "Error mapping view of file. Error code: " << GetLastError() << endl;
        CloseHandle(hMarketShelves);
        CloseHandle(hMarketValability);
        CloseHandle(hProductPrices);
        UnmapViewOfFile(pViewShelves);
        return 0;
    }
    if (pViewPrices == NULL) {
        cerr << "SOLD " << "Error mapping view of file. Error code: " << GetLastError() << endl;
        CloseHandle(hMarketShelves);
        CloseHandle(hMarketValability);
        CloseHandle(hProductPrices);
        UnmapViewOfFile(pViewShelves);
        UnmapViewOfFile(pViewValability);
        return 0;
    }

    //// Opening Log file and error file
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
        cerr << "SOLD " << "Error opening LOGS file. Error code: " << GetLastError() << endl;
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
            cerr << "SOLD " << "Error opening handleMappedFiles product. Error code: " << GetLastError() << endl;
            Sleep(10000);
            this->closeHandle(LOGS_FILE);
            
        // unmap view of file
            UnmapViewOfFile(pViewPrices);
            UnmapViewOfFile(pViewShelves);
            UnmapViewOfFile(pViewValability);


            return 0;
        }
    }

    ////close handles for memory mapping files and current file
    CloseHandle(hFile);
    CloseHandle(hMarketShelves);
    CloseHandle(hMarketValability);
    CloseHandle(hProductPrices);

    //// closing Logs and Error file
    this->closeHandle(LOGS_FILE);
    
    //// unmap view of file
    UnmapViewOfFile(pViewPrices);
    UnmapViewOfFile(pViewShelves);
    UnmapViewOfFile(pViewValability);

    if (!this->writeTotalSales()) {
        // closing Logs and Error file
        cerr << "SOLD " << "Error writing to sold file. Error code: " << GetLastError() << endl;
        return 0;
    }

    return 1;
}

DWORD Sold::handleMappedFiles(LPVOID pViewShelves, LPVOID pViewValability, LPVOID pViewPrices, ProductInfo& product)
{
    DWORD shelve_id = product.getShelveId();

    // Cast memory mapped views to DWORD arrays
    DWORD* shelves = static_cast<DWORD*>(pViewShelves);
    DWORD* valability = static_cast<DWORD*>(pViewValability);
    DWORD* prices = static_cast<DWORD*>(pViewPrices);

    // Check if shelve_id is valid and the shelf exists
    if (shelves[shelve_id] == 0xFFFFFFFF) {
        this->createOrOpenFiles(ERRORS_FILE);
        stringstream errorsFile;
        errorsFile << "S-a incercat vanzarea unui produs de pe un raft " << shelve_id << " ce nu contine produs\r\n";
        this->appendToFile(ERRORS_FILE, errorsFile.str());
        this->closeHandle(ERRORS_FILE);
        return 1; // Continue processing other lines
    }

    DWORD id_produs = shelves[shelve_id];
    if (valability[id_produs] != 0) {
        stringstream ss;
        ss << "S-a vandut produsul " << id_produs << " de pe raftul " << shelve_id << " cu "
            << valability[id_produs] << " zile inainte de a fi donat si cu pretul de " << prices[id_produs] << "\r\n";

        this->appendToFile(LOGS_FILE, ss.str());
       

        // Accumulate the total sales amount instead of writing to sold.txt here
        totalSales += prices[id_produs];

        // Set memory mapped file values
        valability[id_produs] = 0xFFFFFFFF;
        prices[id_produs] = 0xFFFFFFFF;
        shelves[shelve_id] = 0xFFFFFFFF;

    }

    return 1;

}

DWORD Sold::writeTotalSales() {
    HANDLE hFile = CreateFile(
        this->getPathFor(SOLD_FILE),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_ALWAYS, // Open existing file or create if it does not exist
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        return 0;
    }

    int sold = 0;
    DWORD bytesRead;
    DWORD bytesWritten;

    if (!ReadFile(hFile, &sold, sizeof(int), &bytesRead, NULL) || bytesRead != sizeof(int)) {
        CloseHandle(hFile);
        return 0;
    }

    sold += totalSales;

    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

    if (!WriteFile(hFile, &sold, sizeof(int), &bytesWritten, NULL) || bytesWritten != sizeof(int)) {
        CloseHandle(hFile);
        return 0;
    }

    CloseHandle(hFile);
    totalSales = 0;

    return 1;
}
