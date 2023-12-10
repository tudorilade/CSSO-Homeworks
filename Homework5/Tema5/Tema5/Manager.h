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

	HWND logger;

	DirFileHandler fileHandler;
	Client client;

	char* lastRequestResponse = NULL;
	DWORD numberOfGetRequests = 0;
	DWORD numberOfPostRequests = 0;

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
	void LOG(LPCSTR, BOOL);
	void LOG(LPSTR, BOOL);

	BOOL sendStats();
	BOOL proccessConfigFile(vector<RequestType>&);
	RequestType processLineAndGetRequest(const char*);
	BOOL processDoHomeworkRequest(RequestType&);
	BOOL processAditionalHomework(RequestType&);
	BOOL processGetRequest(RequestType&);
	BOOL proccessPostRequest(RequestType&);
};

