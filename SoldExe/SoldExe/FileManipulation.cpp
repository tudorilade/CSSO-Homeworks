#include "FileManipulation.h"


/**
* Getter for retrieving the handle corresponding to fileType
*/
HANDLE FileManipulation::getHandle(DWORD fileType) {
	switch (fileType)
	{
	case LOGS_FILE: return hLogs;
	case DONATION_FILE: return hDonation;
	case SOLD_FILE: return hSold;
	default: return hErrors;
	}
}


/**
* Getter for retrieving the handle corresponding to fileType
*/
LPCSTR FileManipulation::getPathFor(DWORD fileType) {
	switch (fileType)
	{
	case LOGS_FILE: return PATH_TO_LOGS;
	case DONATION_FILE: return PATH_TO_DONATION;
	case SOLD_FILE: return PATH_TO_SOLD;
	default: return PATH_TO_ERRORS;
	}
}

/**
* Method responsible for setting the handle to correct fileType
*/
void FileManipulation::setHandle(DWORD fileType, HANDLE handle) {
	switch (fileType)
	{
	case LOGS_FILE: hLogs = handle; break;
	case DONATION_FILE: hDonation = handle; break;
	case SOLD_FILE: hSold = handle; break;
	default: hErrors = handle; break;
	}
}


/**
* Method responsible for creating, opening and setting the handle corresponding to fileType
*
* @param pathToFile - path to file
* @param fileType - the type of file to create (errors.txt, logs.txt etc.)
* @return non-zero for success; 0 otherwise
*/
DWORD FileManipulation::createOrOpenFiles(DWORD fileType) {
	HANDLE fileHandler = CreateFile(
		this->getPathFor(fileType), // the filename
		GENERIC_WRITE, // open for write
		0,
		NULL,
		CREATE_ALWAYS, // open file always if exists or not and truncate the content
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (fileHandler == INVALID_HANDLE_VALUE) {
		return 0;
	}

	this->setHandle(fileType, fileHandler);

	return 1;
}

/**
* Method responsible for writing a buffer to a targetHandle
*
* @return non-zero for success; 0 otherwise
*/
DWORD FileManipulation::writeToFile(DWORD fileType, string buffer) {

	DWORD bytesWritten;

	if (!WriteFile(this->getHandle(fileType), buffer.c_str(), buffer.size(), &bytesWritten, NULL))
	{
		// Initialize the files with 0
		return 0;

	}

	if (buffer.size() != bytesWritten)
	{
		return 0;
	}

	return 1;
}


/**
* Method responsible for appending a buffer to a targetHandle
*
* @return non-zero for success; 0 otherwise
*/
DWORD FileManipulation::appendToFile(DWORD fileType, string buffer) {

	SetFilePointer(this->getHandle(fileType), 0, NULL, FILE_END);

	DWORD bytesWritten;

	if (!WriteFile(this->getHandle(fileType), buffer.c_str(), buffer.size(), &bytesWritten, NULL))
	{
		// Initialize the files with 0
		return 0;
	}

	if (buffer.size() != bytesWritten)
	{
		return 0;
	}

	return 1;
}



/**
* Closes handle to fileType
*/
void FileManipulation::closeHandle(DWORD fileType) {
	CloseHandle(this->getHandle(fileType));
}
