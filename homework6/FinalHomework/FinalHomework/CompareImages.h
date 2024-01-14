#pragma once
#include "EvaluatePerformancesManager.h"

int startComparing(evInput& input) {
    input.setStaticImageName();
    input.setDinamicImageName();
    size_t pozLast_inStatic = input.staticName.find_last_of(L"_");
    size_t pozLast_inDinamic = input.dinamicName.find_last_of(L"_");

    wstring extractedNumberStatic = input.staticName.substr(pozLast_inStatic + 1);
    extractedNumberStatic.erase(extractedNumberStatic.length() - 2);
    
    wstring extractedNumberDinamic = input.dinamicName.substr(pozLast_inDinamic + 1);
    extractedNumberDinamic.erase(extractedNumberDinamic.length() - 2);
    
    if (extractedNumberStatic.length() < extractedNumberDinamic.length())
        return 1;
    else if(extractedNumberStatic.length() > extractedNumberDinamic.length())
        return 2;
    else {
        int i = 0;
        int oneNumerStatic = extractedNumberStatic[0] - L'0';
        int oneNumberDinamic = extractedNumberDinamic[0] - L'0';
        while (i < extractedNumberStatic.length() + 1) {
            if (oneNumerStatic < oneNumberDinamic)
                return 1;
            else if (oneNumerStatic > oneNumberDinamic)
                return 2;
            else {
                i++;
                oneNumerStatic = extractedNumberStatic[i] - L'0';
                oneNumberDinamic = extractedNumberDinamic[i] - L'0';
            }
        }        
    }
}