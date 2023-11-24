#pragma once
#include <Windows.h>
#include <wchar.h>
#include <string.h>
#include <iostream>
#include <sstream>



#define DEPOSIT_DIR 1
#define SOLD_DIR 2

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

	// memory mapping names
	// TODO

	// extra variables
	wstring pathToDeposit;
	wstring pathToSold;
	wstring lastError;

public:
	DWORD setPathDir(wstring, DWORD);
	DWORD setUpDirAndFiles();
	wstring getPathDir(DWORD);
	wstring getContentForConsole();
	wstring getLastError();
	BOOL recursiveCreateDirectory(LPCSTR);
	BOOL creationOfFileAndDirs(LPCSTR, DWORD);
	BOOL createFile(LPCSTR);
	static wstring ConvertToWString(LPCSTR);

};
