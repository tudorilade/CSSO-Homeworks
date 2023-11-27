#pragma once
#include "DirFileHandler.h"


class Manager
{
private:
	wstring lastError;

public:
	DWORD startManagement(DirFileHandler&);
	wstring getError();
	BOOL execute(DirFileHandler&);
	BOOL populateEventHandles(HANDLE*, LPCSTR*, int);
};

