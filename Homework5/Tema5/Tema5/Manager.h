#pragma once

#include "DirFileHandler.h"
#include "Client.h"
#include "RequestType.h"
#include "vector"

class Manager
{
private:

	LPSTR assignHomework;

	LPSTR serverResource;
	DWORD sizeLinkResource;
	
	LPSTR nrMatricol;
	DWORD sizeNrMatricol;

	LPSTR relativePath;
	DWORD sizeRelativePath;

	string lastError;
	DWORD numberOfGetRequests;
	DWORD numberOFWriteRequests;

	HWND logger;

	DirFileHandler fileHandler;
	Client client;

	DWORD lastRequestResponse;

public:
	Manager() {};
	Manager(LPSTR, DWORD, LPSTR, DWORD, HWND&);
	LPSTR getResource();
	DWORD getSizeResource();
	DWORD execute();
	BOOL startProcessing();
	BOOL downloadConfigFile();
	string getLastError();
	void LOG(LPCSTR);
	void LOG(LPSTR);

	BOOL proccessConfigFile(vector<RequestType>&);
	RequestType processLineAndGetRequest(const char*);
	BOOL processDoHomeworkRequest(RequestType&);
	BOOL processAditionalHomework(RequestType&);
	BOOL processGetRequest(RequestType&);
	BOOL proccessPostRequest(RequestType&);
};

