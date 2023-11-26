#pragma once
#include "BaseChildProcesses.h"

class Donations : public BaseChildProcesses
{
public:
	Donations() {};
	virtual ProductInfo processLineAndGetProduct(const char*) override;
	virtual int proccessFile(LPCSTR) override;
};

