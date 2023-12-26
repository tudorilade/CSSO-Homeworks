#pragma once
#include <Windows.h>
#include <wchar.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include "constants.h"

// file constants
#define INFO_FILE 1
#define COMPARISON_FILE 2
#define GRAPHS_FILE 3

// directories constants
#define WEEK_LAB_DIR 11
#define IMAGE_DIR 12
#define SEQUENTIAL_DIR 13
#define STATIC_DIR 14
#define DINAMIC_DIR 15


using namespace std;

class DirFileHandler
{
private:
	// directories 
	string lastError;
	LPCSTR WEEK_LAB_PATH = "C:\\Facultate\\CSSO\\Week6";
	LPCSTR IMAGE_LAB_PATH = "C:\\Facultate\\CSSO\\Week6\\date";
	LPCSTR SEQUENTIAL_LAB_PATH = "C:\\Facultate\\CSSO\\Week6\\secvential";
	LPCSTR STATIC_LAB_PATH = "C:\\Facultate\\CSSO\\Week6\\static";
	LPCSTR DINAMIC_LAB_PATH = "C:\\Facultate\\CSSO\\Week6\\dinamic";
	
	// files
	LPCSTR INFO_FILE_PATH = "C:\\Facultate\\CSSO\\Week6\\info.txt";
	LPCSTR COMP_FILE_PATH = "C:\\Facultate\\CSSO\\Week6\\comp.txt";
	LPCSTR GRAPHS_FILE_PATH = "C:\\Facultate\\CSSO\\Week6\\grafice.xlsx";

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
	DWORD fileExistsOrInvalid(HANDLE);
};
