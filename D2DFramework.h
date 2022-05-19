#pragma once

#include <d2d1.h>
#include <wrl/client.h>
#include <exception>
#include <sstream>
class com_exception : public std::exception
{
private :
	HRESULT mResult;

public :
	com_exception(HRESULT hr) : mResult {hr}
	{
	}

	virtual const char* what() const override
	{
		std::stringstream ss;
		ss << "Failure with Code : " << mResult << std::endl;

		return ss.str().c_str();
	}
};

inline void ThrowIfFailed(HRESULT hr) // inline은 함수호출스텍을 줄여서 성능을 증가시키는 것이다. 
									  // 함수의 코드를 해당줄에 붙여서 호출스텍을 사용하지 않게 하는 것이다.
									  // 이것은 compiler가 판단해서 하는데 이것을 우리가 원해도 거부할 수도 있다.
{
	if (FAILED(hr))
	{
		throw com_exception(hr);
	}
}

class D2DFramework
{
private :
	const LPCWSTR WINDOW_CLASSNAME{ L"MyD2DWindowClass" };

protected :
	HWND mHwnd;
	Microsoft::WRL::ComPtr<ID2D1Factory>			mspD2DFactory{};
	Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget>	mspRenderTarget;

protected :
	HRESULT InitWindow(HINSTANCE hInstance, LPCWSTR title, UINT width, UINT height);
	virtual HRESULT InitD2D(HWND hwnd);


public :
	virtual void Initialize(HINSTANCE hInstance, LPCWSTR title = L"D2DFramework", UINT width = 1024, UINT height = 768);
	virtual void Release();
	virtual void Render();
	virtual int GameLoop();

	void ShowErrorMsg(LPCWSTR msg, HRESULT error = 0, LPCWSTR title = L"Error");

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};