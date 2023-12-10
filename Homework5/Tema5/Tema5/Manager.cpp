#include "Manager.h"
#include "vector"

Manager::Manager(LPSTR serverAddress, DWORD sizeResource, LPSTR nrMatricol, DWORD sizeNrMatricol, HWND& logWindow) {
	this->sizeLinkResource = sizeResource;
	this->sizeNrMatricol = sizeNrMatricol;
	size_t sizeHomework = strlen("assignhomework/");
	size_t relativeSize = sizeHomework + sizeNrMatricol + 1; // +1 for null terminator

	this->serverResource = (LPSTR)calloc(this->sizeLinkResource, sizeof(char));
	this->nrMatricol = (LPSTR)calloc(this->sizeNrMatricol, sizeof(char));
	this->relativePath = (LPSTR)calloc(relativeSize, sizeof(char));

	strncpy_s(this->serverResource, sizeLinkResource, serverAddress, sizeResource);
	strncpy_s(this->nrMatricol, sizeNrMatricol, nrMatricol, sizeNrMatricol);
	strcpy_s(this->relativePath, relativeSize, "assignhomework/");
	strncat_s(this->relativePath, relativeSize, nrMatricol, sizeNrMatricol);

	this->logger = logWindow;
}


LPSTR Manager::getResource()
{
	return this->serverResource;
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
	this->fileHandler = DirFileHandler();

	if (!fileHandler.setUpDirAndFiles())
	{
		this->lastError = fileHandler.getLastError();
		return FALSE;
	}

	this->client = Client(this->logger);

	if (!client.isHandleValid(client.getHandle(INTERNET)))
	{
		this->lastError = "Error opening connection to internet. Error code: " + to_string(GetLastError());
		return FALSE;
	}

	this->LOG("Starting the processing..... \r\n\r\n");

	if (!this->startProcessing())
	{
		return FALSE;
	}

	this->client.closeHandles();
	this->LOG("Finalized processing..... \r\n");

	return 1;
}

BOOL Manager::proccessConfigFile(vector<RequestType>& requestsVector) {

	//open wanted file which holds the requests information
	HANDLE hReqFile = CreateFile(
		this->fileHandler.getPath(MY_CONFIG),
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (hReqFile == INVALID_HANDLE_VALUE) { 
		this->lastError = "Error opening config file. Error code: " + to_string(GetLastError());
		return FALSE; 
	}

	//go line by line
	DWORD dwBytesRead;
	char buffer[BUFFER_SIZE];

	char currentLine[BUFFER_SIZE];
	size_t currentLineIndex = 0;

	while (ReadFile(hReqFile, buffer, BUFFER_SIZE - 1, &dwBytesRead, NULL) && dwBytesRead > 0) {
		buffer[dwBytesRead] = '\0'; // Null-terminate the buffer

		for (char* p = buffer; *p; ++p) {
			if (*p == '\n') {
				currentLine[currentLineIndex] = '\0';

				RequestType request = this->processLineAndGetRequest(currentLine);


				if (request.getReqType() != -1) {
					requestsVector.emplace_back(request);
				}
				else {
					this->LOG("Unknown request for: ");
					this->LOG(request.getPath(), TRUE);
				}

				currentLineIndex = 0;
			}
			else {
				currentLine[currentLineIndex++] = *p;
			}
		}
	}

	if (currentLineIndex > 0) {
		// Null-terminate the last line if not terminated with '\n'
		currentLine[currentLineIndex] = '\0';
		requestsVector.emplace_back(processLineAndGetRequest(currentLine));
	}

	CloseHandle(hReqFile);
	return TRUE;
}

RequestType Manager::processLineAndGetRequest(const char* line) {
	int reqType = -1;
	char path[50];
	memset(path, 0, 50);

	const char* getPtr = strstr(line, "GET:");
	const char* postPtr = strstr(line, "POST:");
	const char* pathPtr = strstr(line, ".com/");

	if (getPtr != NULL) {
		reqType = GET_REQ; // 1 for GET
	}
	else if (postPtr != NULL) {
		reqType = POST_REQ; // 2 for POST
	}

	unsigned int routeType = DO_HOMEWORK_ROUTE;

	if (pathPtr != NULL) {
		pathPtr += strlen(".com/");
		size_t pathLength = strcspn(pathPtr, "\0"); // Find the length of the path
		if (pathLength < sizeof(path) - 1) {
			// Copy the path and null-terminate
			memcpy(path, pathPtr, pathLength);
		}

		if (strstr(line, "dohomework_additional") != NULL)
		{
			routeType = DO_ADDITIONAL_ROUTE;
		}
	}

	return RequestType(reqType, path, routeType);
}

/**
* Main method responsible for establishing the connection with the server, processing the config file and 
* sending the results to endhomework/
*/
BOOL Manager::startProcessing()
{
	// 1. instantiate client
	// 2. download resource
	// 3. write to myConfig

	this->LOG("Starting session with ");
	this->LOG(this->getResource(), TRUE);

	client.startSession(this->getResource(), HTTPS_PROTOCOL);

	if (!client.isHandleValid(client.getHandle(SESSION)))
	{
		this->lastError = "Error starting session to internet. Error code: " + to_string(GetLastError());
		return FALSE;
	}

	this->LOG("Session established!", TRUE);

	if (!downloadConfigFile())
	{
		this->lastError = "Error downloading the file from " + string(this->relativePath) + ".Error code : " + to_string(GetLastError());
		return FALSE;
	}


	// 4. Read Line by Line from the file until all file is passed
	this->LOG("Start parsing the config file! \r\n");

	vector<RequestType> requests;
	if (!this->proccessConfigFile(requests))
	{
		return FALSE;
	}

	for (RequestType& req : requests) {
		//send requests
		this->LOG("\r\n", TRUE);
		if (req.getRoute() == DO_HOMEWORK_ROUTE) {
			if (!this->processDoHomeworkRequest(req)) {
				this->LOG("Request for route: ");
				this->LOG(req.getPath());
				this->LOG(" couldn't be process. Reason: ");
				this->LOG(this->lastError.c_str(), TRUE);
			}
		}
		else if (req.getRoute() == DO_ADDITIONAL_ROUTE) {
			if (!this->processAditionalHomework(req)) {
				this->LOG("Request for route: ");
				this->LOG(req.getPath());
				this->LOG(" couldn't be proccessed. Reason: ");
				this->LOG(this->lastError.c_str(), TRUE);
			}
		}
	}
	// 5. Send the last post
	this->LOG("\r\n", TRUE);


	DWORD totalReq = numberOfGetRequests + numberOfPostRequests;
	this->LOG("Total of requests: ");
	this->LOG(to_string(totalReq).c_str(), TRUE);

	if (!this->sendStats()) {
		this->LOG("Post request for the last stats could not be proccessed!\r\n");
	}

	this->LOG("\r\n", TRUE);
	this->LOG("Finalized parsing the config file! \r\n");

	return TRUE;
}

/**
* Method responsible for sending the last request to endhomework route
*/
BOOL Manager::sendStats() {
	
	const char* path = fileHandler.getPath(DOWNLOADS_DIR);
	size_t len = strlen(path);
	char* newPath = new char[len + 3];
	strcpy_s(newPath, len + 1, path);
	strcat_s(newPath, len + 3, "\\*");
	LPCSTR totalSize = fileHandler.GetTotalSize(newPath);
	if (strlen(totalSize) == 0) {
		return FALSE;
	}
	DWORD totalReq = numberOfGetRequests + numberOfPostRequests;
	string postData = "id=" +
		string(this->nrMatricol) +
		"&total=" +
		to_string(totalReq) +
		"&get=" +
		to_string(numberOfGetRequests) +
		"&post=" +
		to_string(numberOfPostRequests) +
		"&size=" + string(totalSize);

	char* reqPath = new char[strlen("endhomework")+1];
	strcpy_s(reqPath, strlen("endhomework") + 1, "endhomework");
	HINTERNET hPostRequest = client.post(reqPath, postData);
	if (hPostRequest == NULL) {
		this->lastError = "Failed to post request. Error code: " + to_string(GetLastError());
		return FALSE;
	}

	if (!client.isStatusCodeValid(hPostRequest))
	{
		this->lastError = "Failed to post request. Error code: " + to_string(GetLastError());
		return FALSE;
	}

	return TRUE;
}

BOOL Manager::processDoHomeworkRequest(RequestType& req)
{
	switch (req.getReqType())
	{
	case GET_REQ: return this->processGetRequest(req);
	case POST_REQ: return this->proccessPostRequest(req);
	}

	return FALSE;
}


BOOL Manager::processGetRequest(RequestType& req)
{
	HINTERNET hGetRequest = client.get(req.getPath(), numberOfGetRequests);
	if (hGetRequest == NULL) {
		this->lastError = "Failed to get request. Error code: " + to_string(GetLastError());
		return FALSE;
	}

	if (!client.isStatusCodeValid(hGetRequest))
	{
		InternetCloseHandle(hGetRequest);
		this->lastError = "Failed to post request. Error code: " + to_string(GetLastError());
		return FALSE;
	}

	DWORD bytesRead = 0;
	char response[BUFFER_SIZE] = { 0 };

	if (!InternetReadFile(hGetRequest, response, BUFFER_SIZE - 1, &bytesRead)) {
		this->lastError = "Can't retrieve the request. Error code: " + to_string(GetLastError());
		InternetCloseHandle(hGetRequest);
		return FALSE;
	}

	if (bytesRead == 0) {
		this->lastError = "Empty response. Error code: " + to_string(GetLastError());
		InternetCloseHandle(hGetRequest);
		return FALSE;
	}

	InternetCloseHandle(hGetRequest);

	this->lastRequestResponse = new char[bytesRead + 1];
	strcpy_s(this->lastRequestResponse, bytesRead + 1, response);
	this->lastRequestResponse[bytesRead] = '\0';

	this->LOG("Read from current request ");
	this->LOG(req.getPath());
	this->LOG(" response ");
	this->LOG(response);
	this->LOG("\r\n");

	string downloadPath = fileHandler.getPath(DOWNLOADS_DIR);
	string reqValue = req.getValueFromRoute();
	string extension = ".txt";
	string fullPath = downloadPath + "\\" + reqValue + extension;
	const char* fPath = fullPath.c_str();

	char* dumpBuffer = new char[strlen(response) + strlen("\n") + 1];
	strcpy_s(dumpBuffer, strlen(response) + strlen("\n") + 1, response);
	strcat_s(dumpBuffer, strlen(response) + strlen("\n") + 1, "\n");

	HANDLE hFileToValue = fileHandler.openFile(fPath);

	if (fileHandler.fileExistsOrInvalid(hFileToValue) == 0) {
		// file does not exist; create and write
		hFileToValue = fileHandler.createFile(fPath);
		if (fileHandler.fileExistsOrInvalid(hFileToValue) != 1) {
			this->lastError = "Can't open file. Error code: " + to_string(GetLastError());
			delete[] dumpBuffer;
			return FALSE;
		}
		if (!fileHandler.writeToFile(hFileToValue, dumpBuffer, strlen(dumpBuffer))) {
			this->lastError = "Can't write to file. Error code: " + to_string(GetLastError());
			CloseHandle(hFileToValue);
			delete[] dumpBuffer;
			return FALSE;
		}
		CloseHandle(hFileToValue);
		this->LOG("Wrote to file");
		this->LOG(fPath, TRUE);
		delete[] dumpBuffer;
		return TRUE;

	}
	else if (fileHandler.fileExistsOrInvalid(hFileToValue) == 2) {
		this->lastError = "Can't open file. Error code: " + to_string(GetLastError());
		delete[] dumpBuffer;
		return FALSE;
	}

	if (!fileHandler.appendToFile(hFileToValue, dumpBuffer, strlen(dumpBuffer))) {
		this->lastError = "Can't append to file. Error code: " + to_string(GetLastError());
		CloseHandle(hFileToValue);
		delete[] dumpBuffer;
		return FALSE;
	}
	this->LOG("Appended content to file", TRUE);
	this->LOG(fPath, TRUE);
	CloseHandle(hFileToValue);
	delete[] dumpBuffer;
	return TRUE;
}


BOOL Manager::proccessPostRequest(RequestType& req)
{
	string postData = "id=" + string(nrMatricol) + "&value=" + lastRequestResponse; // Replace with actual data
	HINTERNET hPostRequest = client.post(req.getPath(), postData, numberOfPostRequests);
	if (hPostRequest == NULL) {
		this->lastError = "Failed to post request. Error code: " + std::to_string(GetLastError());
		return FALSE;
	}

	if (!client.isStatusCodeValid(hPostRequest))
	{
		this->lastError = "Failed to post request. Error code: " + to_string(GetLastError());
		return FALSE;
	}

	return TRUE;
}

BOOL Manager::processAditionalHomework(RequestType& req)
{
	
	HINTERNET hGetRequest = client.get(req.getPath(), numberOfGetRequests);
	if (hGetRequest == NULL) { return FALSE; }

	if (!client.isStatusCodeValid(hGetRequest))
	{
		this->lastError = "Failed to post request. Error code: " + to_string(GetLastError());
		return FALSE;
	}

	// Preparing to write to file
	HANDLE hFile;
	const char* routeValue = req.getValueFromRoute();
	size_t fileNameSize = strlen(fileHandler.getPath(DOWNLOADS_DIR)) + strlen(routeValue) + strlen("_additional.txt") + 2; // +1 for null-terminator
	char* fileName = new char[fileNameSize];
	strcpy_s(fileName, fileNameSize, fileHandler.getPath(DOWNLOADS_DIR));
	strcat_s(fileName, fileNameSize, "\\");
	strcat_s(fileName, fileNameSize, routeValue);
	strcat_s(fileName, fileNameSize, "_additional.txt");
	HANDLE hTempFile = fileHandler.openFile(fileName);
	int checkExistance = fileHandler.fileExistsOrInvalid(hTempFile);
	CloseHandle(hTempFile);
	if (checkExistance == 1) //file exists
	{
		hFile = fileHandler.openFile(fileName);
		if (hFile == INVALID_HANDLE_VALUE) {
			InternetCloseHandle(hGetRequest);
			this->lastError = "Failed to open the file. Error code: " + to_string(GetLastError());
			return FALSE;
		}
	}
	else if(checkExistance == 0){ //does not exist
		hFile = fileHandler.createFile(fileName);
		if (hFile == INVALID_HANDLE_VALUE) {
			InternetCloseHandle(hGetRequest);
			this->lastError = "Failed to create the file. Error code: " + to_string(GetLastError());
			return FALSE;
		}
	} else
	{
		this->lastError = "Failed to create / open the file. Error code: " + to_string(GetLastError());
		InternetCloseHandle(hGetRequest);
		return FALSE;
	}
	DWORD bytesRead = 0;
	// Read data into the buffer
	char buffer[sizeof(DWORD)];
	memset(buffer, 0, sizeof(DWORD));
	if (!InternetReadFile(hGetRequest, buffer, sizeof(DWORD), &bytesRead))
	{
		this->lastError = "Can't retrieve the request. Error code: " + to_string(GetLastError());
		InternetCloseHandle(hGetRequest);
		return FALSE;
	}

	if (bytesRead == 0)
	{
		this->lastError = "Can't retrieve the request. Error code: " + to_string(GetLastError());
		InternetCloseHandle(hGetRequest);
		return FALSE;
	}
	this->lastRequestResponse = (char*)calloc(bytesRead + 1, sizeof(char)); // +1 for null-terminator
	strncpy_s(this->lastRequestResponse, bytesRead + 1, buffer, bytesRead);

	char bufferDUMPED[50];
	strcpy_s(bufferDUMPED, 50, this->lastRequestResponse);
	strcat_s(bufferDUMPED, 50, "\n");
	if (!fileHandler.appendToFile(hFile, bufferDUMPED, strlen(bufferDUMPED))) {
		InternetCloseHandle(hGetRequest);
		CloseHandle(hFile);
		return FALSE;
	}

	this->LOG("Response get: ");
	this->LOG(buffer, TRUE);

	while (strcmp(buffer, "0") != 0) {
		size_t sizeOfNewPath = strlen("dohomework_additional/") + strlen(buffer) + 1; // +1 for null-terminator
		char* newPath = new char[sizeOfNewPath];
		strcpy_s(newPath, sizeOfNewPath, "dohomework_additional/");
		strcat_s(newPath, sizeOfNewPath, buffer);

		hGetRequest = client.get(newPath, numberOfGetRequests);
		
		if (!client.isStatusCodeValid(hGetRequest))
		{
			InternetCloseHandle(hGetRequest);
			CloseHandle(hFile);
			this->lastError = "Failed to get request. Error code: " + to_string(GetLastError());
			return FALSE;
		}
		bytesRead = 0;
		//empty the buffer
		memset(buffer, 0, sizeof(DWORD));
		if (!InternetReadFile(hGetRequest, buffer, sizeof(DWORD), &bytesRead))
		{
			InternetCloseHandle(hGetRequest);
			CloseHandle(hFile);
			this->lastError = "Can't retrieve the request. Error code: " + std::to_string(GetLastError());
			return FALSE;
		}

		if (bytesRead == 0)
		{
			InternetCloseHandle(hGetRequest);
			CloseHandle(hFile);
			this->lastError = "Can't retrieve the request. Error code: " + to_string(GetLastError());
			return FALSE;
		}
		this->lastRequestResponse = (char*)calloc(bytesRead + 1, sizeof(char)); // +1 for null-terminator

		// Use strncpy_s safely
		strncpy_s(this->lastRequestResponse, bytesRead + 1, buffer, bytesRead);
		this->LOG("Response get: ");
		this->LOG(buffer, TRUE);
		char* dumpBuffer = new char[strlen(buffer) + strlen("\n") + 1];
		strcpy_s(dumpBuffer, strlen(buffer) + strlen("\n") + 1, buffer);
		strcat_s(dumpBuffer, strlen(buffer) + strlen("\n") + 1, "\n");

		if (!fileHandler.appendToFile(hFile, dumpBuffer, strlen(dumpBuffer))) {
			InternetCloseHandle(hGetRequest);
			CloseHandle(hFile);
			return FALSE;
		}
		free(dumpBuffer);
		free(newPath);
	}

	CloseHandle(hFile);
	InternetCloseHandle(hGetRequest);
	
	return TRUE;
}


BOOL Manager::downloadConfigFile()
{
	this->LOG("Trying to access resource from route ");
	this->LOG(this->relativePath, TRUE);
	this->LOG("", TRUE);

	HINTERNET hRequest = client.get(this->relativePath);

	if (!client.isStatusCodeValid(hRequest))
	{
		InternetCloseHandle(hRequest);
		this->lastError = "Failed to get request. Error code: " + to_string(GetLastError());
		return FALSE;
	}

	this->LOG("", TRUE);
	this->LOG("Request successful !", TRUE);
	
	if (hRequest == NULL) { return FALSE; }

	char buffer[4096];
	DWORD bytesRead;
	
	this->LOG("Creating and writing response to myconfig.txt", TRUE);
	HANDLE configHandle = fileHandler.createFile(fileHandler.getPath(MY_CONFIG));
	if (configHandle == INVALID_HANDLE_VALUE) { return FALSE; }

	while (InternetReadFile(hRequest, buffer, sizeof(buffer), &bytesRead) && bytesRead != 0) {
		if (!fileHandler.writeToFile(configHandle, buffer, bytesRead)) { 
			this->lastError = "Failed to write response. Error code: " + to_string(GetLastError());
			CloseHandle(configHandle);
			InternetCloseHandle(hRequest);
			return FALSE; 
		};
	}

	CloseHandle(configHandle);
	InternetCloseHandle(hRequest);
	this->LOG("Content successfully written to myconfig.txt", TRUE);
	return TRUE;
}


void Manager::LOG(LPCSTR buffer)
{
	int length = GetWindowTextLength(logger);
	SendMessage(logger, EM_SETSEL, (WPARAM)length, (LPARAM)length);
	SendMessage(logger, EM_REPLACESEL, FALSE, (LPARAM)buffer);
}

void Manager::LOG(LPSTR buffer)
{
	int length = GetWindowTextLength(logger);
	SendMessage(logger, EM_SETSEL, (WPARAM)length, (LPARAM)length);
	SendMessage(logger, EM_REPLACESEL, FALSE, (LPARAM)buffer);
}

void Manager::LOG(LPCSTR buffer, BOOL addNewLine)
{
	int length = GetWindowTextLength(logger);
	SendMessage(logger, EM_SETSEL, (WPARAM)length, (LPARAM)length);
	SendMessage(logger, EM_REPLACESEL, FALSE, (LPARAM)buffer);
	if (addNewLine) {
		SendMessage(logger, EM_REPLACESEL, FALSE, (LPARAM)"\r\n");
	}
}

void Manager::LOG(LPSTR buffer, BOOL addNewLine)
{
	int length = GetWindowTextLength(logger);
	SendMessage(logger, EM_SETSEL, (WPARAM)length, (LPARAM)length);
	SendMessage(logger, EM_REPLACESEL, FALSE, (LPARAM)buffer);
	if (addNewLine) {
		SendMessage(logger, EM_REPLACESEL, FALSE, (LPARAM)"\r\n");
	}
}
