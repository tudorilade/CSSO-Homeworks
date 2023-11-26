#pragma once
#include <Windows.h>
#include <wchar.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include "constants.h"
#include "FileManipulation.h"


#define DEPOSIT_DIR 1
#define SOLD_DIR 2
#define DONATE_EXE 1
#define SOLD_EXE 2
#define DEPOSIT_EXE 3

using namespace std;

class DirFileHandler
{
private:
	// directories 
	LPCSTR WEEK_LAB_PATH = "C:\\Facultate\\CSSO\\Week4";
	LPCSTR SUMMARY_DIR_PATH = "C:\\Facultate\\CSSO\\Week4\\Reports\\Summary";

	// files
	LPCSTR SOLD_FILE_PATH = "C:\\Facultate\\CSSO\\Week4\\Reports\\Summary\\sold.txt";
	LPCSTR DONATIONS_FILE_PATH = "C:\\Facultate\\CSSO\\Week4\\Reports\\Summary\\donations.txt";

	// path to executables
	// 
	// Here you should provide the write paths from your machines to correct executable that needs to be started

	LPCSTR PATH_TO_DONATE_EXE = "C:\\Users\\teomi\\Desktop\\an3\\SEM1\\CSSO\\CSSO-Homeworks\\DonationsExe\\x64\\Debug\\DonationsExe.exe";
	LPCSTR PATH_TO_SOLD_EXE = "C:\\Users\\teomi\\Desktop\\an3\\SEM1\\CSSO\\CSSO-Homeworks\\SoldExe\\x64\\Debug\\SoldExe.exe";
	LPCSTR PATH_TO_DEPOSIT_EXE = "C:\\Users\\teomi\\Desktop\\an3\\SEM1\\CSSO\\CSSO-Homeworks\\DepositExe\\Deposit\\x64\\Debug\\DepositExe.exe";

	HANDLE hMapShelves = NULL;
	HANDLE hMapValability = NULL;
	HANDLE hMapPrices = NULL;

	DWORD MAPPING_MEMORY_SIZE = sizeof(DWORD) * 10000;


	// memory mapping names
	// TODO

	// extra variables
	wstring pathToDeposit;
	wstring pathToSold;
	wstring lastError;
	wstring content;

public:
	DWORD setPathDir(wstring, DWORD);
	DWORD setUpDirAndFiles();
	wstring getPathDir(DWORD);
	wstring getContentForConsole();
	wstring getCLIArguments(LPCSTR);
	wstring getLastError();
	BOOL recursiveCreateDirectory(LPCSTR);
	BOOL creationOfFileAndDirs(LPCSTR, DWORD);
	BOOL createFile(LPCSTR);
	DWORD createMemoryMapping();
	DWORD readResult();
	LPCSTR getPathToExecutable(DWORD);
	void closeMemoryHandles();
	static wstring ConvertToWString(LPCSTR);

};
