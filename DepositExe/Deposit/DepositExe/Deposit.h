#pragma once
using namespace std;
#include <tchar.h>
#include<windows.h>
#include<vector>
#include "ProductInfo.h"

class Deposit {

public:
    Deposit() {};

    void startProccessing(LPCSTR);
    int CheckForErrors(HANDLE, HANDLE, HANDLE);
    int proccessFirstFile(LPCSTR);
    vector<ProductInfo> processLines(HANDLE);
    ProductInfo processLineAndGetProduct(const char*);

};