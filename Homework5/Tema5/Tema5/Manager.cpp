#include "Manager.h"


Manager::Manager(LPSTR link, LPSTR matricolNr) {
	this->sizeLinkResource = strlen(link) + strlen(matricolNr) + 1;
	this->linkResource = (LPSTR)calloc(this->sizeLinkResource, sizeof(LPSTR));
	strncat_s(this->linkResource, sizeLinkResource, link, strlen(link));
	strncat_s(this->linkResource, sizeLinkResource, matricolNr, strlen(matricolNr));
}

LPSTR Manager::getResource()
{
	return this->linkResource;
}


DWORD Manager::getSizeResource()
{
	return this->sizeLinkResource;
}


string Manager::getLastError()
{
	return this->lastError;
}

DWORD Manager::execute()
{
	DirFileHandler fileHandler = DirFileHandler();

	if (!fileHandler.setUpDirAndFiles())
	{
		this->lastError = fileHandler.getLastError();
		return FALSE;
	}

	return 1;
}
