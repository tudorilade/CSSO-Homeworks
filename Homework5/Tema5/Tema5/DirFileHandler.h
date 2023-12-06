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

using namespace std;

class DirFileHandler
{
private:
	// directories 
	string lastError;
	LPCSTR WEEK_LAB_PATH = "C:\\Facultate\\CSSO\\Week5";
	LPCSTR DOWNLOAD_DIR_PATH = "C:\\Facultate\\CSSO\\Week5\\Downloads";


public:
	DWORD setUpDirAndFiles();
	string getPathDir(DWORD);
	string getLastError();
	BOOL recursiveCreateDirectory(LPCSTR);
	BOOL creationOfFileAndDirs(LPCSTR, DWORD);
	BOOL createFile(LPCSTR);
};

