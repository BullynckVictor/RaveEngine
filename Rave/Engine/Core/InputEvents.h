#pragma once
#include "Engine/Utility/Event.h"
#include "Engine/Utility/Flags.h"
#include "Engine/Utility/Vector.h"
#include "Engine/Core/SystemInclude.h"

namespace rv
{
	enum Key : u8;
	enum MouseButton : u8;

	static constexpr Identifier key_pressed_event		= "Key Pressed";
	static constexpr Identifier key_released_event		= "Key Released";
	static constexpr Identifier char_event				= "Character";

	static constexpr Identifier mouse_moved_event		= "Mouse Moved";
	static constexpr Identifier mbutton_pressed_event	= "Mouse Button Pressed";
	static constexpr Identifier mbutton_released_event	= "Mouse Button Released";

	struct KeyPressedEvent : public EventData
	{
		static constexpr Identifier type_id = key_pressed_event;

		KeyPressedEvent();
		KeyPressedEvent(Key key, bool repeated);

		Key key;
		bool repeated;
	};

	struct KeyReleasedEvent : public EventData
	{
		static constexpr Identifier type_id = key_released_event;

		KeyReleasedEvent();
		KeyReleasedEvent(Key key, bool repeated);

		Key key;
		bool repeated;
	};

	struct CharEvent : public EventData
	{
		static constexpr Identifier type_id = char_event;

		CharEvent();
		CharEvent(char character, bool repeated);

		char character;
		bool repeated;
	};

	struct MouseMovedEvent : public EventData
	{
		static constexpr Identifier type_id = mouse_moved_event;

		MouseMovedEvent();
		MouseMovedEvent(const Point& position, Flags<MouseButton> pressedButtons);

		Point position;
		Flags<MouseButton> pressedButtons;
	};

	struct MouseButtonPressedEvent : public EventData
	{
		static constexpr Identifier type_id = mbutton_pressed_event;

		MouseButtonPressedEvent();
		MouseButtonPressedEvent(MouseButton button, const Point& position, Flags<MouseButton> pressedButtons);

		MouseButton button;
		Point position;
		Flags<MouseButton> pressedButtons;
	};

	struct MouseButtonReleasedEvent : public EventData
	{
		static constexpr Identifier type_id = mbutton_released_event;

		MouseButtonReleasedEvent();
		MouseButtonReleasedEvent(MouseButton button, const Point& position, Flags<MouseButton> pressedButtons);

		MouseButton button;
		Point position;
		Flags<MouseButton> pressedButtons;
	};

	enum MouseButton : u8
	{
		RV_MOUSE_NULL	 = 0xFF,
		RV_MOUSE_LEFT	 = 0,
		RV_MOUSE_MIDDLE	 = 1,
		RV_MOUSE_RIGHT	 = 2,
		RV_MOUSE_X1		 = 3,
		RV_MOUSE_X2		 = 4,

		RV_MOUSE_CONTROL = 5,
		RV_MOUSE_SHIFT	 = 6,
	};

#	ifdef RV_PLATFORM_WINDOWS
	enum Key : u8
	{
		RV_KEY_NULL					 = 0,
		RV_KEY_LBUTTON				 = VK_LBUTTON,
		RV_KEY_RBUTTON				 = VK_RBUTTON,
		RV_KEY_CANCEL				 = VK_CANCEL,
		RV_KEY_BACK					 = VK_BACK,
		RV_KEY_TAB					 = VK_TAB,
		RV_KEY_CLEAR				 = VK_CLEAR,
		RV_KEY_RETURN				 = VK_RETURN,
		RV_KEY_SHIFT				 = VK_SHIFT,
		RV_KEY_CONTROL				 = VK_CONTROL,
		RV_KEY_MENU					 = VK_MENU,
		RV_KEY_PAUSE				 = VK_PAUSE,
		RV_KEY_CAPITAL				 = VK_CAPITAL,
		RV_KEY_ESCAPE				 = VK_ESCAPE,
		RV_KEY_CONVERT				 = VK_CONVERT,
		RV_KEY_NONCONVERT			 = VK_NONCONVERT,
		RV_KEY_ACCEPT				 = VK_ACCEPT,
		RV_KEY_MODECHANGE			 = VK_MODECHANGE,
		RV_KEY_SPACE				 = VK_SPACE,
		RV_KEY_PRIOR				 = VK_PRIOR,
		RV_KEY_NEXT					 = VK_NEXT,
		RV_KEY_END					 = VK_END,
		RV_KEY_HOME					 = VK_HOME,
		RV_KEY_LEFT					 = VK_LEFT,
		RV_KEY_UP					 = VK_UP,
		RV_KEY_RIGHT				 = VK_RIGHT,
		RV_KEY_DOWN					 = VK_DOWN,
		RV_KEY_SELECT				 = VK_SELECT,
		RV_KEY_PRINT				 = VK_PRINT,
		RV_KEY_EXECUTE				 = VK_EXECUTE,
		RV_KEY_SNAPSHOT				 = VK_SNAPSHOT,
		RV_KEY_INSERT				 = VK_INSERT,
		RV_KEY_DELETE				 = VK_DELETE,
		RV_KEY_HELP					 = VK_HELP,
		RV_KEY_0					 = '0',
		RV_KEY_1					 = '1',
		RV_KEY_2					 = '2',
		RV_KEY_3					 = '3',
		RV_KEY_4					 = '4',
		RV_KEY_5					 = '5',
		RV_KEY_6					 = '6',
		RV_KEY_7					 = '7',
		RV_KEY_8					 = '8',
		RV_KEY_9					 = '9',
		RV_KEY_A					 = 'A',
		RV_KEY_B					 = 'B',
		RV_KEY_C					 = 'C',
		RV_KEY_D					 = 'D',
		RV_KEY_E					 = 'E',
		RV_KEY_F					 = 'F',
		RV_KEY_G					 = 'G',
		RV_KEY_H					 = 'H',
		RV_KEY_I					 = 'I',
		RV_KEY_J					 = 'J',
		RV_KEY_K					 = 'K',
		RV_KEY_L					 = 'L',
		RV_KEY_M					 = 'M',
		RV_KEY_N					 = 'N',
		RV_KEY_O					 = 'O',
		RV_KEY_P					 = 'P',
		RV_KEY_Q					 = 'Q',
		RV_KEY_R					 = 'R',
		RV_KEY_S					 = 'S',
		RV_KEY_T					 = 'T',
		RV_KEY_U					 = 'U',
		RV_KEY_V					 = 'V',
		RV_KEY_W					 = 'W',
		RV_KEY_X					 = 'X',
		RV_KEY_Y					 = 'Y',
		RV_KEY_Z					 = 'Z',
		RV_KEY_LWIN					 = VK_LWIN,
		RV_KEY_RWIN					 = VK_RWIN,
		RV_KEY_APPS					 = VK_APPS,
		RV_KEY_SLEEP				 = VK_SLEEP,
		RV_KEY_NUMPAD0				 = VK_NUMPAD0,
		RV_KEY_NUMPAD1				 = VK_NUMPAD1,
		RV_KEY_NUMPAD2				 = VK_NUMPAD2,
		RV_KEY_NUMPAD3				 = VK_NUMPAD3,
		RV_KEY_NUMPAD4				 = VK_NUMPAD4,
		RV_KEY_NUMPAD5				 = VK_NUMPAD5,
		RV_KEY_NUMPAD6				 = VK_NUMPAD6,
		RV_KEY_NUMPAD7				 = VK_NUMPAD7,
		RV_KEY_NUMPAD8				 = VK_NUMPAD8,
		RV_KEY_NUMPAD9				 = VK_NUMPAD9,
		RV_KEY_MULTIPLY				 = VK_MULTIPLY,
		RV_KEY_ADD					 = VK_ADD,
		RV_KEY_SEPARATOR			 = VK_SEPARATOR,
		RV_KEY_SUBTRACT				 = VK_SUBTRACT,
		RV_KEY_DECIMAL				 = VK_DECIMAL,
		RV_KEY_DIVIDE				 = VK_DIVIDE,
		RV_KEY_F1					 = VK_F1,
		RV_KEY_F2					 = VK_F2,
		RV_KEY_F3					 = VK_F3,
		RV_KEY_F4					 = VK_F4,
		RV_KEY_F5					 = VK_F5,
		RV_KEY_F6					 = VK_F6,
		RV_KEY_F7					 = VK_F7,
		RV_KEY_F8					 = VK_F8,
		RV_KEY_F9					 = VK_F9,
		RV_KEY_F10					 = VK_F10,
		RV_KEY_F11					 = VK_F11,
		RV_KEY_F12					 = VK_F12,
		RV_KEY_F13					 = VK_F13,
		RV_KEY_F14					 = VK_F14,
		RV_KEY_F15					 = VK_F15,
		RV_KEY_F16					 = VK_F16,
		RV_KEY_F17					 = VK_F17,
		RV_KEY_F18					 = VK_F18,
		RV_KEY_F19					 = VK_F19,
		RV_KEY_F20					 = VK_F20,
		RV_KEY_F21					 = VK_F21,
		RV_KEY_F22					 = VK_F22,
		RV_KEY_F23					 = VK_F23,
		RV_KEY_F24					 = VK_F24,
		RV_KEY_NUMLOCK				 = VK_NUMLOCK,
		RV_KEY_SCROLL				 = VK_SCROLL,
		RV_KEY_LSHIFT				 = VK_LSHIFT,
		RV_KEY_RSHIFT				 = VK_RSHIFT,
		RV_KEY_LCONTROL				 = VK_LCONTROL,
		RV_KEY_RCONTROL				 = VK_RCONTROL,
		RV_KEY_LMENU				 = VK_LMENU,
		RV_KEY_RMENU				 = VK_RMENU,
		RV_KEY_BROWSER_BACK			 = VK_BROWSER_BACK,
		RV_KEY_BROWSER_FORWARD		 = VK_BROWSER_FORWARD,
		RV_KEY_BROWSER_REFRESH		 = VK_BROWSER_REFRESH,
		RV_KEY_BROWSER_STOP			 = VK_BROWSER_STOP,
		RV_KEY_BROWSER_SEARCH		 = VK_BROWSER_SEARCH,
		RV_KEY_BROWSER_FAVORITES	 = VK_BROWSER_FAVORITES,
		RV_KEY_BROWSER_HOME			 = VK_BROWSER_HOME,
		RV_KEY_VOLUME_MUTE			 = VK_VOLUME_MUTE,
		RV_KEY_VOLUME_DOWN			 = VK_VOLUME_DOWN,
		RV_KEY_VOLUME_UP			 = VK_VOLUME_UP,
		RV_KEY_MEDIA_NEXT_TRACK		 = VK_MEDIA_NEXT_TRACK,
		RV_KEY_MEDIA_PREV_TRACK		 = VK_MEDIA_PREV_TRACK,
		RV_KEY_MEDIA_STOP			 = VK_MEDIA_STOP,
		RV_KEY_MEDIA_PLAY_PAUSE		 = VK_MEDIA_PLAY_PAUSE,
		RV_KEY_LAUNCH_MAIL			 = VK_LAUNCH_MAIL,
		RV_KEY_LAUNCH_MEDIA_SELECT	 = VK_LAUNCH_MEDIA_SELECT,
		RV_KEY_LAUNCH_APP1			 = VK_LAUNCH_APP1,
		RV_KEY_LAUNCH_APP2			 = VK_LAUNCH_APP2,
		RV_KEY_OEM_PLUS				 = VK_OEM_PLUS,
		RV_KEY_OEM_COMMA			 = VK_OEM_COMMA,
		RV_KEY_OEM_MINUS			 = VK_OEM_MINUS,
		RV_KEY_OEM_PERIOD			 = VK_OEM_PERIOD,
		RV_KEY_PACKET				 = VK_PACKET,
		RV_KEY_ATTN					 = VK_ATTN,
		RV_KEY_CRSEL				 = VK_CRSEL,
		RV_KEY_EXSEL				 = VK_EXSEL,
		RV_KEY_EREOF				 = VK_EREOF,
		RV_KEY_PLAY					 = VK_PLAY,
		RV_KEY_ZOOM					 = VK_ZOOM,
		RV_KEY_PA1					 = VK_PA1,
	};
#	endif
}