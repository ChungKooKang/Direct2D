#include <windows.h>
#include <d2d1.h> //direct 2D 1버젼 Window7 버젼


#pragma comment (lib, "d2d1.lib")

/* 
						Direct2D 사용법

	1. Direct2D Factory		: Factory를 만듦.
	2. Render Target 생성	: 대상을 정하는 것. 어디에다가 그리는지를 정하는 것.
	3. Render 그리기		: 그리기.
	4. 해제					: 없애기. release ~

*/
const wchar_t gClassName[]{ L"MyWindowClass" };

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


// 0. D2D 전역 변수들

ID2D1Factory* gpD2DFactory{};
ID2D1HwndRenderTarget* gpRenderTargetP{};


int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	// 1. D2D Factory 만들기

	HRESULT hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED, &gpD2DFactory	
		// 잔상. 나혼자 순보. 하나를 공유하는 자원을 나누어 쓰는 것. 싱글 쓰레드를 쓸 것임, 포인터의 포인터
	);

	// SUCCEEDED(hr) 많은 성공들을 확인할 수있는 것.

	if (FAILED(hr)) // FAIED는 < 0 라는 뜻이다. 많은 error들을 확인할 수 있는 것.
	{
		MessageBox(NULL, L"Failed To Create D2D Factory!", L"Error", MB_OK);
		return 0;
	}

	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpszClassName = gClassName;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpfnWndProc = WindowProc;
	wc.cbSize = sizeof(WNDCLASSEX);
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Failed To Register!", L"Error", MB_OK);
		return 0;
	}

	RECT rc{ 0, 0, 1024, 768 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND hwnd = CreateWindowEx(
		NULL,
		gClassName,
		L"Direct2D",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	if (hwnd == NULL)
	{
		MessageBox(NULL, L"Failed To Create", L"Error", MB_OK);
		return 0;
	}


	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return static_cast<int>(msg.wParam);
}

void OnPaint(HWND hwnd)
{
	HDC hdc;
	PAINTSTRUCT ps;

	hdc = BeginPaint(hwnd, &ps);

	EndPaint(hwnd, &ps);
}



LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT :
		OnPaint(hwnd);
		break;

	case WM_CLOSE :
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY :
		PostQuitMessage(0);
		break;

	default :
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}