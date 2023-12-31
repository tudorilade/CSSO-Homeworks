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
	wstring lastError;
	LPCWSTR WEEK_LAB_PATH = L"C:\\Facultate\\CSSO\\Week6";
	LPCWSTR IMAGE_LAB_PATH = L"C:\\Facultate\\CSSO\\Week6\\date";
	LPCWSTR SEQUENTIAL_LAB_PATH = L"C:\\Facultate\\CSSO\\Week6\\secvential";
	LPCWSTR STATIC_LAB_PATH = L"C:\\Facultate\\CSSO\\Week6\\static";
	LPCWSTR DINAMIC_LAB_PATH = L"C:\\Facultate\\CSSO\\Week6\\dinamic";
	
	// files
	LPCWSTR INFO_FILE_PATH = L"C:\\Facultate\\CSSO\\Week6\\info.txt";
	LPCWSTR COMP_FILE_PATH = L"C:\\Facultate\\CSSO\\Week6\\comp.txt";
	LPCWSTR GRAPHS_FILE_PATH = L"C:\\Facultate\\CSSO\\Week6\\grafice.xlsx";

	HANDLE myConfig;

public:
	DWORD setUpDirAndFiles();
	LPCWSTR getPath(DWORD);
	wstring getLastError();
	BOOL recursiveCreateDirectory(LPCWSTR);
	BOOL creationOfFileAndDirs(LPCWSTR, DWORD);
	HANDLE createFile(LPCWSTR);
	HANDLE openFile(LPCWSTR);
	BOOL writeToFile(HANDLE, LPCSTR, DWORD);
	BOOL appendToFile(HANDLE, LPCSTR, DWORD);
	DWORD fileExistsOrInvalid(HANDLE);
};
