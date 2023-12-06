#include "FileManipulation.h"


/**
* Getter for retrieving the handle corresponding to fileType
*/
HANDLE FileManipulation::getHandle(DWORD fileType) {
	switch (fileType)
	{
	case LOGS_FILE: return hLogs;
	case DONATION_FILE: return hDonation;
	case SOLD_FILE : return hSold;
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
* Open a file if exists.
* 
* @return 2 - if file is not found
* @return 0 - if any other error
* @return 1 - in case of success
*/
DWORD FileManipulation::openIfExists(DWORD fileType) {
	HANDLE fileHandler = CreateFile(
		this->getPathFor(fileType),
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING, // OPEN IF EXISTS
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (fileHandler == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {
			this->setHandle(fileType, fileHandler);
			return 2;
		}
		return 0;
	}

	this->setHandle(fileType, fileHandler);

	return 1;
}

/**
* Method responsible for deciding from which file to read and return text to console. Sold file or donation file should be pass to read from both.
*/
string FileManipulation::readFromFile(DWORD fileType) {

	switch (fileType)
	{
	case ERRORS_FILE: return processErrorFile(fileType);
	default: return processSuccessFile();
	}
}

/**
* Method responsible for readinng from error file
*/
string FileManipulation::processErrorFile(DWORD fileType){
	HANDLE fileHandler = this->getHandle(fileType);
	string buffer;
	DWORD bytesRead;
	CHAR readBuffer[4096]; // read the file 4kb by 4kb

	while (true) {
		BOOL readResult = ReadFile(
			fileHandler,
			readBuffer,
			sizeof(readBuffer) - sizeof(WCHAR), // null terminator
			&bytesRead,
			NULL
		);

		if (!readResult || bytesRead == 0) {
			break; //error or end of file
		}

		readBuffer[bytesRead / sizeof(WCHAR)] = L'\0'; // add null terminate
		buffer.append(readBuffer);
	}

	CloseHandle(this->getHandle(fileType));

	return buffer;
}

/**
* Method responsible for readinng from donation and sold file
*/
string FileManipulation::processSuccessFile() {
	DWORD res;
	if ((res = openIfExists(DONATION_FILE)) == 0 || res == 2)
	{
		return "Error from opening DON " + to_string(GetLastError());
	}

	if ((res = openIfExists(SOLD_FILE)) == 0 || res == 2)
	{
		return "Error from opening SOLD " + to_string(GetLastError());
	}


	HANDLE donationHandle = this->getHandle(DONATION_FILE);
	HANDLE soldHandle = this->getHandle(SOLD_FILE);

	DWORD bytesRead;
	INT32 donatedItems = 0;
	INT32 soldItems = 0;

	if (!ReadFile(
		donationHandle,
		&donatedItems,
		sizeof(INT32),
		&bytesRead,
		NULL
	)) {
		return "Error from reading DON " + to_string(GetLastError());
	}

	if (!ReadFile(
		soldHandle,
		&soldItems,
		sizeof(INT32),
		&bytesRead,
		NULL
	)) {
		return "Error from reading SOLD " + to_string(GetLastError());;
	}

	stringstream ss;

	ss << "Number of sold items: " << to_string(soldItems) << "\r\n";
	ss << "Number of donated items: " << to_string(donatedItems) << "\r\n";

	CloseHandle(donationHandle);
	CloseHandle(soldHandle);

	return ss.str();
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
* Closes handle to fileType
*/
void FileManipulation::closeHandle(DWORD fileType) {
	CloseHandle(this->getHandle(fileType));
}
