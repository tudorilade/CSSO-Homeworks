#pragma once

#include "DirFileHandler.h"

class Manager
{
private:
	LPSTR linkResource;
	DWORD sizeLinkResource;
	string lastError;

public:
	Manager() {};
	Manager(LPSTR, LPSTR);
	LPSTR getResource();
	DWORD getSizeResource();
	DWORD execute();
	string getLastError();
};

