#pragma once
#include "Engine/Core/SystemInclude.h"
#include "Engine/Core/InputEvents.h"
#include "Engine/Utility/Result.h"
#include "Engine/Utility/Vector.h"
#include "Engine/Utility/Event.h"

namespace rv
{
	struct WindowDescriptor
	{
		WindowDescriptor() = default;

		std::string title;
		Extent2D size;
		bool resize = false;
	};

	static constexpr Identifier window_closed_event			= Identifier("Window Closed Event");
	static constexpr Identifier window_resized_event		= Identifier("Window Resize Event");
	static constexpr Identifier window_moved_event			= Identifier("Window Moved Event");
	static constexpr Identifier window_dpi_changed_event	= Identifier("Window DPI Changed Event");

	struct WindowClosedEvent : public EventData
	{
		static constexpr Identifier type_id = window_closed_event;
		WindowClosedEvent() : EventData(type_id) {}
	};

	struct WindowResizedEvent : public EventData
	{
		static constexpr Identifier type_id = window_resized_event;
		WindowResizedEvent(Extent2D size = {}, bool minimized = false, bool maximized = false) 
			: 
			EventData(type_id), size(size), minimized(minimized), maximized(maximized)
		{}

		Extent2D size;
		bool minimized;
		bool maximized;
	};

	struct WindowMovedEvent : public EventData
	{
		static constexpr Identifier type_id = window_moved_event;
		WindowMovedEvent(Point position = {})
			:
			EventData(type_id), position(position)
		{}

		Point position;
	};

	struct WindowDpiChangedEvent : public EventData
	{
		static constexpr Identifier type_id = window_dpi_changed_event;
		WindowDpiChangedEvent(uint dpi = 96)
			:
			EventData(type_id), dpi(dpi)
		{}

		uint dpi;
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

		class Window : public EventQueue
		{
		public:
			Window() = default;
			~Window();

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
			bool Closed() const;
			Result Close();

			bool FullScreen() const;
			Result SetFullScreen(bool fullscreen);
			Result ToggleFullScreen();

			bool Resized();
			bool PeekResized() const;

			bool HandleMessages();

		private:
			static LRESULT CALLBACK SetupProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
			static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
			LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

			static constexpr Flags<MouseButton> ToMouseButtons(WPARAM wParam);
			static DWORD GetStyle(bool resize);

		private:
			HWND hwnd = nullptr;
			WindowDescriptor descriptor;
			bool minimized = true;
			uint dpi = 96;
			Point position;
			bool resized = false;

			WINDOWPLACEMENT prevPlacement{ sizeof(WINDOWPLACEMENT) };
			
			static WindowClass wclass;
		};
	}

	typedef win32::Window Window;
}
#endif