#pragma once
#include <tchar.h>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include<windows.h>
#include <stdio.h>

#include "FileManipulation.h"
#include "ProductInfo.h"
#include "../../../tema4/tema4/constants.h"


using namespace std;


class BaseChildProcesses : public FileManipulation
{
protected:
    BOOL isDeposit = FALSE;

public:
    BaseChildProcesses() {};
    BaseChildProcesses(BOOL dep) { isDeposit = dep; }
    void startProccessing(LPCSTR);
    vector<string> preprocessingFiles(LPCSTR);
    BOOL getDepositFlag();
    char* constructPath(const char*, const char*);
    static bool compareFileNames(string&, string&);
    HANDLE getHandleForMappedFile(LPCSTR);

    virtual ProductInfo processLineAndGetProduct(const char*) = 0;
    virtual int proccessFile(LPCSTR) = 0;

};

