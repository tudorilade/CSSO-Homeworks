#pragma once
using namespace std;
#include <tchar.h>
#include<windows.h>
#include<vector>
#include "ProductInfo.h"
#include "FileManipulation.h"


class Deposit : FileManipulation{

public:
    Deposit() {};

    void startProccessing(LPCSTR);
    int proccessFile(LPCSTR);
    vector<ProductInfo> processLines(HANDLE);
    ProductInfo processLineAndGetProduct(const char*);
    vector<string> preprocessingFiles(LPCSTR);

};