
#include <windows.h>

#define RGBA(r,g,b,a)          (COLORREF)(((BYTE)(r) |((WORD)((BYTE)(g)) << 8)) |(((DWORD)((BYTE)(b)) << 16)) |(((DWORD)((BYTE)(a)) << 24)))

//���ڹ��̺���
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
//ע�ᴰ�ں���
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
//�����ֲ㴰��
HWND CreateLayeredWindow(HINSTANCE hInstance, HWND hWnd, int iWidth, int iHeight, int iPosX, int iPosY, COLORREF* colRGBA)
{

    //ע��ֲ㴰��
    RegWindow(hInstance, L"LayeredWindow", WindowProc, BLACK_BRUSH);
    //�����ֲ㴰��
    HWND hLayeredWindow = CreateWindowEx(WS_EX_LAYERED, L"LayeredWindow", 0,
        WS_POPUP | WS_BORDER, 0, 0, iWidth, iHeight, NULL, NULL, hInstance, NULL);

    //���ֲ㴰����Ϊ�����ڵ��Ӵ���
    SetParent(hLayeredWindow, hWnd);

    //���·ֲ㴰��//////////////////////////////////////////////

    //�����ֲ㴰�ڵ�DC
    HDC hLayeredWindowDC = GetDC(hLayeredWindow);
    HDC hCompatibleDC = CreateCompatibleDC(hLayeredWindowDC);
    //���BLENDFUNCTION�ṹ
    BLENDFUNCTION blend = { 0 };
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;
    //������ʾλ��
    POINT ptDst = { iPosX, iPosY };
    //���ƴ��ڴ�С
    SIZE sizeWnd = { iWidth, iHeight };
    //Ϊ0����
    POINT pSrc = { 0, 0 };

    //����һ���뵱ǰDC���ݵ�λͼ
    HBITMAP hCustomBmp = NULL;
    hCustomBmp = CreateCompatibleBitmap(hLayeredWindowDC, iWidth, iHeight);
    //��hCustomBmpָ����hCompatibleDC��
    SelectObject(hCompatibleDC, hCustomBmp);
    //���bmpInfo
    BITMAPINFO bmpInfo = { 0 };
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = iWidth;
    bmpInfo.bmiHeader.biHeight = -iHeight;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biCompression = BI_RGB;
    bmpInfo.bmiHeader.biBitCount = 32;
    //ʹ��ָ����DIB��ɫ����������λͼ�е�����
    SetDIBits(NULL, hCustomBmp, 0, iHeight, colRGBA, &bmpInfo, DIB_RGB_COLORS);
    //���·ֲ㴰��
    UpdateLayeredWindow(hLayeredWindow, hLayeredWindowDC, &ptDst, &sizeWnd, hCompatibleDC, &pSrc, NULL, &blend, ULW_ALPHA);
    //�ͷ�DC
    DeleteDC(hLayeredWindowDC);

    return hLayeredWindow;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE PrevhInstance, LPSTR lpCmdLine, int nCmdShow)
{
    //ע�ᴰ��
    RegWindow(hInstance, L"WINDOW", WindowProc, WHITE_BRUSH);
    //��������
    HWND hWnd = CreateWindow(L"WINDOW", 0,
        WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, NULL, NULL, hInstance, NULL);
    //��ʾ����
    ShowWindow(hWnd, SW_SHOW);
    //���´���
    UpdateWindow(hWnd);

    //����BGRA����
    COLORREF* colBGRA = new COLORREF[100 * 100];
    //��ʼ��
    ZeroMemory(colBGRA, 100 * 100 * sizeof(COLORREF));
    //����͸����ɫ
    for (int i = 0; i < 100 * 100; i++)
    {
        //��Ȼ�õ���RGBA����ʵ������BGRA
        colBGRA[i] = RGBA(233, 0, 0, 128);
        //                 B    G    R   A
    }
    //�����ֲ㴰��
    HWND hLW = CreateLayeredWindow(hInstance, hWnd, 100, 100, 0, 0, colBGRA);

    //��Ϣѭ��
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

