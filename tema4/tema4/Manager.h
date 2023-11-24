#pragma once
#include "DirFileHandler.h"


class Manager
{
private:
	wstring lastError;

public:
	DWORD startManagement(DirFileHandler&);
	wstring getError();
};

