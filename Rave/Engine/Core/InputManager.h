#pragma once
#include "Engine/Core/Window.h"

namespace rv
{
	class Keyboard
	{
	public:
		Keyboard();

		bool KeyPressed(Key key) const;
		u16  Flagged(Key key);
		u16  Peek(Key key) const;
		void Flush();
		void Flush(Key key);

	private:
		std::array<bool, 255> keys;
		std::array<u16, 255> flagged;

		friend class InputManager;
	};

	class Mouse
	{
	public:
		const Vector2& Position() const;
		const Vector2& WorldPosition() const;
		const Point& ScreenPosition() const;

		class Button
		{
		public:
			bool Pressed() const;
			u16  Flagged();
			u16  Peek() const;
			void Flush();

		private:
			bool pressed = false;
			u16  flagged = 0;

			friend class InputManager;
		};

		Button left;
		Button middle;
		Button right;
		Button x1;
		Button x2;

		Button& GetButton(MouseButton button);
		const Button& GetButton(MouseButton button) const;

	private:
		Vector2 position;
		Vector2 worldPosition;
		Point screenPosition;

		friend class InputManager;
	};

	class InputManager
	{
	public:
		InputManager() = default;
		InputManager(Window& window);

		void HandleInput();
		
		std::string InputString();
		const std::string& PeekInputString() const;
		bool InputStringEmpty() const;
		bool InputStringChanged() const;

	public:
		Keyboard keyboard;
		Mouse mouse;

	private:
		EventListener listener;
		Extent2D size;
		std::string inputString;
	};
}