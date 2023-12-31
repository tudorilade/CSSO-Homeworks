#pragma once

#include "constants.h"
#include "utils.h"
#include "DirFileHandler.h"

using namespace std;


class Manager
{
protected:
	HWND mainWindow;
	HWND logger;
	DirFileHandler fileHandler;
public:
	Manager() {};
	Manager(HWND);

	virtual void execute() = 0;
	HWND getLogger();

	DWORD setupFileHandler();
	void static LOG(HWND, LPCSTR, BOOL);
	void static LOG(HWND, LPSTR, BOOL);
	void static LOG(HWND, LPCWSTR, BOOL);
	void static LOG(HWND, LPWSTR, BOOL);
};

