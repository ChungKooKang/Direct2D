#include <windows.h>
#include <d2d1.h> //direct 2D 1버젼 Window7 버젼
#include <cmath>

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
void OnPaint(HWND hwnd);


// 0. D2D 전역 변수들

ID2D1Factory* gpD2DFactory{};
ID2D1HwndRenderTarget* gpRenderTargetP{};
ID2D1SolidColorBrush* gpBrush{};
ID2D1RadialGradientBrush* gpRadialBrush{};


int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
)
{

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

	// 1. D2D Factory 만들기

	HRESULT hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED, &gpD2DFactory
		// 잔상. 나혼자 순보. 하나를 공유하는 자원을 나누어 쓰는 것. 싱글 쓰레드를 쓸 것임, 포인터의 포인터
		// multi thread의 경우 deadlack를 조심해야 한다.
	);

	// SUCCEEDED(hr) 많은 성공들을 확인할 수있는 것.

	if (FAILED(hr)) // FAIED는 < 0 라는 뜻이다. 많은 error들을 확인할 수 있는 것.
	{
		MessageBox(NULL, L"Failed To Create D2D Factory!", L"Error", MB_OK);
		return 0;
	}

	// 2. RenderTarget 생성

		hr = gpD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(
				hwnd,
				D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)
				),
			&gpRenderTargetP
			);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Failed To Create D2D Render Target!", L"Error", MB_OK);
			return 0;
		}

		hr = gpRenderTargetP->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::OrangeRed),
			&gpBrush
		);

		ID2D1GradientStopCollection* pGradientStops;
		D2D1_GRADIENT_STOP gradientStops[2];
		gradientStops[0].color = D2D1::ColorF(D2D1::ColorF::Yellow);
		gradientStops[0].position = 0.0f;

		gradientStops[1].color = D2D1::ColorF(D2D1::ColorF::Crimson);
		gradientStops[1].position = 1.0f;



		hr = gpRenderTargetP->CreateGradientStopCollection(
			gradientStops,
			2,
			&pGradientStops
		);


		hr = gpRenderTargetP->CreateRadialGradientBrush(
			D2D1::RadialGradientBrushProperties(
				D2D1::Point2F(50.0f, 150.0f),
				D2D1::Point2F(0.0f, 0.0f),
				50.0f, 50.0f
			),
			pGradientStops,
			&gpRadialBrush
		);

		if (pGradientStops)
		{
			pGradientStops->Release();
			pGradientStops = nullptr;
		}


	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	//MSG msg;
	//while(GetMessage(&msg, NULL, 0, 0))
	//{
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);

	//	OnPaint(hwnd);
	//}

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
			OnPaint(hwnd);
		}
	}

	// 4. 해제 (생성의 역순으로)
	if (gpRadialBrush)
	{
		gpRadialBrush->Release();
		gpRadialBrush = nullptr;
	}
	if (gpBrush)
	{
		gpBrush->Release();
		gpBrush = nullptr;
	}
	if (gpRenderTargetP != nullptr)
	{
		gpRenderTargetP->Release();
		gpRenderTargetP = nullptr;
	}
	if (gpD2DFactory)
	{
		gpD2DFactory->Release();
		gpD2DFactory = nullptr;
	}
	return static_cast<int>(msg.wParam);
}

void OnPaint(HWND hwnd)
{
	HDC hdc;
	PAINTSTRUCT ps;

	hdc = BeginPaint(hwnd, &ps);

	// 3. 그리기
	gpRenderTargetP->BeginDraw();
	gpRenderTargetP->Clear(D2D1::ColorF(0.0f, 0.2f, 0.4f, 1.0f));
	
	gpBrush->SetOpacity(1.0f);
	gpBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Aquamarine));
	gpRenderTargetP->FillRectangle(
		D2D1::RectF(0.0f, 0.0f, 100.0f, 100.0f),
		gpBrush
	);

	gpBrush->SetOpacity(0.5f);
	gpBrush->SetColor(D2D1::ColorF(D2D1::ColorF::LightYellow));
	gpRenderTargetP->FillRectangle(
		D2D1::RectF(50.0f, 50.0f, 150.0f, 150.0f),
		gpBrush
	);
	static float angle = 0.0f;

	gpRenderTargetP->FillEllipse(
		D2D1::Ellipse(D2D1::Point2F(75.0f + sinf(angle) * 25.0f, 150.0f), 50.0f, 50.0f),
		gpRadialBrush
	);

	angle+=0.1f;

	gpRenderTargetP->EndDraw();


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