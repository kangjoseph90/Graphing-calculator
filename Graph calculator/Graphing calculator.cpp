// Graph calculator.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "Graphing calculator.h"

using namespace std;

#define MAX_LOADSTRING 100

string Eq = "(|x|^(5/2))+(y^3)-(2*(y^2))-(3*|x|*y)=1"; 
//string Eq = "|1-((1-(4*z))^(1/2))|<1";

appState StateInfo;
bool drag = false;
pair<double, double> dragstart,dragend;

const COLORREF rgbRed = 0x000000FF;
const COLORREF rgbGreen = 0x0000FF00;
const COLORREF rgbBlue = 0x00FF0000;
const COLORREF rgbBlack = 0x00000000;
const COLORREF rgbWhite = 0x00FFFFFF;
const COLORREF rgbGray = 0x00A0A0A0;

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    StateInfo.GraphType = 0;
    incodeEq(Eq, StateInfo);


    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GRAPHCALCULATOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GRAPHCALCULATOR));

    MSG msg;

    // 기본 메시지 루프입니다:
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
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GRAPHCALCULATOR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GRAPHCALCULATOR);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 400, 400, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
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
            POINT pt = Pair2PT({ 0,0 }, StateInfo);
            for (int i = 0; i < StateInfo.screenSize.first; i++) 
                SetPixel(hdc, i, pt.y, rgbGray);
            for (int i = 0; i < StateInfo.screenSize.second; i++) 
                SetPixel(hdc, pt.x, i, rgbGray);
            for (int i = 0; i < StateInfo.screenSize.first; i++) {
                for (int j = 0; j < StateInfo.screenSize.second; j++) {
                    if (inGraph(PT2Pair({ i,j }, StateInfo), StateInfo)) {
                        SetPixel(hdc, i, j, rgbRed);
                    }
                }
            }
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_MOUSEWHEEL:
        {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hWnd, &pt);
            pair<double, double> now = PT2Pair(pt, StateInfo);
            if ((short)HIWORD(wParam) > 0) StateInfo.perPixel /= 2;
            else StateInfo.perPixel *= 2;
            StateInfo.topleft = { now.first - StateInfo.perPixel * pt.x,now.second + StateInfo.perPixel * pt.y };
            InvalidateRect(hWnd, NULL, true);
        }
        break;
    case WM_SIZE:
        {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);
            StateInfo.screenSize = { width,height };
            InvalidateRect(hWnd, NULL, true);
        }
        break;
    case WM_LBUTTONDOWN:
        {
            POINT pt = { LOWORD(lParam),HIWORD(lParam) };
            if (DragDetect(hWnd, pt)) {
                drag = true;
                dragstart = PT2Pair(pt, StateInfo);
            }
        }
        break;
    case WM_LBUTTONUP:
        {
            POINT pt = { LOWORD(lParam),HIWORD(lParam) };
            if (drag) {
                drag = false;
                dragend = PT2Pair(pt, StateInfo);
                StateInfo.topleft.first -= dragend.first - dragstart.first;
                StateInfo.topleft.second -= dragend.second - dragstart.second;
                InvalidateRect(hWnd, NULL, true);
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

// 정보 대화 상자의 메시지 처리기입니다.
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
