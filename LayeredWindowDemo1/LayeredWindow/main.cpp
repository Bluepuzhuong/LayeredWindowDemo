
#include <windows.h>

#define RGBA(r,g,b,a)          (COLORREF)(((BYTE)(r) |((WORD)((BYTE)(g)) << 8)) |(((DWORD)((BYTE)(b)) << 16)) |(((DWORD)((BYTE)(a)) << 24)))

//窗口过程函数
LRESULT CALLBACK WindowProc(HWND hWnd, UINT Msg,
    WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, Msg, wParam, lParam);
    }
    return 0;
}
//注册窗口函数
void RegWindow(HINSTANCE hInstance, LPCWSTR lpClassName, WNDPROC wndProc, DWORD dwColor)
{
    WNDCLASS wnd;
    wnd.cbClsExtra = 0;
    wnd.cbWndExtra = 0;
    wnd.hbrBackground = (HBRUSH)(GetStockObject(dwColor));
    wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
    wnd.hIcon = LoadCursor(NULL, IDI_APPLICATION);
    wnd.lpfnWndProc = wndProc;
    wnd.lpszClassName = lpClassName;
    wnd.lpszMenuName = NULL;
    wnd.style = CS_HREDRAW;
    wnd.hInstance = hInstance;
    RegisterClass(&wnd);
}
//创建分层窗口
HWND CreateLayeredWindow(HINSTANCE hInstance, HWND hWnd, int iWidth, int iHeight, int iPosX, int iPosY, COLORREF* colRGBA)
{

    //注册分层窗口
    RegWindow(hInstance, L"LayeredWindow", WindowProc, BLACK_BRUSH);
    //创建分层窗口
    HWND hLayeredWindow = CreateWindowEx(WS_EX_LAYERED, L"LayeredWindow", 0,
        WS_POPUP | WS_BORDER, 0, 0, iWidth, iHeight, NULL, NULL, hInstance, NULL);

    //将分层窗口设为本窗口的子窗口
    SetParent(hLayeredWindow, hWnd);

    //更新分层窗口//////////////////////////////////////////////

    //创建分层窗口的DC
    HDC hLayeredWindowDC = GetDC(hLayeredWindow);
    HDC hCompatibleDC = CreateCompatibleDC(hLayeredWindowDC);
    //填充BLENDFUNCTION结构
    BLENDFUNCTION blend = { 0 };
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;
    //控制显示位置
    POINT ptDst = { iPosX, iPosY };
    //控制窗口大小
    SIZE sizeWnd = { iWidth, iHeight };
    //为0就行
    POINT pSrc = { 0, 0 };

    //创建一副与当前DC兼容的位图
    HBITMAP hCustomBmp = NULL;
    hCustomBmp = CreateCompatibleBitmap(hLayeredWindowDC, iWidth, iHeight);
    //将hCustomBmp指定到hCompatibleDC中
    SelectObject(hCompatibleDC, hCustomBmp);
    //填充bmpInfo
    BITMAPINFO bmpInfo = { 0 };
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = iWidth;
    bmpInfo.bmiHeader.biHeight = -iHeight;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biCompression = BI_RGB;
    bmpInfo.bmiHeader.biBitCount = 32;
    //使用指定的DIB颜色数据来设置位图中的像素
    SetDIBits(NULL, hCustomBmp, 0, iHeight, colRGBA, &bmpInfo, DIB_RGB_COLORS);
    //更新分层窗口
    UpdateLayeredWindow(hLayeredWindow, hLayeredWindowDC, &ptDst, &sizeWnd, hCompatibleDC, &pSrc, NULL, &blend, ULW_ALPHA);
    //释放DC
    DeleteDC(hLayeredWindowDC);

    return hLayeredWindow;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE PrevhInstance, LPSTR lpCmdLine, int nCmdShow)
{
    //注册窗口
    RegWindow(hInstance, L"WINDOW", WindowProc, WHITE_BRUSH);
    //创建窗口
    HWND hWnd = CreateWindow(L"WINDOW", 0,
        WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, NULL, NULL, hInstance, NULL);
    //显示窗口
    ShowWindow(hWnd, SW_SHOW);
    //更新窗口
    UpdateWindow(hWnd);

    //创建BGRA数据
    COLORREF* colBGRA = new COLORREF[100 * 100];
    //初始化
    ZeroMemory(colBGRA, 100 * 100 * sizeof(COLORREF));
    //填充半透明蓝色
    for (int i = 0; i < 100 * 100; i++)
    {
        //虽然用的是RGBA，但实际上是BGRA
        colBGRA[i] = RGBA(233, 0, 0, 128);
        //                 B    G    R   A
    }
    //创建分层窗口
    HWND hLW = CreateLayeredWindow(hInstance, hWnd, 100, 100, 0, 0, colBGRA);

    //消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

