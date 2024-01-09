#include "EvaluatePerformancesManager.h"

EvaluatePerformancesManager::EvaluatePerformancesManager(
	HWND hWnd,HINSTANCE hInst, evInput input, ShowImages& showImages
) : Manager(hWnd), showImages(&showImages), hInst(hInst)
{
	this->bitmapInput = input;
	this->logger = GetDlgItem(hWnd, ID_LOG_WINDOW_E);
	this->bitmapInput.adjustPaths(this->fileHandler);
	this->bitmapInput.setImageName();
}


void EvaluatePerformancesManager::execute()
{
	this->LOG(this->logger, "Creating the initial setup....", TRUE);

	if (!this->setupEvaluateDirAndFiles())
	{
		this->LOG(this->logger, "Couln't setup the directories. Error:", FALSE);
		this->LOG(this->logger, this->fileHandler.getLastError().c_str(), TRUE);
		return;
	}
	this->LOG(this->logger, "Successfully created initial setup....", TRUE);

	this->LOG(this->logger, "Extracting bitmap header info....", TRUE);
	
	// collect bmp header info and display
	this->displayBitmapHeaders();
	this->LOG(this->logger, "Begin the execution of tests...", TRUE);
	

	// run tests
	this->runTests();
	this->LOG(this->logger, "Execution of tests is finalized", TRUE);
}

/* Method responsible for runing tests based on user input
*/
void EvaluatePerformancesManager::runTests()
{
	cmdInfo cInfo;
	cInfo.setInfo(
		this->fileHandler,
		this->bitmapInput.greyPath,
		this->bitmapInput.inversePath,
		this->bitmapInput.imageName,
		this->bitmapInput.imagePath,
		this->bHeadersInfo
	);

	evPerfResults timeResults;

	switch (bitmapInput.testType)
	{
	case COMBO_CHOICE_1_INT:
		this->invokeTest(timeResults, cInfo, COMBO_CHOICE_1_INT);
		break;
	case COMBO_CHOICE_2_INT:
		this->invokeTest(timeResults, cInfo, COMBO_CHOICE_2_INT);
		break;
	case COMBO_CHOICE_3_INT:
		this->invokeTest(timeResults, cInfo, COMBO_CHOICE_3_INT);
		break;
	}
}

void EvaluatePerformancesManager::invokeTest(evPerfResults& timeResults, cmdInfo& cInfo, DWORD testType)
{
	switch (testType) {
	case COMBO_CHOICE_1_INT:
		this->command = new StaticCommand(cInfo);
		break;
	case COMBO_CHOICE_2_INT:
		this->command = new DynamicCommand(cInfo);
		break;
	case COMBO_CHOICE_3_INT:
		this->command = new SequentialCommand(cInfo);
		break;
	}

	this->command->execute(timeResults);

	if (!timeResults.lastError.empty())
	{
		this->LOG(logger, timeResults.lastError.c_str(), TRUE);
		return;
	}

	this->showImages->showEvaluateGrayImage = true;
	this->showImages->showEvaluateReverseImage = true;
	this->showImages->loadImage(timeResults.greyPath.c_str(), this->mainWindow, this->hInst, GRAY_IMAGE);
	this->showImages->loadImage(timeResults.inversePath.c_str(), this->mainWindow, this->hInst, INVERSE_IMAGE);
	this->displayExecutionTime(timeResults);
}

void EvaluatePerformancesManager::displayExecutionTime(evPerfResults& timeRes)
{
	HWND hExecTimeGrey = GetDlgItem(this->mainWindow, ID_EXEC_TIME_GREY);
	SetWindowTextW(hExecTimeGrey, timeRes.grayScaleTiming.c_str());

	HWND hExecInverse = GetDlgItem(this->mainWindow, ID_EXEC_TIME_INVERSE);
	SetWindowTextW(hExecInverse, timeRes.inverseScaleTiming.c_str());
}

BOOL EvaluatePerformancesManager::setupEvaluateDirAndFiles()
{
	if (!this->fileHandler.setUpDirAndFiles())
		return FALSE;

	switch (bitmapInput.testType)
	{
	case COMBO_CHOICE_1_INT:
		return this->fileHandler.recursiveCreateDirectory(this->fileHandler.getPath(STATIC_DIR));
		break;
	case COMBO_CHOICE_2_INT:
		return this->fileHandler.recursiveCreateDirectory(this->fileHandler.getPath(DINAMIC_DIR));
		break;
	case COMBO_CHOICE_3_INT:
		return this->fileHandler.recursiveCreateDirectory(this->fileHandler.getPath(SEQUENTIAL_DIR));
		break;
	}

	// this means that directory path was supplied as input
	return TRUE;
}


/**
* Method responsible for invoking the extraction of bitmap header and displaying to the 
* target window
*/
void EvaluatePerformancesManager::displayBitmapHeaders()
{
	HWND hBitMapWindow = GetDlgItem(this->mainWindow, ID_BMP_HEADER);
	if (!this->bitmapInput.extractBitmapInfo(this->bHeadersInfo))
	{
		this->LOG(this->logger, "Couldn't extract the bitmap INFO. Error code: ", FALSE);
		this->LOG(this->logger, to_string(GetLastError()).c_str(), TRUE);
		return;
	}

	SetWindowText(hBitMapWindow, this->bHeadersInfo.toString().c_str());
	this->LOG(this->logger, "Successfully extracted bitmap header info....", TRUE);
}


/**
* Helper method used to verify whether all paths have been provided or not;
* Apropiate message is displayed otherwise
*/
BOOL EvaluatePerformancesManager::isInputValid(evInput& input)
{
	if (input.imagePath.empty())
		return FALSE;

	if (input.typeOfTest.empty())
		return FALSE;

	return TRUE;
}


// 
// Evaluate input definitions
//

void EvaluateInput::clear()
{
	imagePath.clear();
	greyPath.clear();
	inversePath.clear();
	typeOfTest.clear();
}
void EvaluateInput::getTypeOfTest(HWND hWnd)
{
	char test[30];
	memset(test, 0, 30);
	int index = ComboBox_GetCurSel(hWnd);

	if (index != CB_ERR)
	{
		ComboBox_GetLBText(hWnd, index, test);
		this->typeOfTest = string(test);
		this->getTypeOfTest();
	}
	else
	{
		this->typeOfTest.clear();
	}
}

void EvaluateInput::getTypeOfTest()
{
	if (typeOfTest == string(COMBO_CHOICE_1))
	{
		testType = COMBO_CHOICE_1_INT;
	}
	else if (typeOfTest == string(COMBO_CHOICE_2))
	{
		testType = COMBO_CHOICE_2_INT;
	}
	else {
		testType = COMBO_CHOICE_3_INT;
	}
}

void EvaluateInput::adjustPaths(DirFileHandler& fHandler)
{
	DWORD directory;

	switch (testType)
	{
	case COMBO_CHOICE_1_INT:
		directory = STATIC_DIR;
		break;
	case COMBO_CHOICE_2_INT:
		directory = DINAMIC_DIR;
		break;
	default:
		directory = SEQUENTIAL_DIR;
	}

	if (greyPath.empty())
	{
		greyPath = wstring(fHandler.getPath(directory));
	}

	if (inversePath.empty())
	{
		inversePath = wstring(fHandler.getPath(directory));
	}
}


void EvaluateInput::setImageName()
{
	size_t pos = imagePath.find_last_of(L"\\");
	wstring imgWithExtension = imagePath.substr(pos + 1);

	size_t indexDot = imgWithExtension.find(L".");

	if (indexDot != std::wstring::npos) {
		imageName = imgWithExtension.substr(0, indexDot);
	}
	else {
		imageName = imgWithExtension;
	}
}


/**
* Helper method responsible for extracting the bitmap headers information from the photo
*/
BOOL EvaluateInput::extractBitmapInfo(bHeaders& bHeader)
{
	HANDLE hFile = CreateFileW(
		imagePath.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD bytesRead;
	BOOL readSuccess;
	
	// extracting bfHeader
	readSuccess = ReadFile(hFile, &bHeader.bfHeader, sizeof(bHeader.bfHeader), &bytesRead, NULL);
	
	if (!readSuccess || bytesRead != sizeof(bHeader.bfHeader))
	{
		CloseHandle(hFile);
		return FALSE;
	}

	// extracting biHeader
	readSuccess = ReadFile(hFile, &bHeader.biHeader, sizeof(bHeader.biHeader), &bytesRead, NULL);

	if (!readSuccess || bytesRead != sizeof(bHeader.biHeader))
	{
		CloseHandle(hFile);
		return FALSE;
	}

	CloseHandle(hFile);
	return TRUE;
}
