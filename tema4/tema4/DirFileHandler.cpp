#include "DirFileHandler.h"


/**
* Method responsible for setting the path to sold or deposit directory
*/
DWORD DirFileHandler::setPathDir(wstring path, DWORD typeOfDir)
{
	switch (typeOfDir)
	{
	case DEPOSIT_DIR:
		this->pathToDeposit = path + L"\\*";
		return 1;
	case SOLD_DIR:
		this->pathToSold = path + L"\\*";
		return 1;
	default:
		return 0;
	}
}

wstring DirFileHandler::getPathDir(DWORD typeOfDir) {
	switch (typeOfDir) {
	case DEPOSIT_DIR: return this->pathToDeposit;
	case SOLD_DIR: return this->pathToSold;
	}
	return L"";
}

/**
* Method responsible for retrieving the final content of the requested files
*/
wstring DirFileHandler::getContentForConsole()
{

    FileManipulation fHandler = FileManipulation();

    //if (fHandler.openIfExists(ERRORS_FILE) == 2)
    //{
    //    return fHandler.readFromFile(ERRORS_FILE);
    //}

	return fHandler.readFromFile(DONATION_FILE);
}

/**
* Method responsible for setting up the files and directories for homework 4
* 
* Succesful setup is one in which all requested files and directories have been created successfully; (Atomic)
* @return 1 in case of success; 0 otherwise
*/
DWORD DirFileHandler::setUpDirAndFiles()
{
    // create week lab directory
    if (!recursiveCreateDirectory(WEEK_LAB_PATH)) {
        return FALSE;
    }

    // create summary + reports directories
    if (!recursiveCreateDirectory(SUMMARY_DIR_PATH)) {
        return FALSE;
    }

    if (!createFile(SOLD_FILE_PATH)) {
        return FALSE;
    }

    if (!createFile(DONATIONS_FILE_PATH)) {
        return FALSE;
    }
    
    // All directories and files were successfully created ! when a file fails to be created, 
    // execution stops and prints the error
	return 1;

}

/**
* Method responsible for retrieving the last error that has occured
* 
* @return a string representing the error
*/
wstring DirFileHandler::getLastError()
{
	
	return this->lastError;
}

/**
 * @brief Handles errors while creating a directory. If the directory exists,
 *        the method does not treat it as a failure since the directory exists.
 *
 * @param directoryPath The path of the directory.
 * @param lastError The error code from the directory creation attempt.
 * @return FALSE if the error is ERROR_ALREADY_EXISTS, TRUE otherwise.
 */
BOOL DirFileHandler::creationOfFileAndDirs(LPCSTR path, DWORD lastError) {

    if (lastError == ERROR_ALREADY_EXISTS) {
        return TRUE;
    }
    else {
        wstring widePath = this->ConvertToWString(path);

        // Compose the error message
        this->lastError = L"Error when trying to create: " + widePath + L" Error code: " + std::to_wstring(lastError);
        return FALSE;
    }
}

/**
* @brief Method responsible for creating recursively a directory hierarchy starting from an input path.
* Example: if the input path is C:Facultate/CSSO/Laboratoare/Week4, it starts with it an recursively creates
* the directory until the root (C:). It stops where finds the first existing directory.
*
* @param directoryPath: the current path of the directory
* @return TRUE if the directory hierarchy was successfully created, FALSE otherwise
*/
BOOL DirFileHandler::recursiveCreateDirectory(LPCSTR directoryPath) {
    DWORD lastError;
    if (CreateDirectory(directoryPath, NULL) || (lastError = GetLastError()) == ERROR_ALREADY_EXISTS) {
        // If we managed to create a directory or it already exists, we exit
        return TRUE;
    }

    if (lastError == ERROR_PATH_NOT_FOUND) {
        // If path is not found, we try to create it recursevly
        char parentDir[MAX_PATH];

        // get the parentDirectory of directoryPath
        // (e.g. parrent of C:\\Facultate\\CSSO\\Laboratoare\\Week4 is C:\\Facultate\\CSSO\\Laboratoare)
        strncpy_s(parentDir, directoryPath, strrchr(directoryPath, '\\') - directoryPath);
        if (recursiveCreateDirectory(parentDir)) {
            if (CreateDirectory(directoryPath, NULL)) {
                return TRUE;
            }
        }
    }
    return creationOfFileAndDirs(directoryPath, lastError);
}


/**
* @brief Method responsible for creating the "sumar.txt" file.
*
* @return TRUE if sumar.txt was created and sets sumarFile inside class. If the file exists, it resets the content inside.
    FALSE otherwise
*/
BOOL DirFileHandler::createFile(LPCSTR path) {
    HANDLE fileHandler = CreateFile(
        path, // the filename
        GENERIC_WRITE, // open for write
        0,
        NULL,
        CREATE_ALWAYS, // open file always if exists or not and truncate the content
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (fileHandler == INVALID_HANDLE_VALUE) {
        // file couldn't be created; stop execution
        return this->creationOfFileAndDirs(path, GetLastError());
    }

    DWORD bytesWritten;
    LPCSTR bufferToWrite = "0";
    if (!WriteFile(fileHandler, bufferToWrite, strlen(bufferToWrite), &bytesWritten, NULL))
    {   
        // Initialize the files with 0
        wstring widePath = ConvertToWString(path);
        this->lastError = L"Couldn't write to " + widePath + L". Error code: " + to_wstring(GetLastError()) + L"\r\n";
        return FALSE;

    }

    CloseHandle(fileHandler);

    return TRUE;
}


/**
* Helper method responsible for converting any LPCSTR to wide string wstring
*/
wstring DirFileHandler::ConvertToWString(LPCSTR str) {
    if (str == nullptr) {
        return std::wstring();
    }

    int length = MultiByteToWideChar(CP_ACP, 0, str, -1, nullptr, 0);
    if (length == 0) {
        return std::wstring(); 
    }

    std::wstring wideStr(length, L'\0');

    MultiByteToWideChar(CP_ACP, 0, str, -1, &wideStr[0], length);

    wideStr.pop_back();

    return wideStr;
}


/**
* Method responsible for creating a mapping file to specific zones
*
* @return non-zero value in case of success; zero otherwise and closes all handles
*/
DWORD DirFileHandler::createMemoryMapping()
{

    this->hMapShelves = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        this->MAPPING_MEMORY_SIZE,
        marketShelves
    );

    if (this->hMapShelves == NULL) {
        return FALSE;
    }

    this->hMapValability = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        this->MAPPING_MEMORY_SIZE,
        marketValability
    );

    if (this->hMapValability == NULL) {
        CloseHandle(this->hMapShelves);
        return FALSE;
    }

    this->hMapPrices = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        this->MAPPING_MEMORY_SIZE,
        productPrices
    );

    if (this->hMapPrices == NULL) {
        CloseHandle(this->hMapShelves);
        CloseHandle(this->hMapValability);
        return FALSE;
    }

    LPDWORD bMapShelves = (LPDWORD)MapViewOfFile(this->hMapShelves, FILE_MAP_WRITE, 0, 0, this->MAPPING_MEMORY_SIZE);
    if (bMapShelves == NULL) {
        CloseHandle(this->hMapShelves);
        return FALSE;
    }
    memset(bMapShelves, 0xFF, this->MAPPING_MEMORY_SIZE);

    LPDWORD bMapValability = (LPDWORD)MapViewOfFile(this->hMapValability, FILE_MAP_WRITE, 0, 0, this->MAPPING_MEMORY_SIZE);
    if (bMapValability == NULL) {
        CloseHandle(this->hMapShelves);
        CloseHandle(this->hMapValability);
        return FALSE;
    }
    memset(bMapValability, 0xFF, this->MAPPING_MEMORY_SIZE);

    LPDWORD bMapPrices = (LPDWORD)MapViewOfFile(this->hMapPrices, FILE_MAP_WRITE, 0, 0, this->MAPPING_MEMORY_SIZE);
    if (bMapPrices == NULL) {
        CloseHandle(this->hMapShelves);
        CloseHandle(this->hMapValability);
        CloseHandle(this->hMapPrices);
        return FALSE;
    }
    memset(bMapPrices, 0xFF, this->MAPPING_MEMORY_SIZE);

    return TRUE;
}

/**
* Method responsible for closing the memory handles
*/
void DirFileHandler::closeMemoryHandles()
{
    CloseHandle(hMapShelves);
    CloseHandle(hMapValability);
    CloseHandle(hMapPrices);
}

/**
* Method responsible for getting the path to correct executable
* 
*/
LPCSTR DirFileHandler::getPathToExecutable(DWORD executable) {
    switch (executable)
    {
    case DONATE_EXE: return this->PATH_TO_DONATE_EXE;
    case SOLD_EXE: return this->PATH_TO_SOLD_EXE;
    default: return this->PATH_TO_DEPOSIT_EXE;
    }
}

/**
* Formats a CLI argument using a path to executable
*/
wstring DirFileHandler::getCLIArguments(LPCSTR pathToExe)
{
    wstring cmdLineArgs = L"\"" + ConvertToWString(pathToExe) + L"\" \"" +
        getPathDir(DEPOSIT_DIR) + L"\" \"" +
        getPathDir(SOLD_DIR) + L"\"";

    return cmdLineArgs;
}