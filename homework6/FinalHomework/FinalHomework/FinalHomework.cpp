// FinalHomework.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "FinalHomework.h"
#include "ComputePerformancesManager.h"
#include "EvaluatePerformancesManager.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HMENU hMenu;
HMENU hSubMenu;
WCHAR imgPath[MAX_PATH], greyPath[MAX_PATH], inversePath[MAX_PATH];

evInput evaluateInput = evInput();
ShowImages showImages = ShowImages();

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                CreateCustomMenu(HWND);
void                CreateMyPerformances(HWND);
void                CreateEvaluatePerformances(HWND);
void                ClearMainWindow(HWND);
void                CreateMainWindowText(HWND);
void                ClearPerformancesWindows(HWND);
void                CreateEvaluatePerfomances(HWND);
void                ClearEvaluatePerformancesWindows(HWND);
void                DrawBitMapTo(HBITMAP, HDC, HWND, DWORD, DWORD, DWORD, DWORD);
HRESULT             SelectFolder(HWND, WCHAR*, UINT);
HRESULT             SelectFile(HWND, WCHAR*, UINT);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FINALHOMEWORK, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FINALHOMEWORK));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FINALHOMEWORK));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_FINALHOMEWORK);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Manager* manager;
    HWND hLogWindow, window;
    
    switch (message)
    {
    case WM_CREATE:
        CreateCustomMenu(hWnd);
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            // events related to switch between windows
            case ID_COMPUTE_MY_PERFORMANCES:
                ClearMainWindow(hWnd);
                CreateMyPerformances(hWnd);
                break;

            case ID_EVALUATE_MY_PERFORMANCES:
                ClearMainWindow(hWnd);
                CreateEvaluatePerformances(hWnd);
                showImages.ValidateEvaluateImages();
                break;
            case ID_HOME:
                ClearMainWindow(hWnd);
                CreateMainWindowText(hWnd);
                break;

             // events related to Evaluate my PC
            case ID_CLEAR_LOGS_E:
                ClearEvaluatePerformancesWindows(hWnd);
                break;

            case ID_SELECT_INPUT_IMAGE:
                window = GetDlgItem(hWnd, ID_INPUT_PATH_DISPLAY);
                memset(imgPath, MAX_PATH, 0);
                if (SUCCEEDED(SelectFile(hWnd, imgPath, MAX_PATH))) {
                    evaluateInput.imagePath = wstring(imgPath);
                    SetWindowTextW(window, imgPath);
                    showImages.loadImage(imgPath, hWnd, hInst, EVALUATE_IMAGE);
                }
                else {
                    hLogWindow = GetDlgItem(hWnd, ID_LOG_WINDOW_E);
                    EvaluatePerformancesManager::LOG(hLogWindow, "Couldn't get the path for the test image", TRUE);
                }
                break;

            case ID_SELECT_GRAY_OUTPUT:
                window = GetDlgItem(hWnd, ID_GRAY_PATH_DISPLAY);
                memset(greyPath, MAX_PATH, 0);
                if (SUCCEEDED(SelectFolder(hWnd, greyPath, MAX_PATH))) {
                    evaluateInput.greyPath = wstring(greyPath);
                    SetWindowTextW(window, greyPath);
                }
                else {
                    hLogWindow = GetDlgItem(hWnd, ID_LOG_WINDOW_E);
                    EvaluatePerformancesManager::LOG(hLogWindow, "Couldn't get the dir path for the gray output", TRUE);
                }
                break;
                
            case ID_SELECT_INVERSE_OUTPUT:
                window = GetDlgItem(hWnd, ID_INVERSE_PATH_DISPLAY);
                memset(inversePath, MAX_PATH, 0);
                if (SUCCEEDED(SelectFolder(hWnd, inversePath, MAX_PATH))) {
                    evaluateInput.inversePath = wstring(inversePath);
                    SetWindowTextW(window, inversePath);
                }
                else {
                    hLogWindow = GetDlgItem(hWnd, ID_LOG_WINDOW_E);
                    EvaluatePerformancesManager::LOG(hLogWindow, "Couldn't get the path for the test image", TRUE);
                }
                break;

            case ID_RUN_TESTS_E:
                evaluateInput.getTypeOfTest(GetDlgItem(hWnd, ID_CHOICE_BOX));

                if (!EvaluatePerformancesManager::isInputValid(evaluateInput))
                {
                    hLogWindow = GetDlgItem(hWnd, ID_LOG_WINDOW_E);
                    EvaluatePerformancesManager::LOG(hLogWindow, "Input invalid. Check that all paths have been supplied and test type selected.", TRUE);
                    break;
                }
                manager = new EvaluatePerformancesManager(hWnd, hInst, evaluateInput, showImages);
                manager->execute();
                delete manager;
                break;

            // events related to MyPerformances
            case ID_COLLECT_BUTTON:
                manager = new ComputePerformancesManager(hWnd);
                manager->execute();
                delete manager;
                break;
            case ID_CLEAR_LOGS_P:
                ClearPerformancesWindows(hWnd);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        if (showImages.showHomeBitmap && showImages.mainWindowBitMap != NULL) {
            DrawBitMapTo(showImages.mainWindowBitMap, hdc, hWnd, xHomeImage, yHomeImage, widthHomeImage, heightHomeImage);
        }

        if (showImages.showEvaluateImage && showImages.evaluateBitMap != NULL) {
            DrawBitMapTo(showImages.evaluateBitMap, hdc, hWnd, xImagePos1, yimagePos, widthImage, heightImage);
        }

        if (showImages.showEvaluateGrayImage && showImages.evaluateGrayBitMap != NULL) {
            DrawBitMapTo(showImages.evaluateGrayBitMap, hdc, hWnd, xImagePos2, yimagePos, widthImage, heightImage);
        }

        if (showImages.showEvaluateReverseImage && showImages.evaluateReverseBitMap != NULL) {
            DrawBitMapTo(showImages.evaluateReverseBitMap, hdc, hWnd, xImagePos3, yimagePos, widthImage3, heightImage);
        }

        EndPaint(hWnd, &ps);
    }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void CreateCustomMenu(HWND hWnd)
{

    hMenu = CreateMenu();
    hSubMenu = CreatePopupMenu();
    AppendMenu(hSubMenu, MF_STRING, ID_HOME, "Home");
    AppendMenu(hSubMenu, MF_STRING, ID_COMPUTE_MY_PERFORMANCES, "My performances");
    AppendMenu(hSubMenu, MF_STRING, ID_EVALUATE_MY_PERFORMANCES, "Evaluate my PC");
    AppendMenu(hSubMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hSubMenu, MF_STRING, IDM_EXIT, "Exit");
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&Menu");
    SetMenu(hWnd, hMenu);
    CreateMainWindowText(hWnd);
}

void CreateMyPerformances(HWND hWnd) 
{
    CreateWindow("STATIC", "  Informations about my PC ", WS_VISIBLE | WS_CHILD,
        STARTING_POSITION, 20, 180, HEIGHT_LABEL, hWnd, (HMENU)ID_MY_P_TITLE, hInst, NULL);

    CreateWindow("STATIC", "  SID's", WS_VISIBLE | WS_CHILD,
        STARTING_POSITION, 50, WIDTH_LABEL, HEIGHT_LABEL,  hWnd, (HMENU)ID_SID_LABEL, hInst, NULL);

    CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
        STARTING_POSITION, 70, WIDTH_DISPLAY, HEIGHT_DISPLAY, hWnd, (HMENU)ID_SID_DISPLAY, hInst, NULL);

    CreateWindow("STATIC", "  HT API", WS_VISIBLE | WS_CHILD,
        STARTING_POSITION, 180, WIDTH_LABEL, HEIGHT_LABEL, hWnd, (HMENU)ID_HT_LABEL, hInst, NULL);

    CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
        STARTING_POSITION, 200, WIDTH_DISPLAY, HEIGHT_DISPLAY, hWnd, (HMENU)ID_HT_DISPLAY, hInst, NULL);

    CreateWindow("STATIC", "  NUMA API", WS_VISIBLE | WS_CHILD,
        STARTING_POSITION, 310, WIDTH_LABEL, HEIGHT_LABEL, hWnd, (HMENU)ID_NUMA_LABEL, hInst, NULL);

    CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
        STARTING_POSITION, 330, WIDTH_DISPLAY, HEIGHT_DISPLAY, hWnd, (HMENU)ID_NUMA_DISPLAY, hInst, NULL);

    CreateWindow("STATIC", "  CPU Sets", WS_VISIBLE | WS_CHILD,
        STARTING_POSITION, 440, WIDTH_LABEL, HEIGHT_LABEL, hWnd, (HMENU)ID_CPU_LABEL, hInst, NULL);

    CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
        STARTING_POSITION, 460, WIDTH_DISPLAY, HEIGHT_DISPLAY, hWnd, (HMENU)ID_CPU_DISPLAY, hInst, NULL);
    
    CreateWindow("BUTTON", "Collect info", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        STARTING_POSITION, 580, 200, 30, hWnd, (HMENU)ID_COLLECT_BUTTON, hInst, NULL);

    CreateWindow("STATIC", " Logger", WS_VISIBLE | WS_CHILD,
        450, 20, 100, 20, hWnd, (HMENU)ID_LOG_WINDOW_P_LABEL, hInst, NULL);

    CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
        450, 40, 700, 520, hWnd, (HMENU)ID_LOG_WINDOW_P, hInst, NULL);

    CreateWindow("BUTTON", "Clear Logs", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        950, 580, 200, 30, hWnd, (HMENU)ID_CLEAR_LOGS_P, hInst, NULL);
}


void CreateMainWindowText(HWND hWnd) {
    HWND hTitle = CreateWindow("STATIC", "  Final project for CSSO  ",
        WS_VISIBLE | WS_CHILD,
        HOME_STARTING_POSITION, 20, 1000, 120,
        hWnd, (HMENU)ID_MY_P_TITLE, hInst, NULL);

    HFONT titleFont = CreateFont(100, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");

    SendMessage(hTitle, WM_SETFONT, (WPARAM)titleFont, TRUE);

    RECT rect;
    GetClientRect(hWnd, &rect);

    HWND footer = CreateWindow("STATIC", "  Proiect realizat de:", WS_VISIBLE | WS_CHILD,
        1200, rect.bottom - 200, 
        200, 40,
        hWnd, NULL, hInst, NULL);
    
    HWND footerText = CreateWindow("STATIC", "\n Tudor Ilade\n Teodor Sarghie", WS_VISIBLE | WS_CHILD,
        1200, rect.bottom - 180,
        220, 100,
        hWnd, NULL, hInst, NULL);

    HFONT footerFont = CreateFont(25, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");

    SendMessage(footer, WM_SETFONT, (WPARAM)footerFont, TRUE);
    SendMessage(footerText, WM_SETFONT, (WPARAM)footerFont, TRUE);
    showImages.loadImage("C:\\Users\\tudor\\CSSO-Homeworks\\city.bmp", hWnd, hInst, MAIN_IMAGE);
}


void DrawBitMapTo(HBITMAP hBitmap, HDC hdc, HWND hWnd, DWORD xPos, DWORD yPos, DWORD width, DWORD height)
{
    if (hBitmap != NULL) {
        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
        BITMAP bitmap;
        GetObject(hBitmap, sizeof(bitmap), &bitmap);
        StretchBlt(hdc, xPos, yPos, width, height, hdcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
        SelectObject(hdcMem, oldBitmap);
        DeleteDC(hdcMem);
    }
}


void CreateEvaluatePerformances(HWND hWnd) {

    RECT rect;
    GetClientRect(hWnd, &rect);

    // 474 width for a screen

    int widthScreens = 474;

    int endOffirstScreen = widthScreens * 1;
    int endOfSecondScreen = widthScreens * 2;
    int endOfThirdScreen = widthScreens * 3;

    // path of input image
    CreateWindow("BUTTON", "Select input image", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        5, yImagesPos, (int)(endOffirstScreen / 3), 30, hWnd, (HMENU)ID_SELECT_INPUT_IMAGE, hInst, NULL);

    CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_READONLY,
        (int)(endOffirstScreen / 3) + 5, yImagesPos, (int)(2 * endOffirstScreen / 3) - 10, 30, hWnd, (HMENU)ID_INPUT_PATH_DISPLAY, hInst, NULL);

    // path of Gray image
    int widthGray = (int)(endOffirstScreen / 3);
    int startGray = endOffirstScreen + 10;
    CreateWindow("BUTTON", "Select GRAY path", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        startGray, yImagesPos, widthGray, 30, hWnd, (HMENU)ID_SELECT_GRAY_OUTPUT, hInst, NULL);

    CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_READONLY,
        startGray + widthGray, yImagesPos, (int)(2 * widthScreens / 3) - 10, 30, hWnd, (HMENU)ID_GRAY_PATH_DISPLAY, hInst, NULL);

    CreateWindow("STATIC", "Execution time:",
        WS_CHILD | WS_VISIBLE | ES_LEFT,
        startGray, yExecution, 100, 20,
        hWnd, (HMENU)ID_LABEL_EXEC_TIME_GREY, hInst, NULL);


    CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_READONLY,
        startGray + 100, yExecution, 100, 20, hWnd, (HMENU)ID_EXEC_TIME_GREY, hInst, NULL);


    // path of Inverse image
    
    int widthInverse= (int)(widthScreens / 3);
    int startInverse  = endOfSecondScreen + 10;

    CreateWindow("BUTTON", "Select INVERSE path", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        startInverse, yImagesPos, widthInverse, 30, hWnd, (HMENU)ID_SELECT_INVERSE_OUTPUT, hInst, NULL);

    CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_READONLY,
        startInverse + widthInverse, yImagesPos, (int)(2 * widthScreens / 3) - 15, 30, hWnd, (HMENU)ID_INVERSE_PATH_DISPLAY, hInst, NULL);

    CreateWindow("STATIC", "Execution time:",
        WS_CHILD | WS_VISIBLE | ES_LEFT,
        startInverse, yExecution, 100, 20,
        hWnd, (HMENU)ID_LABEL_EXEC_TIME_INVERSE, hInst, NULL);

    CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_READONLY,
        startInverse + 100, yExecution, 100, 20, hWnd, (HMENU)ID_EXEC_TIME_INVERSE, hInst, NULL);


    // creation of selection tests

    CreateWindow("STATIC", "Select type of test",
        WS_CHILD | WS_VISIBLE | ES_LEFT,
        10, yComboBox, 130, 20,
        hWnd, (HMENU)ID_CHOICE_LABEL, hInst, NULL);

    HWND hComboBox = CreateWindow("COMBOBOX", "",
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_SORT,
        130, yComboBox, 180, 150,
        hWnd, (HMENU)ID_CHOICE_BOX, hInst, NULL);

    ComboBox_AddString(hComboBox, COMBO_CHOICE_1);
    ComboBox_AddString(hComboBox, COMBO_CHOICE_2);
    ComboBox_AddString(hComboBox, COMBO_CHOICE_3);

    // creation of logs

    CreateWindow("STATIC", "Logger", WS_VISIBLE | WS_CHILD,
        5, yExecution, 100, 20, hWnd, (HMENU)ID_LOG_WINDOW_E_LABEL, hInst, NULL);

    CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
        5, yExecution + 20, widthScreens, 110, hWnd, (HMENU)ID_LOG_WINDOW_E, hInst, NULL);

    CreateWindow("BUTTON", "Clear Logs", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        1200, 650, 200, 30, hWnd, (HMENU)ID_CLEAR_LOGS_E, hInst, NULL);

    // run tests

    CreateWindow("BUTTON", "Run tests", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        1200, yComboBox, 200, 30, hWnd, (HMENU)ID_RUN_TESTS_E, hInst, NULL);


    CreateWindow("STATIC", "BMP Header Info",
        WS_CHILD | WS_VISIBLE | ES_LEFT,
        350 + 120, yComboBox, 130, 20,
        hWnd, (HMENU)ID_BMP_HEADER_LABEL, hInst, NULL);

    CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
        480 + 120, yComboBox, 400, 60, hWnd, (HMENU)ID_BMP_HEADER, hInst, NULL);
}

BOOL CALLBACK DestoryChildCallback(
    HWND   hwnd,
    LPARAM lParam
)
{
    if (hwnd != NULL) {
        DestroyWindow(hwnd);
    }

    return TRUE;
}

/*
    function responsible for clearing up the main window
*/
void ClearMainWindow(HWND hWnd) {
    EnumChildWindows(hWnd, DestoryChildCallback, NULL);
    showImages.InvalidateImages();
    evaluateInput.clear();
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
}


void ClearPerformancesWindows(HWND mainWindow)
{
    // clear LOGs window
    HWND hLogWindow = GetDlgItem(mainWindow, ID_LOG_WINDOW_P);
    SetWindowText(hLogWindow, "");

    // clear SID window
    HWND sidWindow = GetDlgItem(mainWindow, ID_SID_DISPLAY);
    SetWindowText(sidWindow, "");

    // clear HT window
    HWND HTWindow = GetDlgItem(mainWindow, ID_HT_DISPLAY);
    SetWindowText(HTWindow, "");

    // clear NUMA window
    HWND numaWindow = GetDlgItem(mainWindow, ID_NUMA_DISPLAY);
    SetWindowText(numaWindow, "");

    // clear CPU window
    HWND cpuWindow = GetDlgItem(mainWindow, ID_CPU_DISPLAY);
    SetWindowText(cpuWindow, "");
}

void ClearEvaluatePerformancesWindows(HWND mainWindow)
{
    showImages.InvalidateEvaluateImages();
    evaluateInput.clear();
    InvalidateRect(mainWindow, NULL, TRUE);
    UpdateWindow(mainWindow);

    HWND hImgPath = GetDlgItem(mainWindow, ID_INPUT_PATH_DISPLAY);
    SetWindowText(hImgPath, "");

    HWND hGrayPath = GetDlgItem(mainWindow, ID_GRAY_PATH_DISPLAY);
    SetWindowText(hGrayPath, "");

    HWND hInversePath = GetDlgItem(mainWindow, ID_INVERSE_PATH_DISPLAY);
    SetWindowText(hInversePath, "");

    HWND execGrey = GetDlgItem(mainWindow, ID_EXEC_TIME_GREY);
    SetWindowText(execGrey, "");

    HWND execInverse = GetDlgItem(mainWindow, ID_EXEC_TIME_INVERSE);
    SetWindowText(execInverse, "");

    HWND hLogs = GetDlgItem(mainWindow, ID_LOG_WINDOW_E);
    SetWindowText(hLogs, "");

    HWND hBmpHeader = GetDlgItem(mainWindow, ID_BMP_HEADER);
    SetWindowText(hBmpHeader, "");
}

/**
* Method responsible for selecting a folder and writing the absolute path to it to folderPath variable
*/
HRESULT SelectFolder(HWND hWnd, WCHAR* folderPath, UINT pathSize) {
    IFileOpenDialog* pFileOpenDialog = NULL;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpenDialog));
    if (SUCCEEDED(hr)) {
        DWORD dwOptions;
        hr = pFileOpenDialog->GetOptions(&dwOptions);
        if (SUCCEEDED(hr)) {
            hr = pFileOpenDialog->SetOptions(dwOptions | FOS_PICKFOLDERS);
            if (SUCCEEDED(hr)) {
                hr = pFileOpenDialog->Show(hWnd);
                if (SUCCEEDED(hr)) {
                    IShellItem* pItem = NULL;
                    hr = pFileOpenDialog->GetResult(&pItem);
                    if (SUCCEEDED(hr)) {
                        PWSTR pszFilePath = NULL;
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                        if (SUCCEEDED(hr)) {
                            wcsncpy_s(folderPath, pathSize, pszFilePath, pathSize - 1);
                            CoTaskMemFree(pszFilePath);
                        }
                        pItem->Release();
                    }
                }
            }
        }
        pFileOpenDialog->Release();
    }
    return hr;
}


/**
* Method responsible for selecting a file and writing the absolute path to it to folderPath variable
*/
HRESULT SelectFile(HWND hWnd, WCHAR* filePath, UINT pathSize) {
    IFileOpenDialog* pFileOpenDialog = NULL;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpenDialog));
    if (SUCCEEDED(hr)) {
        hr = pFileOpenDialog->Show(hWnd);
        if (SUCCEEDED(hr)) {
            IShellItem* pItem = NULL;
            hr = pFileOpenDialog->GetResult(&pItem);
            if (SUCCEEDED(hr)) {
                PWSTR pszFilePath = NULL;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                if (SUCCEEDED(hr)) {
                    wcsncpy_s(filePath, pathSize, pszFilePath, pathSize - 1);
                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
        }
        pFileOpenDialog->Release();
    }
    return hr;
}
