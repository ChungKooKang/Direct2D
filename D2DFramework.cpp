#include <sstream>
#include "D2DFramework.h"

#pragma comment (lib, "d2d1.lib")

HRESULT D2DFramework::Init(HWND hwnd)
{
	// 1. D2D Factory �����

	HRESULT hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED, mspD2DFactory.GetAddressOf()
		// �ܻ�. ��ȥ�� ����. �ϳ��� �����ϴ� �ڿ��� ������ ���� ��. �̱� �����带 �� ����, �������� ������
		// multi thread�� ��� deadlack�� �����ؾ� �Ѵ�.
	);

	// SUCCEEDED(hr) ���� �������� Ȯ���� ���ִ� ��.

	if (FAILED(hr)) // FAIED�� < 0 ��� ���̴�. ���� error���� Ȯ���� �� �ִ� ��.
	{
		ShowErrorMsg(L"Failed To Creat D2D Factory!");
		return hr;
	}

	// 2. RenderTarget ����
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