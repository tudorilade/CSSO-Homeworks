#include "Client.h"


Client::Client()
{
	this->openInternetConnection();
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
HINTERNET Client::get(LPSTR route)
{
	HINTERNET hRequest = HttpOpenRequest(session, "GET", route, NULL, NULL, NULL, INTERNET_FLAG_SECURE, 0);
	if (!hRequest) { return NULL; }
	if (!HttpSendRequest(hRequest, NULL, 0, NULL, 0)) { return NULL; }
	return hRequest;
}


LPCSTR Client::post(LPSTR resource, LPCSTR buffer)
{

	return "";
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