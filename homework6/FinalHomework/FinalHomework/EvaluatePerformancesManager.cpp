#include "EvaluatePerformancesManager.h"

EvaluatePerformancesManager::EvaluatePerformancesManager(HWND hWnd, evInput input, ShowImages& showImages) : Manager(hWnd)
{
	this->bitmapInput = input;
	this->logger = GetDlgItem(hWnd, ID_LOG_WINDOW_E);
	this->showImages = showImages;
}


void EvaluatePerformancesManager::execute()
{
	this->LOG(this->logger, "starting to execute tests...", TRUE);
}

/**
* Helper method used to verify whether all paths have been provided or not;
* Apropiate message is displayed otherwise
*/
BOOL EvaluatePerformancesManager::isInputValid(evInput& input)
{
	if (input.greyPath.empty())
		return FALSE;

	if (input.imagePath.empty())
		return FALSE;

	if (input.inversePath.empty())
		return FALSE;

	if (input.typeOfTest.empty())
		return FALSE;

	return TRUE;
}

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
