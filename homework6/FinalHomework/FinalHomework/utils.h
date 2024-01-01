#pragma once
#include <ShObjIdl.h>
#include "windowsx.h"


#define MAIN_IMAGE 0
#define EVALUATE_IMAGE 1
#define GRAY_IMAGE 2
#define INVERSE_IMAGE 3

// global struct
typedef struct ShowImages {
    HBITMAP mainWindowBitMap = NULL;
    HBITMAP evaluateBitMap = NULL;
    HBITMAP evaluateGrayBitMap = NULL;
    HBITMAP evaluateReverseBitMap = NULL;

    BOOL showHomeBitmap = false;
    BOOL showEvaluateImage = false;
    BOOL showEvaluateGrayImage = false;
    BOOL showEvaluateReverseImage = false;

    void InvalidateImages()
    {
        showHomeBitmap = false;
        showEvaluateImage = false;
        showEvaluateGrayImage = false;
        showEvaluateReverseImage = false;
    }

    void InvalidateEvaluateImages()
    {
        showEvaluateGrayImage = false;
        showEvaluateImage = false;
        showEvaluateReverseImage = false;
    }


    void ValidateEvaluateImages()
    {
        showEvaluateGrayImage = true;
        showEvaluateImage = true;
        showEvaluateReverseImage = true;
    }

    void loadImage(WCHAR* path, HWND hWnd, HINSTANCE hInst, DWORD typeImage)
    {
        switch (typeImage)
        {
        case MAIN_IMAGE:
            mainWindowBitMap = (HBITMAP)LoadImageW(hInst, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            showHomeBitmap = true;
            break;
        case EVALUATE_IMAGE:
            evaluateBitMap = (HBITMAP)LoadImageW(hInst, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            showEvaluateImage = true;
            break;
        case GRAY_IMAGE:
            evaluateGrayBitMap = (HBITMAP)LoadImageW(hInst, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            showEvaluateGrayImage = true;
            break;

        case INVERSE_IMAGE:
            evaluateReverseBitMap = (HBITMAP)LoadImageW(hInst, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            showEvaluateReverseImage = true;
            break;
        }
        InvalidateRect(hWnd, NULL, TRUE);
    }

    void loadImage(const WCHAR* path, HWND hWnd, HINSTANCE hInst, DWORD typeImage)
    {
        switch (typeImage)
        {
        case MAIN_IMAGE:
            mainWindowBitMap = (HBITMAP)LoadImageW(hInst, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            showHomeBitmap = true;
            break;
        case EVALUATE_IMAGE:
            evaluateBitMap = (HBITMAP)LoadImageW(hInst, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            showEvaluateImage = true;
            break;
        case GRAY_IMAGE:
            evaluateGrayBitMap = (HBITMAP)LoadImageW(hInst, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            showEvaluateGrayImage = true;
            break;

        case INVERSE_IMAGE:
            evaluateReverseBitMap = (HBITMAP)LoadImageW(hInst, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            showEvaluateReverseImage = true;
            break;
        }
        InvalidateRect(hWnd, NULL, TRUE);
    }

    void loadImage(const char* path, HWND hWnd, HINSTANCE hInst, DWORD typeImage)
    {
        switch (typeImage)
        {
        case MAIN_IMAGE:
            mainWindowBitMap = (HBITMAP)LoadImage(hInst, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            showHomeBitmap = true;
            break;
        case EVALUATE_IMAGE:
            evaluateBitMap = (HBITMAP)LoadImage(hInst, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            showEvaluateImage = true;
            break;
        case GRAY_IMAGE:
            evaluateGrayBitMap = (HBITMAP)LoadImage(hInst, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            showEvaluateGrayImage = true;
            break;

        case INVERSE_IMAGE:
            evaluateReverseBitMap = (HBITMAP)LoadImage(hInst, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            showEvaluateReverseImage = true;
            break;
        }
        InvalidateRect(hWnd, NULL, TRUE);
    }

} ShowImages;
