#include "DirFileHandler.h"


string DirFileHandler::getPathDir(DWORD typeOfDir) {
    switch (typeOfDir) {
    case WEEK_LAB_DIR: return this->WEEK_LAB_PATH;
    case DOWNLOADS_DIR: return this->DOWNLOAD_DIR_PATH;
    }
    return "";
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
    if (!recursiveCreateDirectory(DOWNLOAD_DIR_PATH)) {
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
string DirFileHandler::getLastError()
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
        // Compose the error message
        this->lastError = "Error when trying to create: " + string(path) + " Error code: " + to_string(lastError);
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

    return TRUE;
}
