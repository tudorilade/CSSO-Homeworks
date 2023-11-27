#pragma once
using namespace std;
#include "BaseChildProcesses.h"

class Deposit : public BaseChildProcesses {

public:
    Deposit() : BaseChildProcesses(TRUE) {}

    virtual int proccessFile(LPCSTR) override;
    virtual ProductInfo processLineAndGetProduct(const char*) override;

    vector<ProductInfo> processLines(HANDLE);
    int handleMappedFiles(LPVOID, LPVOID, LPVOID, ProductInfo);

};