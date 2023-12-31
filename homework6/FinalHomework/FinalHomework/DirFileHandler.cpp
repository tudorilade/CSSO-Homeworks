#include "DirFileHandler.h"


LPCWSTR DirFileHandler::getPath(DWORD typeOfDir) {
    switch (typeOfDir) {
        case WEEK_LAB_DIR: return this->WEEK_LAB_PATH;
        case IMAGE_DIR: return this->IMAGE_LAB_PATH;
        case SEQUENTIAL_DIR: return this->SEQUENTIAL_LAB_PATH;
        case STATIC_DIR: return this->STATIC_LAB_PATH;
        case DINAMIC_DIR: return this->DINAMIC_LAB_PATH;
        case INFO_FILE: return this->INFO_FILE_PATH;
        case COMPARISON_FILE: return this->COMP_FILE_PATH;
        case GRAPHS_FILE: return this->GRAPHS_FILE_PATH;
    }
    return L"";
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

    if (!recursiveCreateDirectory(IMAGE_LAB_PATH))
    {
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
wstring DirFileHandler::getLastError(){
    if (!lastError.empty()) {
        wstring tmp = move(lastError);
        return tmp;
    }
    return L""; 
}

/**
 * @brief Handles errors while creating a directory. If the directory exists,
 *        the method does not treat it as a failure since the directory exists.
 *
 * @param directoryPath The path of the directory.
 * @param lastError The error code from the directory creation attempt.
 * @return FALSE if the error is ERROR_ALREADY_EXISTS, TRUE otherwise.
 */
BOOL DirFileHandler::creationOfFileAndDirs(LPCWSTR path, DWORD lastError) {

    if (lastError == ERROR_ALREADY_EXISTS) {
        return TRUE;
    }
    else {
        // Compose the error message
        this->lastError = L"Error when trying to create: " + wstring(path) + L" Error code: " + to_wstring(lastError);
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
BOOL DirFileHandler::recursiveCreateDirectory(LPCWSTR directoryPath) {
    DWORD lastError;
    if (CreateDirectoryW(directoryPath, NULL) || (lastError = GetLastError()) == ERROR_ALREADY_EXISTS) {
        // If we managed to create a directory or it already exists, we exit
        return TRUE;
    }

    if (lastError == ERROR_PATH_NOT_FOUND) {
        // If path is not found, we try to create it recursevly
        WCHAR parentDir[MAX_PATH];

        // get the parentDirectory of directoryPath
        // (e.g. parrent of C:\\Facultate\\CSSO\\Laboratoare\\Week4 is C:\\Facultate\\CSSO\\Laboratoare)
        wcsncpy_s(parentDir, directoryPath, wcsrchr(directoryPath, '\\') - directoryPath);
        if (recursiveCreateDirectory(parentDir)) {
            if (CreateDirectoryW(directoryPath, NULL)) {
                return TRUE;
            }
        }
    }
    return creationOfFileAndDirs(directoryPath, lastError);
}

/**
* @brief Method responsible for opening an existing file.
*
* @return a handle to the file
*/
HANDLE DirFileHandler::openFile(LPCWSTR path) {
    HANDLE myConfigHandler = CreateFileW(
        path,
        GENERIC_WRITE | GENERIC_READ,
        NULL,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    return myConfigHandler;
}

/**
* @brief Method responsible for creating the file if it does not exist, or truncates an existing one.
*
* @return a handle to the file*/
HANDLE DirFileHandler::createFile(LPCWSTR path)
{
    HANDLE myConfigHandler = CreateFileW(
        path,
        GENERIC_WRITE | GENERIC_READ,
        NULL,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    return myConfigHandler;
}


/**
* Method responsible for deciding if a handle is invalid, and return the appropiate parameter
*/
DWORD DirFileHandler::fileExistsOrInvalid(HANDLE hFile)
{
    if (hFile == INVALID_HANDLE_VALUE)
    {
        DWORD dwError = GetLastError();
        if (dwError == ERROR_FILE_NOT_FOUND || dwError == ERROR_PATH_NOT_FOUND)
        {
            return 0; // File does not exist
        }
        else
        {
            return 2; // File exists but some other error occurred
        }
    }

    return 1;
}


/**
* Method responsible for writing a buffer of size bufferSize to a handle file
*
* @return true in case of successful writing, FALSE otherwise
*/
BOOL DirFileHandler::writeToFile(HANDLE handle, LPCSTR buffer, DWORD bufferSize) {
    DWORD bytesWritten;
    if (!WriteFile(handle, buffer, bufferSize, &bytesWritten, NULL)) { return FALSE; }
    if (bufferSize != bytesWritten) { return FALSE; }
    return TRUE;
}


/**
* Method responsible for appending a buffer of size bufferSize to the end of a handle file
*
* @return true in case of successful appending, FALSE otherwise
*/
BOOL DirFileHandler::appendToFile(HANDLE handle, LPCSTR buffer, DWORD bufferSize) {
    DWORD bytesWritten;
    SetFilePointer(handle, 0, NULL, FILE_END);
    if (!WriteFile(handle, buffer, bufferSize, &bytesWritten, NULL)) { return FALSE; }
    if (bufferSize != bytesWritten) { return FALSE; }
    return TRUE;
}
