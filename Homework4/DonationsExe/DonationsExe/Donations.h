#pragma once
#include "BaseChildProcesses.h"

class Donations : public BaseChildProcesses
{
private:
	int total = 0;
public:
	Donations() {};
	virtual ProductInfo processLineAndGetProduct(const char*) override;
	virtual int proccessFile(LPCSTR) override;
	DWORD writeTotalSales();
};

