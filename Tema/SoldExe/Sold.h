#pragma once

#include "BaseChildProcesses.h"

using namespace std;


class Sold : public BaseChildProcesses {
private:
	INT32 totalSales = 0;
	
public:

	Sold() : BaseChildProcesses(FALSE) {};


	virtual ProductInfo processLineAndGetProduct(const char*) override;
	virtual int proccessFile(LPCSTR) override;
	vector <ProductInfo> processLines(HANDLE);
	DWORD handleMappedFiles(LPVOID, LPVOID, LPVOID, ProductInfo&);
	DWORD writeTotalSales();

};

