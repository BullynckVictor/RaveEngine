#include "Engine/Core/InputEvents.h"

rv::KeyPressedEvent::KeyPressedEvent()
	:
	EventData(type_id),
	key(RV_KEY_NULL),
	repeated(false)
{
}

rv::KeyPressedEvent::KeyPressedEvent(Key key, bool repeated)
	:
	EventData(type_id),
	key(key),
	repeated(repeated)
{
}

rv::KeyReleasedEvent::KeyReleasedEvent()
	:
	EventData(type_id),
	key(RV_KEY_NULL),
	repeated(false)
{
}

rv::KeyReleasedEvent::KeyReleasedEvent(Key key, bool repeated)
	:
	EventData(type_id),
	key(key),
	repeated(repeated)
{
}

rv::CharEvent::CharEvent()
	:
	EventData(type_id),
	character(0),
	repeated(false)
{
}

rv::CharEvent::CharEvent(char character, bool repeated)
	:
	EventData(type_id),
	character(character),
	repeated(repeated)
{
}

rv::MouseMovedEvent::MouseMovedEvent()
	:
	EventData(type_id)
{
}

rv::MouseMovedEvent::MouseMovedEvent(const Point& position, Flags<MouseButton> pressedButtons)
	:
	EventData(type_id),
	position(position),
	pressedButtons(pressedButtons)
{
}

rv::MouseButtonPressedEvent::MouseButtonPressedEvent()
	:
	EventData(type_id),
	button(RV_MOUSE_NULL)
{
}

rv::MouseButtonPressedEvent::MouseButtonPressedEvent(MouseButton button, const Point& position, Flags<MouseButton> pressedButtons)
	:
	EventData(type_id),
	button(button),
	position(position),
	pressedButtons(pressedButtons)
{
}

rv::MouseButtonReleasedEvent::MouseButtonReleasedEvent()
	:
	EventData(type_id),
	button(RV_MOUSE_NULL)
{
}

rv::MouseButtonReleasedEvent::MouseButtonReleasedEvent(MouseButton button, const Point& position, Flags<MouseButton> pressedButtons)
	:
	EventData(type_id),
	button(button),
	position(position),
	pressedButtons(pressedButtons)
{
}
