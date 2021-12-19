#include "Engine/Core/InputManager.h"
#include "Engine/Utility/Error.h"
#include "Engine/Utility/String.h"
#include <stdarg.h>

rv::InputManager::InputManager(Window& window)
	:
	listener(window)
{
}

void rv::InputManager::HandleInput()
{
	bool posChanged = false;
	while (Event e = listener.GetEvent())
	{
		switch (e.ID())
		{
			case window_resized_event:
			{
				auto& event = e.get<WindowResizedEvent>();
				if (!event.minimized)
				{
					size = event.size;
					posChanged = true;
				}
			}
			break;

			case key_pressed_event:
			{
				auto& event = e.get<KeyPressedEvent>();
				if (!event.repeated)
				{
					keyboard.keys[event.key] = true;
					++keyboard.flagged[event.key];
				}
			}
			break;

			case key_released_event:
			{
				auto& event = e.get<KeyPressedEvent>();
				keyboard.keys[event.key] = false;
			}
			break;

			case char_event:
			{
				inputString.push_back(e.get<CharEvent>().character);
			}
			break;

			case mouse_moved_event:
			{
				auto& event = e.get<MouseMovedEvent>();
				mouse.screenPosition = event.position;
				posChanged = true;
			}
			break;

			case mbutton_pressed_event:
			{
				auto& event = e.get<MouseButtonPressedEvent>();
				auto& button = mouse.GetButton(event.button);
				button.pressed = true;
				++button.flagged;
			};
			break;

			case mbutton_released_event:
			{
				auto& event = e.get<MouseButtonPressedEvent>();
				auto& button = mouse.GetButton(event.button);
				button.pressed = false;
			};
			break;
		}
	}

	if (posChanged)
	{
		if (keyboard.KeyPressed(RV_KEY_SPACE))
			int i = 0;
		float min = (float)std::min(size.width, size.height) / 2;
		mouse.position.x =  float(mouse.screenPosition.x - (int)size.width  / 2) / min;
		mouse.position.y = -float(mouse.screenPosition.y - (int)size.height / 2) / min;
		mouse.worldPosition = mouse.position;
	}
}

std::string rv::InputManager::InputString()
{
	std::string temp = inputString;
	inputString.clear();
	return temp;
}

const std::string& rv::InputManager::PeekInputString() const
{
	return inputString;
}

bool rv::InputManager::InputStringEmpty() const
{
	return inputString.empty();
}

bool rv::InputManager::InputStringChanged() const
{
	return !inputString.empty();
}

rv::Keyboard::Keyboard()
	:
	keys(),
	flagged()
{
	keys.fill(false);
	flagged.fill(0);
}

bool rv::Keyboard::KeyPressed(Key key) const
{
	return keys[key];
}

rv::u16 rv::Keyboard::KeyFlagged(Key key)
{
	u16 f = flagged[key];
	if (f != 0)
		--flagged[key];
	return f;
}

rv::u16 rv::Keyboard::PeekKey(Key key) const
{
	return flagged[key];
}

void rv::Keyboard::Flush()
{
	flagged.fill(0);
}

void rv::Keyboard::Flush(Key key)
{
	flagged[key] = 0;
}

bool rv::Keyboard::KeyPressed(char key) const
{
	return KeyPressed((Key)key);
}

rv::u16 rv::Keyboard::KeyFlagged(char key)
{
	return KeyFlagged((Key)key);
}

rv::u16 rv::Keyboard::PeekKey(char key) const
{
	return PeekKey((Key)key);
}

void rv::Keyboard::Flush(char key)
{
	return Flush((Key)key);
}

const rv::Vector2& rv::Mouse::Position() const
{
	return position;
}

const rv::Vector2& rv::Mouse::WorldPosition() const
{
	return worldPosition;
}

const rv::Point& rv::Mouse::ScreenPosition() const
{
	return screenPosition;
}

rv::Mouse::Button& rv::Mouse::GetButton(MouseButton button)
{
	switch (button)
	{
		case RV_MOUSE_LEFT:		return left;
		case RV_MOUSE_MIDDLE:	return middle;
		case RV_MOUSE_RIGHT:	return right;
		case RV_MOUSE_X1:		return x1;
		case RV_MOUSE_X2:		return x2;
	}
	rv_throw("Unable to return Mouse Button of type 'NULL', 'SHIFT' or 'CONTROL'");
	throw 0;
}

const rv::Mouse::Button& rv::Mouse::GetButton(MouseButton button) const
{
	switch (button)
	{
		case RV_MOUSE_LEFT:		return left;
		case RV_MOUSE_MIDDLE:	return middle;
		case RV_MOUSE_RIGHT:	return right;
		case RV_MOUSE_X1:		return x1;
		case RV_MOUSE_X2:		return x2;
	}
	rv_throw("Unable to return Mouse Button of type 'NULL', 'SHIFT' or 'CONTROL'");
	throw 0;
}

bool rv::Mouse::Button::Pressed() const
{
	return pressed;
}

rv::u16 rv::Mouse::Button::Flagged()
{
	u16 temp = flagged;
	if (flagged != 0)
		--flagged;
	return temp;
}

rv::u16 rv::Mouse::Button::Peek() const
{
	return flagged;
}

void rv::Mouse::Button::Flush()
{
	flagged = 0;
}