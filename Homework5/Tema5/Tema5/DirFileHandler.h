#pragma once
#pragma once
#include <Windows.h>
#include <wchar.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include "constants.h"


#define WEEK_LAB_DIR 1
#define DOWNLOADS_DIR 2
#define MY_CONFIG 3

using namespace std;

class DirFileHandler
{
private:
	// directories 
	string lastError;
	LPCSTR WEEK_LAB_PATH = "C:\\Facultate\\CSSO\\Week5";
	LPCSTR DOWNLOAD_DIR_PATH = "C:\\Facultate\\CSSO\\Week5\\Downloads";
	LPCSTR MY_CONFIG_PATH = "C:\\Facultate\\CSSO\\Week5\\myconfig.txt";

	HANDLE myConfig;

public:
	DWORD setUpDirAndFiles();
	LPCSTR getPath(DWORD);
	string getLastError();
	BOOL recursiveCreateDirectory(LPCSTR);
	BOOL creationOfFileAndDirs(LPCSTR, DWORD);
	HANDLE createFile(LPCSTR);
	HANDLE openFile(LPCSTR);
	BOOL writeToFile(HANDLE, LPCSTR, DWORD);
	BOOL appendToFile(HANDLE, LPCSTR, DWORD);
};

