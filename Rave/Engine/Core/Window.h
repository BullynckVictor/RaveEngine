#pragma once
#include "Engine/Core/SystemInclude.h"
#include "Engine/Utility/Result.h"
#include "Engine/Utility/Vector.h"

namespace rv
{
	struct WindowDescriptor
	{
		WindowDescriptor() = default;

		std::string title;
		Extent2D size;
		bool resize = false;
	};
}

#ifdef RV_PLATFORM_WINDOWS
namespace rv
{
	namespace win32
	{
		struct WindowClass
		{
			WindowClass() = default;

			static Result Create(WindowClass& window, const std::string& name, WNDPROC procedure);

			WNDCLASSEX classex = { sizeof(WNDCLASSEX) };
			HINSTANCE instance = GetModuleHandle(nullptr);
			std::string name;
		};

		struct ExposedWindow
		{
			ExposedWindow() = default;
			HWND hwnd = nullptr;
			HINSTANCE instance = nullptr;
		};

		class Window
		{
		public:
			Window() = default;

			static Result Create(Window& window, const std::string& title, uint width, uint height, bool resize = false);
			static Result Create(Window& window, const WindowDescriptor& descriptor);

			ExposedWindow Expose() const;

			const WindowDescriptor& Descriptor() const;

			bool Resizeable() const;
			const Extent2D& Size() const;
			const Point& Position() const;

			Result SetSize(const Extent2D& size);
			Result SetPosition(const Point& position);

			const std::string& Title() const;
			Result SetTitle(const std::string& title);

			bool Minimized() const;
			bool Open() const;
			Result Close();


			bool HandleMessages();

		private:
			static LRESULT CALLBACK SetupProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
			static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
			LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		private:
			HWND hwnd = nullptr;
			WindowDescriptor descriptor;
			bool minimized = true;
			uint dpi;
			Point position;
			
			static WindowClass wclass;
		};
	}

	typedef win32::Window Window;
}
#endif