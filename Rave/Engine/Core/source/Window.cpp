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
	window.classex.hCursor = LoadCursor(NULL, IDC_ARROW);

	rif_win32_check(RegisterClassEx(&window.classex));

	return result;
}


rv::win32::Window::~Window()
{
	SetWindowLongPtr(hwnd, GWLP_USERDATA, NULL);
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
	DWORD style = GetStyle(descriptor.resize);

	window.dpi = GetDpiForSystem();

	RECT rect{};
	rect.left = 0;
	rect.top = 0;
	rect.right = descriptor.size.width * window.dpi / 96;
	rect.bottom = descriptor.size.height * window.dpi / 96;
	rif_win32_check(AdjustWindowRect(&rect, style, false));

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

	SetWindowLongPtr(window.hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&window));
	SetWindowLongPtr(window.hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(StaticWindowProc));

	ShowWindow(window.hwnd, SW_NORMAL);

	rv_log(str("Created Window \"", descriptor.title, "\""));

	return result;
}

rv::win32::ExposedWindow rv::win32::Window::Expose() const
{
	ExposedWindow window;
	window.hwnd = hwnd;
	window.instance = wclass.instance;
	return window;
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
	return rv_win32_check(SetWindowPos(hwnd, nullptr, position.x, position.y, size.width, size.height, SWP_ASYNCWINDOWPOS | SWP_NOMOVE));
}

rv::Result rv::win32::Window::SetPosition(const Point& position)
{
	return rv_win32_check(SetWindowPos(hwnd, nullptr, position.x, position.y, descriptor.size.width, descriptor.size.height, SWP_ASYNCWINDOWPOS | SWP_NOSIZE));
}

const std::string& rv::win32::Window::Title() const
{
	return descriptor.title;
}

rv::Result rv::win32::Window::SetTitle(const std::string& title)
{
	descriptor.title = title;
	return rv_win32_check(SetWindowText(hwnd, title.c_str()));
}

bool rv::win32::Window::Minimized() const
{
	return minimized;
}

bool rv::win32::Window::Open() const
{
	return hwnd;
}

bool rv::win32::Window::Closed() const
{
	return !Open();
}

rv::Result rv::win32::Window::Close()
{
	return rv_win32_check(DestroyWindow(hwnd));
}

bool rv::win32::Window::FullScreen() const
{
	return !(GetWindowLongPtr(hwnd, GWL_STYLE) & WS_OVERLAPPEDWINDOW);
}

rv::Result rv::win32::Window::SetFullScreen(bool fullscreen)
{
	rv_result;

	if (fullscreen)
	{
		if (FullScreen())
			return result;

		MONITORINFO mi { sizeof(MONITORINFO) };
		rif_win32_assert(GetWindowPlacement(hwnd, &prevPlacement));
		rif_win32_assert(GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &mi));

		DWORD style = GetStyle(descriptor.resize);
		rif_win32_assert(SetWindowLongPtr(hwnd, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW));

		rif_win32_assert(SetWindowPos(
			hwnd, HWND_TOP,
			mi.rcMonitor.left, mi.rcMonitor.top,
			mi.rcMonitor.right - mi.rcMonitor.left,
			mi.rcMonitor.bottom - mi.rcMonitor.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED
		));
	}
	else
	{
		if (!FullScreen())
			return result;

		rif_win32_assert(SetWindowLong(hwnd, GWL_STYLE, GetStyle(descriptor.resize)));
		rif_win32_assert(SetWindowPlacement(hwnd, &prevPlacement));
		rif_win32_assert(SetWindowPos(
			hwnd, NULL, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED
		));
	}
	return result;
}

rv::Result rv::win32::Window::ToggleFullScreen()
{
	return SetFullScreen(!FullScreen());
}

bool rv::win32::Window::Resized()
{
	bool temp = resized;
	resized = false;
	return temp;
}

bool rv::win32::Window::PeekResized() const
{
	return resized;
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
			{
				return window->WindowProc(hwnd, msg, wParam, lParam);
			}
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT rv::win32::Window::StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	if (window)
	{
		return window->WindowProc(hwnd, msg, wParam, lParam);
	}
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
		return 0;
		case WM_DESTROY:
		{
			this->hwnd = nullptr;
			EmplaceEvent<WindowClosedEvent>();
		}
		return 0;

		case WM_SIZE:
		{
			if (wParam == SIZE_MINIMIZED)
			{
				minimized = true;
				EmplaceEvent<WindowResizedEvent>(Extent2D(), true);
			}
			else
			{
				minimized = false;
				if (wParam != SIZE_RESTORED)
					resized = true;
				descriptor.size.width = (uint)LOWORD(lParam);
				descriptor.size.height = (uint)HIWORD(lParam);
				EmplaceEvent<WindowResizedEvent>(descriptor.size);
			}
		}
		return 0;
		case WM_MOVE:
		{
			position.x = (int)LOWORD(lParam);
			position.y = (int)HIWORD(lParam);
			EmplaceEvent<WindowMovedEvent>(position);
		}
		return 0;

		case WM_DPICHANGED:
		{
			dpi = HIWORD(wParam);
			EmplaceEvent<WindowDpiChangedEvent>(dpi);
			RECT* rect = reinterpret_cast<RECT*>(lParam);
			if (rect)
				SetWindowPos(hwnd, nullptr, rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top, 0);
		}
		return 0;

		//keyboard events
		case WM_KEYDOWN:
		{
			BOOL repeatFlag = (HIWORD(lParam) & KF_REPEAT) == KF_REPEAT;
			EmplaceEvent<KeyPressedEvent>((Key)wParam, (bool)repeatFlag);
		}
		return 0;
		case WM_KEYUP:
		{
			BOOL repeatFlag = (HIWORD(lParam) & KF_REPEAT) == KF_REPEAT;
			EmplaceEvent<KeyReleasedEvent>((Key)wParam, (bool)repeatFlag);
		}
		return 0;
		case WM_CHAR:
		{
			BOOL repeatFlag = (HIWORD(lParam) & KF_REPEAT) == KF_REPEAT;
			EmplaceEvent<CharEvent>((char)wParam, (bool)repeatFlag);
		}
		return 0;

		//mouse events
		case WM_MOUSEMOVE:
		{
			POINTS points = MAKEPOINTS(lParam);
			PostEvent<MouseMovedEvent>({ { points.x, points.y }, ToMouseButtons(wParam) });
		}
		return 0;
		case WM_LBUTTONDOWN:
		{
			POINTS points = MAKEPOINTS(lParam);
			PostEvent<MouseButtonPressedEvent>({ RV_MOUSE_LEFT, { points.x, points.y }, ToMouseButtons(wParam) });
		}
		return 0;
		case WM_MBUTTONDOWN:
		{
			POINTS points = MAKEPOINTS(lParam);
			PostEvent<MouseButtonPressedEvent>({ RV_MOUSE_MIDDLE, { points.x, points.y }, ToMouseButtons(wParam) });
		}
		return 0;
		case WM_RBUTTONDOWN:
		{
			POINTS points = MAKEPOINTS(lParam);
			PostEvent<MouseButtonPressedEvent>({ RV_MOUSE_RIGHT, { points.x, points.y }, ToMouseButtons(wParam) });
		}
		return 0;
		case WM_XBUTTONDOWN:
		{
			POINTS points = MAKEPOINTS(lParam);
			PostEvent<MouseButtonPressedEvent>({ GET_XBUTTON_WPARAM(wParam) ? RV_MOUSE_X1 : RV_MOUSE_X2, { points.x, points.y }, ToMouseButtons(wParam) });
		}
		return 0;
		case WM_LBUTTONUP:
		{
			POINTS points = MAKEPOINTS(lParam);
			PostEvent<MouseButtonReleasedEvent>({ RV_MOUSE_LEFT, { points.x, points.y }, ToMouseButtons(wParam) });
		}
		return 0;
		case WM_MBUTTONUP:
		{
			POINTS points = MAKEPOINTS(lParam);
			PostEvent<MouseButtonReleasedEvent>({ RV_MOUSE_MIDDLE, { points.x, points.y }, ToMouseButtons(wParam) });
		}
		return 0;
		case WM_RBUTTONUP:
		{
			POINTS points = MAKEPOINTS(lParam);
			PostEvent<MouseButtonReleasedEvent>({ RV_MOUSE_RIGHT, { points.x, points.y }, ToMouseButtons(wParam) });
		}
		return 0;
		case WM_XBUTTONUP:
		{
			POINTS points = MAKEPOINTS(lParam);
			PostEvent<MouseButtonReleasedEvent>({ GET_XBUTTON_WPARAM(wParam) ? RV_MOUSE_X1 : RV_MOUSE_X2, { points.x, points.y }, ToMouseButtons(wParam) });
		}
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

constexpr rv::Flags<rv::MouseButton> rv::win32::Window::ToMouseButtons(WPARAM wParam)
{
	Flags<MouseButton> additionalButtons;
	switch (GET_KEYSTATE_WPARAM(wParam))
	{
		case MK_CONTROL:	additionalButtons |= RV_MOUSE_CONTROL; break;
		case MK_LBUTTON:	additionalButtons |= RV_MOUSE_LEFT; break;
		case MK_MBUTTON:	additionalButtons |= RV_MOUSE_MIDDLE; break;
		case MK_RBUTTON:	additionalButtons |= RV_MOUSE_RIGHT; break;
		case MK_SHIFT:		additionalButtons |= RV_MOUSE_SHIFT; break;
		case MK_XBUTTON1:	additionalButtons |= RV_MOUSE_X1; break;
		case MK_XBUTTON2:	additionalButtons |= RV_MOUSE_X2; break;
	}
	return additionalButtons;
}

DWORD rv::win32::Window::GetStyle(bool resize)
{
	DWORD style = WS_OVERLAPPEDWINDOW;
	if (!resize)
		style &= ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
	return style;
}

#endif
