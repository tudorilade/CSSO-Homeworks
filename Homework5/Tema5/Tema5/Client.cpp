#include "Client.h"


Client::Client(HWND& logger)
{
	this->openInternetConnection();
	this->logger = logger;
}


void Client::openInternetConnection()
{
	this->internetOpenHandle = InternetOpen(
		this->userAgent,
		INTERNET_OPEN_TYPE_PRECONFIG,
		NULL,
		NULL,
		NULL
	);
}

void Client::startSession(LPCSTR resource, DWORD protocol)
{
	LPCSTR newResource = this->formatResource(resource, protocol);
	this->session = InternetConnect(
		this->internetOpenHandle,
		resource,
		INTERNET_DEFAULT_HTTPS_PORT,
		NULL,
		NULL,
		INTERNET_SERVICE_HTTP,
		0,
		0
	);
}

HINTERNET Client::getHandle(DWORD type)
{
	switch (type)
	{
	case SESSION: return this->session;
	case INTERNET: return this->internetOpenHandle;
	}
	return this->internetOpenHandle;
}


BOOL Client::isHandleValid(HINTERNET handle)
{
	if (handle == NULL)
	{
		return FALSE;
	}
	return TRUE;
}


LPCSTR Client::getLastError()
{
	return this->lastError.c_str();
}

LPCSTR Client::formatResource(LPCSTR resource, DWORD protocol)
{
	string prefix;

	switch (protocol)
	{
	case FTP_PROTOCOL: prefix = "ftp://"; break;
	default: prefix = "https://";
	}
	string formatedResource = string(prefix) + string(resource);
	size_t size = formatedResource.size() + 1;
	LPSTR result = new char[size];
	strncpy_s(result, size, formatedResource.c_str(), size);
	LPCSTR formattedRes = result;
	return formattedRes;
}


/**
* Method responsible for sending a GET request to a route from session
*/
HINTERNET Client::get(LPSTR route, DWORD& getRequests)
{
	this->LOG("GET: ");
	this->LOG(route, TRUE);
	HINTERNET hRequest = HttpOpenRequest(session, "GET", route, NULL, NULL, NULL, INTERNET_FLAG_SECURE, 0);
	if (!hRequest) { return NULL; }
	if (!HttpSendRequest(hRequest, NULL, 0, NULL, 0)) { return NULL; }
	getRequests++;
	return hRequest;
}


HINTERNET Client::post(LPSTR route, const string& postData, DWORD& postRequests)
{
	this->LOG("POST: ");
	this->LOG(route);
	this->LOG("?");
	this->LOG(postData.c_str(), TRUE);
	LPCSTR headers = "Content-Type: application/x-www-form-urlencoded";
	HINTERNET hRequest = HttpOpenRequest(session, "POST", route, NULL, NULL, NULL, INTERNET_FLAG_SECURE, 0);
	if (!hRequest) { return NULL; }
	if (!HttpSendRequest(hRequest, headers, -1, const_cast<char*>(postData.c_str()), postData.length())) {
		InternetCloseHandle(hRequest);
		return NULL;
	}
	postRequests++;
	return hRequest;
}


/**
* Method responsible for sending a GET request to a route from session
*/
HINTERNET Client::get(LPSTR route)
{
	this->LOG("GET: ");
	this->LOG(route, TRUE);
	HINTERNET hRequest = HttpOpenRequest(session, "GET", route, NULL, NULL, NULL, INTERNET_FLAG_SECURE, 0);
	if (!hRequest) { return NULL; }
	if (!HttpSendRequest(hRequest, NULL, 0, NULL, 0)) { return NULL; }
	return hRequest;
}

/**
* Method responsible for sending a POST request
*/
HINTERNET Client::post(LPSTR route, const string& postData)
{
	this->LOG("POST: ");
	this->LOG(route);
	this->LOG("?");
	this->LOG(postData.c_str(), TRUE);
	LPCSTR headers = "Content-Type: application/x-www-form-urlencoded";
	HINTERNET hRequest = HttpOpenRequest(session, "POST", route, NULL, NULL, NULL, INTERNET_FLAG_SECURE, 0);
	if (!hRequest) { return NULL; }
	if (!HttpSendRequest(hRequest, headers, -1, const_cast<char*>(postData.c_str()), postData.length())) {
		InternetCloseHandle(hRequest);
		return NULL;
	}	
	return hRequest;
}

/**
* Method responsible for checking the status code of a request
*/
BOOL Client::isStatusCodeValid(HINTERNET& hReq) {
	DWORD httpStatusCode = 0;
	DWORD statusCodeSize = sizeof(httpStatusCode);

	if (!HttpQueryInfo(hReq, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
		&httpStatusCode, &statusCodeSize, NULL)) {
		this->LOG("Error while checking the status code: ");
		this->LOG(to_string(GetLastError()).c_str(), TRUE);
		return FALSE;
	}
	this->LOG("Status code: ");
	this->LOG(to_string(httpStatusCode).c_str(), TRUE);
	if (!(httpStatusCode >= 200 && httpStatusCode < 300)) {
		InternetCloseHandle(hReq);
		return FALSE;
	}
	return TRUE;
}

LPSTR Client::constructUrlPath(LPSTR server, LPSTR route)
{
	size_t newSize = strlen(server) + strlen(route) + 2;
	LPSTR buffer = (LPSTR)calloc(newSize, sizeof(LPSTR));
	strncat_s(buffer, newSize, server, strlen(server));
	strncat_s(buffer, newSize, "/", 1);
	strncat_s(buffer, newSize, route, strlen(route));
	return buffer;
}

void Client::closeHandles() {
	if (session)
	{
		InternetCloseHandle(session);
		session = NULL;
	}

	if (internetOpenHandle)
	{
		InternetCloseHandle(internetOpenHandle);
		internetOpenHandle = NULL;
	}
}

void Client::LOG(LPCSTR buffer)
{
	int length = GetWindowTextLength(logger);
	SendMessage(logger, EM_SETSEL, (WPARAM)length, (LPARAM)length);
	SendMessage(logger, EM_REPLACESEL, FALSE, (LPARAM)buffer);
}

void Client::LOG(LPSTR buffer)
{
	int length = GetWindowTextLength(logger);
	SendMessage(logger, EM_SETSEL, (WPARAM)length, (LPARAM)length);
	SendMessage(logger, EM_REPLACESEL, FALSE, (LPARAM)buffer);
}

void Client::LOG(LPCSTR buffer, BOOL addNewLine)
{
	int length = GetWindowTextLength(logger);
	SendMessage(logger, EM_SETSEL, (WPARAM)length, (LPARAM)length);
	SendMessage(logger, EM_REPLACESEL, FALSE, (LPARAM)buffer);
	if (addNewLine) {
		SendMessage(logger, EM_REPLACESEL, FALSE, (LPARAM)"\r\n");
	}
}

void Client::LOG(LPSTR buffer, BOOL addNewLine)
{
	int length = GetWindowTextLength(logger);
	SendMessage(logger, EM_SETSEL, (WPARAM)length, (LPARAM)length);
	SendMessage(logger, EM_REPLACESEL, FALSE, (LPARAM)buffer);
	if (addNewLine) {
		SendMessage(logger, EM_REPLACESEL, FALSE, (LPARAM)"\r\n");
	}
}
