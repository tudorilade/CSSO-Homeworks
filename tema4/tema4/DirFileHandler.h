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
	wstring pathToDeposit;
	wstring pathToSold;

public:
	DWORD setPathDir(wstring, DWORD);
	wstring getPathDir(DWORD);
	wstring getContentForConsole();
};

