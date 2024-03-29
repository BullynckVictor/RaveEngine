#pragma once
#include "Engine/Core/Window.h"
#include <list>
#include <map>

namespace rv
{
	template<typename K>
	concept KeyConcept = requires(K key) {
		(Key)key;
	};

	class Keyboard
	{
	public:
		Keyboard();

		bool KeyPressed(Key key) const;
		u16  KeyFlagged(Key key);
		u16  PeekKey(Key key) const;
		void Flush();
		void Flush(Key key);

		bool KeyPressed(char key) const;
		u16  KeyFlagged(char key);
		u16  PeekKey(char key) const;
		void Flush(char key);

		template<KeyConcept K, KeyConcept... Keys>
		bool KeysPressed(K key, Keys... keys) const
		{
			if constexpr (sizeof...(Keys) != 0)
				return KeyPressed(key) && KeysPressed(keys...);
			else
				return KeyPressed(key);
		}

		template<KeyConcept K, KeyConcept... Keys>
		bool AnyKeyPressed(K key, Keys... keys) const
		{
			if constexpr (sizeof...(Keys) != 0)
				return KeyPressed(key) || AnyKeyPressed(keys...);
			else
				return KeyPressed(key);
		}

		template<KeyConcept K, KeyConcept... Keys>
		bool AnyFlagged(K key, Keys... keys)
		{
			if constexpr (sizeof...(Keys) != 0)
				return Flagged(key) || AnyFlagged(keys...);
			else
				return Flagged(key);
		}

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