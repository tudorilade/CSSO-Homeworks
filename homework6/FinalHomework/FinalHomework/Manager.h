#pragma once

#include "constants.h"
#include "DirFileHandler.h"

using namespace std;


class Manager
{
private:
	HWND mainWindow;
	HWND logger;
	DWORD childWindowType;
	DirFileHandler fileHandler;

public:
	Manager() {};
	Manager(HWND, DWORD);
	
	HWND getLogger();

	void execute();
	void static LOG(HWND, LPCSTR, BOOL);
	void static LOG(HWND, LPSTR, BOOL);
};

