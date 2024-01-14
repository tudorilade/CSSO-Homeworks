#pragma once
#include "EvaluatePerformancesManager.h"

typedef struct executionTime {
    wstring staticTime;
    wstring dinamicTime;
}execTime;

int startComparing(evInput& input, execTime& times) {
    input.setStaticImageName();
    input.setDinamicImageName();
    size_t pozLast_inStatic = input.staticName.find_last_of(L"_");
    size_t pozLast_inDinamic = input.dinamicName.find_last_of(L"_");

    wstring extractedNumberStatic = input.staticName.substr(pozLast_inStatic + 1);
    extractedNumberStatic.erase(extractedNumberStatic.length() - 2);
    
    wstring extractedNumberDinamic = input.dinamicName.substr(pozLast_inDinamic + 1);
    extractedNumberDinamic.erase(extractedNumberDinamic.length() - 2);
    
    times.staticTime = extractedNumberStatic;
    times.dinamicTime = extractedNumberDinamic;
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

void loadImageForCompare(std::vector<Pixel>& pixels, wstring imagePath) {
    HANDLE hFile = CreateFileW(imagePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return;
    }

    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

    const DWORD bufferSize = 0x4000;
    BYTE buffer[bufferSize];
    DWORD bytesRead;

    while (ReadFile(hFile, buffer, bufferSize, &bytesRead, NULL) && bytesRead > 0) {
        for (DWORD i = 0; i < bytesRead; i += 4) {
            Pixel pixel;
            pixel.red = buffer[i];
            pixel.green = buffer[i + 1];
            pixel.blue = buffer[i + 2];
            pixel.alpha = buffer[i + 3];
            pixels.emplace_back(pixel);
        }
    }
    CloseHandle(hFile);
}

bool comparePixelVectors(const std::vector<Pixel>& staticImage, const std::vector<Pixel>& dinamicImage) {
    // Check if the sizes are the same
    if (staticImage.size() != dinamicImage.size()) {
        return false;
    }
    for (std::size_t i = 0; i < staticImage.size(); ++i) {
        if (staticImage[i].blue != dinamicImage[i].blue || staticImage[i].red != dinamicImage[i].red || staticImage[i].green != dinamicImage[i].green) {
            return false;//Imagini diferite
        }
    }
    return true;
}