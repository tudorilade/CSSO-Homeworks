#include "EvaluatePerfomancesCommands.h"

//
// Definition of BitmapHeadersInfo struct
//


/**
* Helper method used for displaying all informations about bitmap contained within headers
*/
string BitmapHeadersInfo::toString() {
	stringstream ss;

	ss << "BITMAPFILEHEADER:\r\n";
	ss << "  Type: " << bfHeader.bfType << "\r\n";
	ss << "  Size: " << bfHeader.bfSize << " bytes\r\n";
	ss << "  Reserved1: " << bfHeader.bfReserved1 << "\r\n";
	ss << "  Reserved2: " << bfHeader.bfReserved2 << "\r\n";
	ss << "  OffBits: " << bfHeader.bfOffBits << "\r\n";

	ss << "BITMAPINFOHEADER:\r\n";
	ss << "  Size: " << biHeader.biSize << "\r\n";
	ss << "  Width: " << biHeader.biWidth << "\r\n";
	ss << "  Height: " << biHeader.biHeight << "\r\n";
	ss << "  Planes: " << biHeader.biPlanes << "\r\n";
	ss << "  BitCount: " << biHeader.biBitCount << "\r\n";
	ss << "  Compression: " << biHeader.biCompression << "\r\n";
	ss << "  SizeImage: " << biHeader.biSizeImage << "\r\n";
	ss << "  XPelsPerMeter: " << biHeader.biXPelsPerMeter << "\r\n";
	ss << "  YPelsPerMeter: " << biHeader.biYPelsPerMeter << "\r\n";
	ss << "  ClrUsed: " << biHeader.biClrUsed << "\r\n";
	ss << "  ClrImportant: " << biHeader.biClrImportant << "\r\n";

	return ss.str();
}


//
// Definition of helper thread function 
//

DWORD WINAPI ThreadFunction(LPVOID lpParam) {
	ThreadData* data = (ThreadData*)lpParam;
	if (data->greyCommand)
		data->staticObj->executeGrey(data->result, data->imageChunk);
	else
		data->staticObj->executeInverse(data->result, data->imageChunk);
	return 0;
}


// 
// Definition of Command Required Info
//

void CommandRequiredInfo::setInfo(
	DirFileHandler fHandler,
	wstring greyPath,
	wstring inversePath,
	wstring imageName, 
	wstring imagePath,
	bHeaders bHeader
)
{
	this->fileHandler = fHandler;
	this->startingByteForPixels = bHeader.bfHeader.bfOffBits;
	this->greyDirPath = greyPath;
	this->inverseDirPath = inversePath;
	this->imageName = imageName;
	this->imagePath = imagePath;
	this->headerBitmap = bHeader;
}

/**
* Method responsible for loading the image in a vector of pixels
*/
void CommandRequiredInfo::loadImage(std::vector<Pixel>& pixels) {
	HANDLE hFile = CreateFileW(imagePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return;
	}

	SetFilePointer(hFile, startingByteForPixels, NULL, FILE_BEGIN);

	const DWORD bufferSize = 0x4000;
	BYTE buffer[bufferSize];
	DWORD bytesRead;

	while (ReadFile(hFile, buffer, bufferSize, &bytesRead, NULL) && bytesRead > 0) {
		for (DWORD i = 0; i < bytesRead; i += 4) {
			Pixel pixel;
			pixel.red = buffer[i];
			pixel.green = buffer[i + 1];
			pixel.blue = buffer[i + 2];
			pixel.alpha = buffer[i + 3];
			pixels.emplace_back(pixel);
		}
	}
	CloseHandle(hFile);
}

SECURITY_ATTRIBUTES* CommandRequiredInfo::setupSecurityDescriptor()
{
	SECURITY_ATTRIBUTES* sa = new SECURITY_ATTRIBUTES;
	if (!sa) {
		return NULL;
	}

	SECURITY_DESCRIPTOR* sd = new SECURITY_DESCRIPTOR;
	if (!sd) {
		delete sa;
		return NULL;
	}

	HANDLE hToken = NULL;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		delete sd;
		delete sa;
		return NULL;
	}

	DWORD dwBufferSize = 0;
	GetTokenInformation(hToken, TokenUser, NULL, 0, &dwBufferSize);
	PTOKEN_USER pTokenUser = (PTOKEN_USER)malloc(dwBufferSize);
	if (!pTokenUser) {
		CloseHandle(hToken);
		delete sd;
		delete sa;
		return NULL; // Memory allocation failed
	}

	if (!GetTokenInformation(hToken, TokenUser, pTokenUser, dwBufferSize, &dwBufferSize)) {
		CloseHandle(hToken);
		free(pTokenUser);
		delete sd;
		delete sa;
		return NULL;
	}

	// Set up the explicit DACL
	EXPLICIT_ACCESS ea;
	ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
	ea.grfAccessPermissions = GENERIC_READ;
	ea.grfAccessMode = SET_ACCESS;
	ea.grfInheritance = NO_INHERITANCE;
	ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea.Trustee.TrusteeType = TRUSTEE_IS_USER;
	ea.Trustee.ptstrName = (LPTSTR)pTokenUser->User.Sid;

	PACL pACL = NULL;
	if (SetEntriesInAcl(1, &ea, NULL, &pACL) != ERROR_SUCCESS) {
		if (pACL) LocalFree(pACL);
		CloseHandle(hToken);
		free(pTokenUser);
		delete sd;
		delete sa;
		return NULL;
	}

	if (!InitializeSecurityDescriptor(sd, SECURITY_DESCRIPTOR_REVISION) ||
		!SetSecurityDescriptorDacl(sd, TRUE, pACL, FALSE)) {
		if (pACL) LocalFree(pACL);
		CloseHandle(hToken);
		free(pTokenUser);
		delete sd;
		delete sa;
		return NULL;
	}

	sa->nLength = sizeof(SECURITY_ATTRIBUTES);
	sa->lpSecurityDescriptor = sd;
	sa->bInheritHandle = FALSE;

	// Clean up
	CloseHandle(hToken);
	free(pTokenUser);
	if (pACL) LocalFree(pACL);

	return sa;
}

/**
* Method responsible for creating a new image given the name of image and the pixels as input
*/
BOOL CommandRequiredInfo::writeImage(wstring imgName, vector<Pixel>& pixels) {
	
	DWORD totalSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + pixels.size() * sizeof(Pixel);

	SECURITY_ATTRIBUTES* sa = this->setupSecurityDescriptor();
	if (sa == NULL)
		return FALSE;


	HANDLE hFile = CreateFileW(imgName.c_str(), GENERIC_WRITE | GENERIC_READ, 0, sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	HANDLE hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, totalSize, NULL);
	if (hMapFile == NULL) {
		CloseHandle(hFile);
		return FALSE;
	}

	LPVOID pBuf = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, totalSize);
	if (pBuf == NULL) {
		CloseHandle(hMapFile);
		CloseHandle(hFile);
		return FALSE;
	}

	char* pBuffer = static_cast<char*>(pBuf);
	memcpy(pBuffer, &this->headerBitmap.bfHeader, sizeof(BITMAPFILEHEADER));
	pBuffer += sizeof(BITMAPFILEHEADER);
	memcpy(pBuffer, &this->headerBitmap.biHeader, sizeof(BITMAPINFOHEADER));
	pBuffer += sizeof(BITMAPINFOHEADER);
	memcpy(pBuffer, pixels.data(), pixels.size() * sizeof(Pixel));

	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
	CloseHandle(hFile);
	return TRUE;
}

/**
* Method responsible for converting a pixel in a grey pixel
*/
Pixel GreyScale::operation(Pixel& originalPixel)
{
	Pixel greyPixel;
	uint8_t gray = static_cast<uint8_t>(0.299 * originalPixel.red + 0.587 * originalPixel.green + 0.114 * originalPixel.blue);
	greyPixel.red = greyPixel.green = greyPixel.blue = gray;
	greyPixel.alpha = originalPixel.alpha;
	return greyPixel;
}

/**
* Command responsible for converting a pixel in a grey one and placing it in a result vector
*/
void GreyScaleCommand::execute(vector<Pixel>& originalPixels, vector<Pixel>& resultPixel)
{
	for (Pixel& pixel : originalPixels)
	{
		resultPixel.emplace_back(grey.operation(pixel));
	}
}

void GreyScaleCommand::execute(Pixel& originalPixels, vector<Pixel>& resultPixel)
{
	resultPixel.emplace_back(grey.operation(originalPixels));
}


/**
* Method responsible for inversing a pixel
*/
Pixel InverseScale::operation(Pixel& originalPixel)
{
	Pixel inversedPixel;
	inversedPixel.red = 0xFF - originalPixel.red;
	inversedPixel.blue= 0xFF - originalPixel.blue;
	inversedPixel.green = 0xFF - originalPixel.green;
	inversedPixel.alpha = originalPixel.alpha;
	return inversedPixel;
}

/**
* Command responsible for converting a pixel in a grey one and placing it in a result vector
*/
void InverseScaleCommand::execute(vector<Pixel>& originalPixels, vector<Pixel>& resultPixels)
{
	for (Pixel& pixel : originalPixels)
	{
		resultPixels.emplace_back(inverse.operation(pixel));
	}
}

void InverseScaleCommand::execute(Pixel& originalPixels, vector<Pixel>& resultPixel)
{
	resultPixel.emplace_back(inverse.operation(originalPixels));
}


Base::Base(cmdInfo cInfo)
{
	this->cInfo = cInfo;
}

void Base::loadImage()
{
	this->cInfo.loadImage(this->imagePixels);
	this->sizeImagePixels = this->imagePixels.size();
}

BOOL Base::failedToLoadImage()
{
	return this->imagePixels.size() == 0;
}


BOOL Base::writeGreyImage(vector<Pixel>& resultGrey, wstring time, evPerfResults& res)
{
	wstring greyImageName = this->cInfo.greyDirPath + L"\\" + this->cInfo.imageName + L"_grey_" + time + L".bmp";
	res.greyPath = greyImageName;
	if (!this->cInfo.writeImage(greyImageName, resultGrey))
		return FALSE;
	
	return TRUE;
}

BOOL Base::writeInverseImage(vector<Pixel>& resultInverse, wstring time, evPerfResults& res)
{
	wstring inverse = this->cInfo.inverseDirPath + L"\\" + this->cInfo.imageName + L"_inverse_" + time + L".bmp";
	res.inversePath = inverse;
	if (!this->cInfo.writeImage(inverse, resultInverse))
		return FALSE;

	return TRUE;
}

int Base::getNumberOfPhysicalProcessors() {
	DWORD length = 0;
	BOOL result = GetLogicalProcessorInformation(nullptr, &length);
	if (result || GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
		return -1;
	}

	std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer(length / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
	result = GetLogicalProcessorInformation(buffer.data(), &length);
	if (!result) {
		return -1;
	}

	int physicalProcessorCount = 0;
	for (const auto& info : buffer) {
		if (info.Relationship == RelationProcessorCore) {
			physicalProcessorCount++;
		}
	}
	return physicalProcessorCount * 2;
}

size_t Base::getSizeOfImage() {
	return sizeImagePixels;
}


//
// Implementation of Sequential and Sequential command
//

Sequential::Sequential(cmdInfo cmd) : Base(cmd) {};


void Sequential::executeGrey(vector<Pixel>& resultGrey)
{
	for (Pixel& orgPixel : this->imagePixels)
	{
		this->greyScaleCommand.execute(orgPixel, resultGrey);
	}
}

void Sequential::executeInverse(vector<Pixel>& resultInverse)
{
	for (Pixel& orgPixel : this->imagePixels)
	{
		this->inverseCommand.execute(orgPixel, resultInverse);
	}
}

SequentialCommand::SequentialCommand(cmdInfo cmd) : Command()
{
	this->sequential = Sequential(cmd);
}

void SequentialCommand::execute(evPerfResults& evRes)
{
	vector<Pixel> resultGrey, resultInverse;
	
	this->sequential.loadImage();

	if (this->sequential.failedToLoadImage())
	{
		evRes.lastError = L"Couldn't load the image for processing. Error code: " + to_wstring(GetLastError());
		return;
	}

	// Executing grey operation
	auto start = std::chrono::high_resolution_clock::now();
	this->sequential.executeGrey(resultGrey);
	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double, std::micro> sequentialTime = end - start;
	evRes.grayScaleTiming = to_wstring((int)sequentialTime.count()) + L"ms";
	if (!this->sequential.writeGreyImage(resultGrey, evRes.grayScaleTiming, evRes))
		evRes.lastError = L"Couldn't create the grey image. Error code: " + to_wstring(GetLastError());

	resultGrey.clear();

	// Executing inverse operation
	start = std::chrono::high_resolution_clock::now();
	this->sequential.executeInverse(resultInverse);
	end = std::chrono::high_resolution_clock::now();
	sequentialTime = end - start;
	evRes.inverseScaleTiming = to_wstring((int)sequentialTime.count()) + L"ms";

	if (!this->sequential.writeInverseImage(resultInverse, evRes.inverseScaleTiming, evRes))
		evRes.lastError = L"Couldn't create the grey image. Error code: " + to_wstring(GetLastError());
	resultInverse.clear();
}


//
// Implementation of Static and Static command
//

Static::Static(cmdInfo cmd) : Base(cmd) {};


void Static::executeGrey(vector<Pixel>& resultGrey, vector<Pixel>& imageChunk)
{
	for (Pixel& orgPixel : imageChunk)
	{
		this->greyScaleCommand.execute(orgPixel, resultGrey);
	}
}

void Static::executeInverse(vector<Pixel>& resultInverse, vector<Pixel>& imageChunk)
{
	for (Pixel& orgPixel : imageChunk)
	{
		this->inverseCommand.execute(orgPixel, resultInverse);
	}
}

StaticCommand::StaticCommand(cmdInfo cmd) : Command()
{
	this->staticc = Static(cmd);
}

void StaticCommand::execute(evPerfResults& evRes)
{
	vector<Pixel> resultGrey, resultInverse;

	this->staticc.loadImage();

	if (this->staticc.failedToLoadImage())
	{
		evRes.lastError = L"Couldn't load the image for processing. Error code: " + to_wstring(GetLastError());
		return;
	}

	int nrWorkers = this->staticc.getNumberOfPhysicalProcessors();
	if (nrWorkers < 0)
	{
		evRes.lastError = L"Couldn't get the number of processors. Error code: " + to_wstring(GetLastError());
		return;
	}

	// Executing grey operation
	auto start = std::chrono::high_resolution_clock::now();
	this->processParallel(resultGrey, nrWorkers, true, false);
	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double, std::micro> sequentialTime = end - start;
	evRes.grayScaleTiming = to_wstring((int)sequentialTime.count()) + L"ms";
	if (!this->staticc.writeGreyImage(resultGrey, evRes.grayScaleTiming, evRes))
		evRes.lastError = L"Couldn't create the grey image. Error code: " + to_wstring(GetLastError());

	resultGrey.clear();

	// Executing inverse operation
	start = std::chrono::high_resolution_clock::now();
	this->processParallel(resultInverse, nrWorkers, false, true);
	end = std::chrono::high_resolution_clock::now();
	sequentialTime = end - start;
	evRes.inverseScaleTiming = to_wstring((int)sequentialTime.count()) + L"ms";

	if (!this->staticc.writeInverseImage(resultInverse, evRes.inverseScaleTiming, evRes))
		evRes.lastError = L"Couldn't create the grey image. Error code: " + to_wstring(GetLastError());
	resultInverse.clear();
}

void StaticCommand::processParallel(vector<Pixel>& finalResult, int nrWorkers, bool processGrey, bool processInverse)
{
	vector<HANDLE> hThreads(nrWorkers);
	vector<ThreadData> threadData(nrWorkers);
	int chunkSize = this->staticc.getSizeOfImage() / nrWorkers;

	// divide the image and spwan threads
	for (int i = 0; i < nrWorkers; ++i) {
		int start = i * chunkSize;
		int end = (i == nrWorkers - 1) ? this->staticc.getSizeOfImage() : (i + 1) * chunkSize;

		threadData[i].staticObj = &this->staticc;
		threadData[i].imageChunk.assign(
			this->staticc.imagePixels.begin() + start,
			this->staticc.imagePixels.begin() + end
		);
		threadData[i].greyCommand = processGrey;
		threadData[i].inverseCommand = processInverse;
		hThreads[i] = CreateThread(NULL, 0, ThreadFunction, &threadData[i], 0, NULL);
	}

	// wait for all threads to complete
	WaitForMultipleObjects(nrWorkers, hThreads.data(), TRUE, INFINITE);

	// combine results
	for (int i = 0; i < nrWorkers; ++i) {
		finalResult.insert(finalResult.end(), threadData[i].result.begin(), threadData[i].result.end());
		CloseHandle(hThreads[i]);
	}
}


//
// Implementation of Dynamic and Dynamic command
//

Dynamic::Dynamic(cmdInfo cmd) : Base(cmd) {};


void Dynamic::executeGrey(vector<Pixel>& resultGrey, vector<Pixel>& imageChunk)
{
	for (Pixel& orgPixel : imageChunk)
	{
		this->greyScaleCommand.execute(orgPixel, resultGrey);
	}
}

void Dynamic::executeInverse(vector<Pixel>& resultInverse, vector<Pixel>& imageChunk)
{
	for (Pixel& orgPixel : imageChunk)
	{
		this->inverseCommand.execute(orgPixel, resultInverse);
	}
}

DynamicCommand::DynamicCommand(cmdInfo cmd) : Command()
{
	this->dynamic = Dynamic(cmd);
}

void DynamicCommand::execute(evPerfResults& evRes)
{
	return;

	// Am pus aici return, doar ca sa nu se ruleze asta; cand crezi ca ai facut implementarea sa 
	// proceseze dynamic aici, scoate return ul de sus.
	vector<Pixel> resultGrey, resultInverse;

	this->dynamic.loadImage();

	if (this->dynamic.failedToLoadImage())
	{
		evRes.lastError = L"Couldn't load the image for processing. Error code: " + to_wstring(GetLastError());
		return;
	}

	int nrWorkers = this->dynamic.getNumberOfPhysicalProcessors();
	if (nrWorkers < 0)
	{
		evRes.lastError = L"Couldn't get the number of processors. Error code: " + to_wstring(GetLastError());
		return;
	}

	// Executing grey operation
	auto start = std::chrono::high_resolution_clock::now();
	this->processParallel(resultGrey, nrWorkers, true, false);
	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double, std::micro> sequentialTime = end - start;
	evRes.grayScaleTiming = to_wstring((int)sequentialTime.count()) + L"ms";
	if (!this->dynamic.writeGreyImage(resultGrey, evRes.grayScaleTiming, evRes))
		evRes.lastError = L"Couldn't create the grey image. Error code: " + to_wstring(GetLastError());

	resultGrey.clear();

	// Executing inverse operation
	start = std::chrono::high_resolution_clock::now();
	this->processParallel(resultInverse, nrWorkers, false, true);
	end = std::chrono::high_resolution_clock::now();
	sequentialTime = end - start;
	evRes.inverseScaleTiming = to_wstring((int)sequentialTime.count()) + L"ms";

	if (!this->dynamic.writeInverseImage(resultInverse, evRes.inverseScaleTiming, evRes))
		evRes.lastError = L"Couldn't create the grey image. Error code: " + to_wstring(GetLastError());
	resultInverse.clear();
}

void DynamicCommand::processParallel(vector<Pixel>& finalResult, int nrWorkers, bool processGrey, bool processInverse)
{
	// TODO: to be implemented as for dynamic way
	// Aici trebuie sa tinem urmatoarele lucruri in vedere: 
	// 1. Pixelii trebuie sa fie ordonati. noi avem load balancing, deci cand un thread termina, parintele 
	// trimite un nou chunk la procesare. nestiind cand termina primul thread, ordinea nu este garantata (Adica toti pixelii sunt ordonati, ca sa rezulte imaginea initiala)
	// deci trebuie sa avem grija cum tinem ordinea respectiva. am mai vazut si cu chatgpt, da niste sugestii bune.
	// in rest, ar trebui sa fie destul de similar cu Static, cu singura conditie ca vom avea Eventuri (Setevent din alea), 
	// ca fiecare thread sa semnalizeze cand si-a terminat treaba pt a putea primi chunk uri de procesare.
	// asta-i tot. Daca e facuta corect procesarea asta, ar trebui sa vezi rezultatul.
}
