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

	this->client = Client();

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

vector<RequestType> Manager::processLines() {

	//open wanted file which holds the requests information
	HANDLE hReqFile = CreateFile(
		"C:\\Facultate\\CSSO\\Week5\\myconfig.txt",
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	vector<RequestType> temp_Reqs;
	//go line by line
	DWORD dwBytesRead;
	const DWORD bufferSize = 1024;
	char buffer[bufferSize];

	char currentLine[bufferSize];
	size_t currentLineIndex = 0;

	while (ReadFile(hReqFile, buffer, bufferSize - 1, &dwBytesRead, NULL) && dwBytesRead > 0) {
		buffer[dwBytesRead] = '\0'; // Null-terminate the buffer

		for (char* p = buffer; *p; ++p) {
			if (*p == '\n') {
				currentLine[currentLineIndex] = '\0';

				///add to vector of products 
				/*ProductInfo product = processLineAndGetProduct(currentLine);
				if (product.getIdProduct() != -1) {
					// Only add valid products to the vector
					productInformation.push_back(product);
				}*/
				this->LOG(currentLine);
				this->LOG("\r\n");
				RequestType request = this->processLineAndGetRequest(currentLine);
				this->LOG(request.getPath());
				this->LOG("\r\n");
				if (request.getReqType() != -1) {
					//add a valid request
					temp_Reqs.push_back(request);
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
		temp_Reqs.push_back(processLineAndGetRequest(currentLine));
	}
	CloseHandle(hReqFile);
	return temp_Reqs;

}

RequestType Manager::processLineAndGetRequest(const char* line) {
	int id,reqType;
	char path[50];

	int requestTypeInt = -1;
	const char* getPtr = strstr(line, "GET:");
	const char* postPtr = strstr(line, "POST:");
	const char* pathPtr = strstr(line, ".com/");

	if (getPtr != NULL) {
		reqType = 1; // 1 pentru GET
		getPtr += strlen("GET:");
	}
	else if (postPtr != NULL) {
		reqType = 2; //2 pentru POST

	}
	if (pathPtr != NULL) {
		pathPtr += strlen(".com/");
		strncpy_s(path, sizeof(path), pathPtr, sizeof(path) - 2);
	}
	RequestType  r = RequestType(requestTypeInt, path);
	this->LOG(r.getPath());


	return RequestType(requestTypeInt, path);
}
BOOL Manager::startProcessing()
{
	// 1. instantiate client
	// 2. download resource
	// 3. write to myConfig

	this->LOG("Starting session with ");
	this->LOG(this->getResource());
	this->LOG("\r\n");

	client.startSession(this->getResource(), HTTPS_PROTOCOL);

	if (!client.isHandleValid(client.getHandle(SESSION)))
	{
		this->lastError = "Error starting session to internet. Error code: " + to_string(GetLastError());
		return FALSE;
	}

	this->LOG("Session established! \r\n");

	if (!downloadConfigFile())
	{
		this->lastError = "Error downloading the file from " + string(this->relativePath) + ".Error code : " + to_string(GetLastError());
		return FALSE;
	}


	// 4. Read Line by Line from the file until all file is passed
	this->LOG("Started parsing the request file! \r\n");

	vector<RequestType> requests = this->processLines();

	

	for (RequestType& req : requests) {
		this->LOG(req.getPath());
		
	}
	



	return TRUE;
}




BOOL Manager::downloadConfigFile()
{
	this->LOG("Trying to access resource from route ");
	this->LOG(this->relativePath);
	this->LOG("\r\n");

	HINTERNET hRequest = client.get(this->relativePath);

	this->LOG("Request successful !\r\n");
	
	if (hRequest == NULL) { return FALSE; }

	char buffer[4096];
	DWORD bytesRead;
	
	this->LOG("Creating and writing response to myconfig.txt\r\n");
	HANDLE configHandle = fileHandler.createFile(fileHandler.getPath(MY_CONFIG));
	if (configHandle == INVALID_HANDLE_VALUE) { return FALSE; }

	while (InternetReadFile(hRequest, buffer, sizeof(buffer), &bytesRead) && bytesRead != 0) {
		if (!fileHandler.writeToFile(configHandle, buffer, bytesRead)) { return FALSE; };
	}

	CloseHandle(configHandle);
	this->LOG("Content successfully written to myconfig.txt\r\n");
	InternetCloseHandle(hRequest);
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
