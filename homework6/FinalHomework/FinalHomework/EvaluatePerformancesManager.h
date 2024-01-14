#pragma once
#include "Manager.h"
#include "EvaluatePerfomancesCommands.h"
#include <sstream>
#include <string>


typedef struct EvaluateInput{
	wstring imagePath;
	wstring static_imagePath;
	wstring dinamic_imagePath;
	wstring greyPath;
	wstring inversePath;
	wstring imageName;
	wstring staticName;
	wstring dinamicName;

	string typeOfTest;
	DWORD testType;

	void clear();
	void getTypeOfTest();
	void getTypeOfTest(HWND hWnd);
	void adjustPaths(DirFileHandler&);
	void setImageName();
	void setStaticImageName();
	void setDinamicImageName();

	BOOL extractBitmapInfo(bHeaders&);
} evInput;


class EvaluatePerformancesManager : public Manager
{
private:
	evInput bitmapInput;
	ShowImages* showImages;
	bHeaders bHeadersInfo;
	Command* command;
	HINSTANCE hInst;

public:
	EvaluatePerformancesManager() : Manager() {};
	EvaluatePerformancesManager(HWND, HINSTANCE, evInput, ShowImages&);

	void execute() override;
	void startComparing();
	void displayBitmapHeaders();
	void runTests();
	void invokeTest(evPerfResults&, cmdInfo&, DWORD);
	void displayExecutionTime(evPerfResults&);

	BOOL setupEvaluateDirAndFiles();
	BOOL static isInputValid(evInput&);
};
