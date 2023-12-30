#pragma once
#include "Manager.h"


typedef struct EvaluateInput{
	wstring imagePath;
	wstring greyPath;
	wstring inversePath;
	string typeOfTest;
	DWORD testType;

	void clear();
	void getTypeOfTest();
	void getTypeOfTest(HWND hWnd);

} evInput;


class EvaluatePerformancesManager : public Manager
{
private:
	evInput bitmapInput;
	ShowImages showImages;

public:
	EvaluatePerformancesManager() : Manager() {};
	EvaluatePerformancesManager(HWND, evInput, ShowImages&);

	void execute() override;
	BOOL static isInputValid(evInput&);
};
