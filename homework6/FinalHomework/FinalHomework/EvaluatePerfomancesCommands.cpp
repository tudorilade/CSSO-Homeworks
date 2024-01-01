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

/**
* Method responsible for creating a new image given the name of image and the pixels as input
*/
BOOL CommandRequiredInfo::writeImage(wstring imgName, vector<Pixel>& pixels) {
	
	DWORD totalSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + pixels.size() * sizeof(Pixel);

	HANDLE hFile = CreateFileW(imgName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
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


Sequential::Sequential(cmdInfo cInfo)
{
	this->cInfo = cInfo;
}

void Sequential::loadImage()
{
	this->cInfo.loadImage(this->imagePixels);
}


BOOL Sequential::writeGreyImage(vector<Pixel>& resultGrey, wstring time, evPerfResults& res)
{
	wstring greyImageName = this->cInfo.greyDirPath + L"\\" + this->cInfo.imageName + L"_grey_" + time + L".bmp";
	res.greyPath = greyImageName;
	if (!this->cInfo.writeImage(greyImageName, resultGrey))
		return FALSE;
	
	return TRUE;
}

BOOL Sequential::writeInverseImage(vector<Pixel>& resultInverse, wstring time, evPerfResults& res)
{
	wstring inverse = this->cInfo.inverseDirPath + L"\\" + this->cInfo.imageName + L"_inverse_" + time + L".bmp";
	res.inversePath = inverse;
	if (!this->cInfo.writeImage(inverse, resultInverse))
		return FALSE;

	return TRUE;
}

BOOL Sequential::failedToLoadImage()
{
	return this->imagePixels.size() == 0;
}

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
