#include "Engine/Core/Window.h"
#include "Engine/Core/Logger.h"
#include "Engine/Utility/Error.h"
#include "Engine/Utility/String.h"

#ifdef RV_PLATFORM_WINDOWS

rv::win32::WindowClass rv::Window::wclass;

rv::Result rv::win32::WindowClass::Create(WindowClass& window, const std::string& name, WNDPROC procedure)
{
	rv_result;

	window.name = name;
	window.instance = GetModuleHandle(nullptr);
	rif_win32_assert(window.instance);
	window.classex.cbSize = sizeof(WNDCLASSEX);
	window.classex.hInstance = window.instance;
	window.classex.lpszClassName = window.name.c_str();
	window.classex.lpfnWndProc = procedure;

	rif_win32_assert(RegisterClassEx(&window.classex));

	return result;
}


rv::Result rv::win32::Window::Create(Window& window, const std::string& title, uint width, uint height, bool resize)
{
	WindowDescriptor desc;
	desc.resize = resize;
	desc.size = { width, height };
	desc.title = title;

	return Create(window, desc);
}

rv::Result rv::win32::Window::Create(Window& window, const WindowDescriptor& descriptor)
{
	rv_result;

	if (wclass.name.empty())
		rv_rif(WindowClass::Create(wclass, "Rave Window Class", SetupProc));

	DWORD styleEx = 0;
	DWORD style =  WS_OVERLAPPEDWINDOW;
	if (!descriptor.resize)
		style &= ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;

	window.dpi = GetDpiForSystem();

	RECT rect{};
	rect.left = 0;
	rect.top = 0;
	rect.right = descriptor.size.width * window.dpi / 96;
	rect.bottom = descriptor.size.height * window.dpi / 96;
	rif_win32_check(AdjustWindowRect(&rect, style, false));

	rv_log(str(rect.bottom - rect.top));

	window.descriptor = descriptor;
	window.hwnd = CreateWindowEx(
		styleEx,
		wclass.name.c_str(),
		descriptor.title.c_str(),
		style,
		CW_USEDEFAULT, CW_USEDEFAULT,
		(int)rect.right - (int)rect.left,
		(int)rect.bottom - (int)rect.top,
		nullptr,
		nullptr,
		wclass.instance,
		&window
	);
	rif_win32_check_info(window.hwnd, str("Unable to create Window \"", descriptor.title, "\""));

	if (!window.hwnd)
		return failure;

	window.dpi = GetDpiForWindow(window.hwnd);

	rif_win32_check(SetWindowLongPtr(window.hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&window)));
	rif_win32_check(SetWindowLongPtr(window.hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(StaticWindowProc)));

	rif_win32_check(ShowWindow(window.hwnd, SW_NORMAL));

	rv_log(str("Created Window \"", descriptor.title, "\""));

	return result;
}

HWND rv::win32::Window::Expose() const
{
	return hwnd;
}

const rv::WindowDescriptor& rv::win32::Window::Descriptor() const
{
	return descriptor;
}

bool rv::win32::Window::Resizeable() const
{
	return descriptor.resize;
}

const rv::Extent2D& rv::win32::Window::Size() const
{
	return descriptor.size;
}

const rv::Point& rv::win32::Window::Position() const
{
	return position;
}

rv::Result rv::win32::Window::SetSize(const Extent2D& size)
{
	return rv_win32_assert(SetWindowPos(hwnd, nullptr, position.x, position.y, size.width, size.height, SWP_ASYNCWINDOWPOS | SWP_NOMOVE));
}

rv::Result rv::win32::Window::SetPosition(const Point& position)
{
	return rv_win32_assert(SetWindowPos(hwnd, nullptr, position.x, position.y, descriptor.size.width, descriptor.size.height, SWP_ASYNCWINDOWPOS | SWP_NOSIZE));
}

const std::string& rv::win32::Window::Title() const
{
	return descriptor.title;
}

rv::Result rv::win32::Window::SetTitle(const std::string& title)
{
	descriptor.title = title;
	return rv_win32_assert(SetWindowText(hwnd, title.c_str()));
}

bool rv::win32::Window::Minimized() const
{
	return minimized;
}

bool rv::win32::Window::Open() const
{
	return hwnd;
}

rv::Result rv::win32::Window::Close()
{
	return rv_win32_check(DestroyWindow(hwnd));
}

bool rv::win32::Window::HandleMessages()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return Open();
}

LRESULT rv::win32::Window::SetupProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_CREATE)
	{
		CREATESTRUCT* create = reinterpret_cast<CREATESTRUCT*>(lParam);
		if (create)
		{
			Window* window = reinterpret_cast<Window*>(create->lpCreateParams);
			if (window)
				return window->WindowProc(hwnd, msg, wParam, lParam);
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT rv::win32::Window::StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	if (window)
		return window->WindowProc(hwnd, msg, wParam, lParam);
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT rv::win32::Window::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_CLOSE:
		{
			Close();
		}
		break;
		case WM_DESTROY:
		{
			this->hwnd = nullptr;
		}
		break;

		case WM_SIZE:
		{
			if (wParam == SIZE_MINIMIZED)
			{
				minimized = true;
			}
			else
			{
				minimized = false;
				descriptor.size.width = (uint)LOWORD(lParam);
				descriptor.size.height = (uint)HIWORD(lParam);
			}
		}
		break;
		case WM_MOVE:
		{
			position.x = (int)LOWORD(lParam);
			position.y = (int)HIWORD(lParam);
		}
		break;

		case WM_DPICHANGED:
		{
			dpi = HIWORD(wParam);
			RECT* rect = reinterpret_cast<RECT*>(lParam);
			if (rect)
				SetWindowPos(hwnd, nullptr, rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top, 0);
		}
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

#endif
