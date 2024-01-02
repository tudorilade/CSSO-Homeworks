#pragma once
#include "Manager.h"
#include "EvaluatePerfomancesCommands.h"
#include <sstream>
#include <string>


typedef struct EvaluateInput{
	wstring imagePath;
	wstring greyPath;
	wstring inversePath;
	wstring imageName;
	string typeOfTest;
	DWORD testType;

	void clear();
	void getTypeOfTest();
	void getTypeOfTest(HWND hWnd);
	void adjustPaths(DirFileHandler&);
	void setImageName();

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
	void displayBitmapHeaders();
	void runTests();
	void invokeTest(evPerfResults&, cmdInfo&, DWORD);
	void displayExecutionTime(evPerfResults&);

	BOOL setupEvaluateDirAndFiles();
	BOOL static isInputValid(evInput&);
};
