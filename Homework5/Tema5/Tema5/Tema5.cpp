// Tema5.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Tema5.h"

using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
LPSTR linkBuffer;             // link buffer
LPSTR matricolBuffer;     // matricol number buffer
LPSTR requestLink;        

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                CreateInterfaceElements(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TEMA5, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TEMA5));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TEMA5));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TEMA5);
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
    HWND hLogWindow;
    size_t sizeLink;
    Manager manager;
    size_t sizeMatricol;

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
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case ID_EXEC_BUTTON:
                linkBuffer = (LPSTR)calloc(LINK_BUFFER_SIZE, sizeof(LPSTR));
                matricolBuffer= (LPSTR)calloc(M_NUMBER_BUFFER_SIZE, sizeof(LPSTR));

                GetDlgItemText(hWnd, ID_LINK, linkBuffer, LINK_BUFFER_SIZE);
                GetDlgItemText(hWnd, ID_MATRICOL, matricolBuffer, M_NUMBER_BUFFER_SIZE);

                sizeMatricol = strlen(matricolBuffer) + 1;
                sizeLink = strlen(linkBuffer) + 1;


                hLogWindow = GetDlgItem(hWnd, ID_LOG_WINDOW);

                manager = Manager(
                    linkBuffer, sizeLink,
                    matricolBuffer, sizeMatricol,
                    hLogWindow
                );

                if (!manager.execute())
                {
                    manager.LOG(manager.getLastError().c_str());
                    break;
                }

                free(linkBuffer);
                free(matricolBuffer);
                free(requestLink);
                sizeLink = 0;

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
            // TODO: Add any drawing code that uses hdc here...
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


/**
* Function responsible for creating the buttons for:
* -> selecting Deposit directory
* -> select sold directory
* -> run Management process
*/
void CreateInterfaceElements(HWND hWnd)
{
    
    // edit for matricol number and link

    CreateWindow("STATIC", "Server address", WS_VISIBLE | WS_CHILD,
        10, 10, 200, 20, hWnd, (HMENU)ID_LINK_SPAN, hInst, NULL);

    CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
        10, 30, 400, 20, hWnd, (HMENU)ID_LINK, hInst, NULL);

    CreateWindow("STATIC", "Matricol Number", WS_VISIBLE | WS_CHILD,
        10, 60, 200, 20, hWnd, (HMENU)ID_MATRICOL_SPAN, hInst, NULL);

    CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
        10, 80, 400, 20, hWnd, (HMENU)ID_MATRICOL, hInst, NULL);

    CreateWindow("BUTTON", "Start requests", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, 120, 200, 30, hWnd, (HMENU)ID_EXEC_BUTTON, hInst, NULL);

    CreateWindow("STATIC", "Requests Logs", WS_VISIBLE | WS_CHILD,
        420, 10, 700, 20, hWnd, (HMENU)ID_MATRICOL_SPAN, hInst, NULL);

    // Button to run the program with the entered data
    CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        420, 30, 700, 400, hWnd, (HMENU)ID_LOG_WINDOW, hInst, NULL);

}