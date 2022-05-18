#include <sstream>
#include "D2DFramework.h"

#pragma comment (lib, "d2d1.lib")

HRESULT D2DFramework::Init(HWND hwnd)
{
	// 1. D2D Factory 만들기

	HRESULT hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED, mspD2DFactory.GetAddressOf()
		// 잔상. 나혼자 순보. 하나를 공유하는 자원을 나누어 쓰는 것. 싱글 쓰레드를 쓸 것임, 포인터의 포인터
		// multi thread의 경우 deadlack를 조심해야 한다.
	);

	// SUCCEEDED(hr) 많은 성공들을 확인할 수있는 것.

	if (FAILED(hr)) // FAIED는 < 0 라는 뜻이다. 많은 error들을 확인할 수 있는 것.
	{
		ShowErrorMsg(L"Failed To Creat D2D Factory!");
		return hr;
	}

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
	if (FAILED(hr))
	{
		ShowErrorMsg(L"Failed To Create D2D Render Target!");
		return hr;
	}

    return S_OK;
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

void D2DFramework::ShowErrorMsg(LPCWSTR msg, HRESULT error, LPCWSTR title)
{
	std::wostringstream oss;

	oss << L"ERROR : " << error << std::endl;
	oss << msg;
	
	OutputDebugString(oss.str().c_str());
}
