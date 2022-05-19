#include <sstream>
#include "D2DFramework.h"

#pragma comment (lib, "d2d1.lib")


HRESULT D2DFramework::InitWindow(HINSTANCE hInstance, LPCWSTR title, UINT width, UINT height)
{
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpszClassName = WINDOW_CLASSNAME;
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


	RECT rc{ 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND hwnd = CreateWindowEx(
		NULL,
		WINDOW_CLASSNAME,
		title,
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
	mHwnd = hwnd;

	SetWindowLongPtr(mHwnd, GWLP_USERDATA, 
		reinterpret_cast<LONG_PTR>(this));

	return S_OK;
}

HRESULT D2DFramework::InitD2D(HWND hwnd)
{
	// 1. D2D Factory 만들기

	HRESULT hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED, mspD2DFactory.GetAddressOf()
		// 잔상. 나혼자 순보. 하나를 공유하는 자원을 나누어 쓰는 것. 싱글 쓰레드를 쓸 것임, 포인터의 포인터
		// multi thread의 경우 deadlack를 조심해야 한다.
	);

	// SUCCEEDED(hr) 많은 성공들을 확인할 수있는 것.



	ThrowIfFailed(hr);

	// 2. RenderTarget 생성
	RECT rc;
	GetClientRect(hwnd, &rc);

	hr = mspD2DFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(
			hwnd,
			D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)
		),
		mspRenderTarget.GetAddressOf()
	);

	ThrowIfFailed(hr);

    return S_OK;		
}


void D2DFramework::Initialize(HINSTANCE hInstance, LPCWSTR title, UINT width, UINT height)
{
	InitWindow(hInstance, title, width, height);
	InitD2D(mHwnd);


	ShowWindow(mHwnd, SW_SHOW);
	UpdateWindow(mHwnd);

}

void D2DFramework::Release()
{
	mspRenderTarget.Reset();
	mspD2DFactory.Reset();
}

void D2DFramework::Render()
{
	mspRenderTarget->BeginDraw();
	mspRenderTarget->Clear(D2D1::ColorF(0.0f, 0.2f, 0.4f, 1.0f));
	mspRenderTarget->EndDraw();
}

int D2DFramework::GameLoop()
{
	MSG msg;
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				break;
			}
		}
		else
		{
			Render();
		}
	}

	Release();

	return static_cast<int>(msg.wParam);
}

void D2DFramework::ShowErrorMsg(LPCWSTR msg, HRESULT error, LPCWSTR title)
{
	std::wostringstream oss;

	oss << L"ERROR : " << error << std::endl;
	oss << msg;
	
	OutputDebugString(oss.str().c_str());
}

LRESULT CALLBACK D2DFramework::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	D2DFramework* pFramework = reinterpret_cast<D2DFramework*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	switch (message)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}
