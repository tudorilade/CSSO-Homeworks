#pragma once
#include <Windows.h>
#include <string>
#include <iostream>
#include "winnett.h"

using namespace std;

#define FTP_PROTOCOL 1
#define HTTPS_PROTOCOL 2
#define SESSION 3
#define INTERNET 4

class Client
{
private:

	LPCSTR doHomeworkRoute = "dohomework/";
	LPCSTR doAdditionalHomeworkRoute = "dohomework_additional/";
	LPCSTR endHomeworkRoute = "endhomework";
	
	LPCSTR userAgent = "Tudor - Teodor";
	HINTERNET internetOpenHandle;
	HINTERNET session;
	string lastError;
public:

	Client();
	void openInternetConnection();
	void startSession(LPCSTR, DWORD);
	void closeHandles();

	BOOL isHandleValid(HINTERNET);	
	BOOL downloadFile(HINTERNET, LPCSTR, LPCSTR);
	
	HINTERNET getHandle(DWORD);
	HINTERNET get(LPSTR, DWORD&);
	HINTERNET post(LPSTR, const string&, DWORD&);
	HINTERNET get(LPSTR);
	HINTERNET post(LPSTR, const string&);

	LPCSTR getLastError();
	LPCSTR formatResource(LPCSTR, DWORD);
	LPSTR constructUrlPath(LPSTR, LPSTR);
};

