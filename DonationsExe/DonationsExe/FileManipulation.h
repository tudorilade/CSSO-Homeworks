#pragma once
#include <Windows.h>
#include <iostream>
#include <string.h>


using namespace std;

#define LOGS_FILE 1
#define ERRORS_FILE 2
#define DONATION_FILE 3
#define SOLD_FILE 4


class FileManipulation
{

private:
	LPCSTR PATH_TO_LOGS = "C:\\Facultate\\CSSO\\Week4\\Reports\\logs.txt";
	HANDLE hLogs;

	LPCSTR PATH_TO_ERRORS = "C:\\Facultate\\CSSO\\Week4\\Reports\\errors.txt";
	HANDLE hErrors;

	LPCSTR PATH_TO_DONATION = "C:\\Facultate\\CSSO\\Week4\\Reports\\Summary\\donations.txt";
	HANDLE hDonation;

	LPCSTR PATH_TO_SOLD = "C:\\Facultate\\CSSO\\Week4\\Reports\\Summary\\sold.txt";
	HANDLE hSold;

public:

	HANDLE getHandle(DWORD);
	void setHandle(DWORD, HANDLE);
	LPCSTR getPathFor(DWORD);
	DWORD createOrOpenFiles(DWORD);
	DWORD writeToFile(DWORD, string);
	void closeHandle(DWORD);
	DWORD appendToFile(DWORD, string);
	DWORD openExistingFile(DWORD);
};
