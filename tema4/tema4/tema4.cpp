// tema4.cpp : Defines the entry point for the application.
//
#include "framework.h"
#include "tema4.h"
#include <ShObjIdl.h>
#include "Manager.h"

HINSTANCE hInst;                             

#define ID_SELECT_DEPOSIT 101
#define ID_SELECT_SOLD 102
#define ID_RUN_MANAGEMENT 103
#define ID_TEXT_AREA 104
#define MAX_LOADSTRING 100
#define MAX_LOADSTRING 100

// Global Variables:
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
WCHAR folderPath[MAX_PATH];

void                CreateInterfaceElements(HWND hWnd);
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
HRESULT             SelectFolder(HWND, WCHAR*, UINT);

DirFileHandler dirFHandler = DirFileHandler();
Manager manager = Manager();


using namespace std;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    CoInitialize(NULL);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TEMA4, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    
    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TEMA4));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    CoUninitialize();

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TEMA4));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TEMA4);
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
    HWND hEdit;
    wstring contentToDisplay;
    switch (message)
    {
    case WM_CREATE:
        CreateInterfaceElements(hWnd);
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case ID_SELECT_DEPOSIT:
                // Set the DEPOSIT dir path accordingly
                hEdit = GetDlgItem(hWnd, ID_TEXT_AREA);
                memset(folderPath, MAX_PATH, 0);
                if (SUCCEEDED(SelectFolder(hWnd, folderPath, MAX_PATH))) {
                    wstring path = L"";
                    path += folderPath;
                    if (dirFHandler.setPathDir(path, DEPOSIT_DIR)) {
                        wstring pathMessage = L"The path ";
                        pathMessage += dirFHandler.getPathDir(DEPOSIT_DIR);
                        pathMessage += L" was successfully set";
                        SetWindowTextW(hEdit, pathMessage.c_str());
                    }
                    else {
                        wstring pathMessage = L"The path to deposit dir couldn't be set";
                        SetWindowTextW(hEdit, pathMessage.c_str());
                    }
                }
                break;
            case ID_SELECT_SOLD:
                // Set the SOLD dir path accordingly
                hEdit = GetDlgItem(hWnd, ID_TEXT_AREA);

                memset(folderPath, MAX_PATH, 0);
                if (SUCCEEDED(SelectFolder(hWnd, folderPath, MAX_PATH))) {
                    if (dirFHandler.setPathDir(folderPath, SOLD_DIR)) {
                        wstring pathMessage = L"The path ";
                        pathMessage += dirFHandler.getPathDir(SOLD_DIR);
                        pathMessage += L" was successfully set";
                        SetWindowTextW(hEdit, pathMessage.c_str());
                    }
                    else {
                        wstring pathMessage = L"The path to sold dir couldn't be set";
                        SetWindowTextW(hEdit, pathMessage.c_str());
                    }
                }
                break;
            case ID_RUN_MANAGEMENT:
                hEdit = GetDlgItem(hWnd, ID_TEXT_AREA);
                SetWindowText(hEdit, "Running Management Process ...\r\t");

                if (!manager.startManagement(dirFHandler))
                {
                    SetWindowTextW(hEdit, manager.getError().c_str());
                    break;
                }

                // when startManagement ends, it means that the child processes have finished;

                contentToDisplay = dirFHandler.getContentForConsole();
                SetWindowTextW(hEdit, contentToDisplay.c_str());

                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
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

/**
* Function responsible for creating the buttons for:
* -> selecting Deposit directory
* -> select sold directory
* -> run Management process
*/
void CreateInterfaceElements(HWND hWnd)
{
    CreateWindow("BUTTON", "Select Deposit Directory", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, 10, 200, 30, hWnd, (HMENU)ID_SELECT_DEPOSIT, hInst, NULL);

    CreateWindow("BUTTON", "Select Sold Directory", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, 50, 200, 30, hWnd, (HMENU)ID_SELECT_SOLD, hInst, NULL);

    CreateWindow("BUTTON", "Run Management Process", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, 90, 200, 30, hWnd, (HMENU)ID_RUN_MANAGEMENT, hInst, NULL);

    CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        220, 10, 500, 300, hWnd, (HMENU)ID_TEXT_AREA, hInst, NULL);
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
