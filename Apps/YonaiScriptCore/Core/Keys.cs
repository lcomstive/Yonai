namespace Yonai
{
	public enum Key : int
	{
		Unknown = -1,

		/* Printable keys */
		Space = 32,
		Apostrophe = 39,  /* ' */
		Comman = 44,  /* , */
		Minus = 45,  /* - */
		Period = 46,  /* . */
		Slash = 47,  /* / */
		Num0 = 48,
		Num1 = 49,
		Num2 = 50,
		Num3 = 51,
		Num4 = 52,
		Num5 = 53,
		Num6 = 54,
		Num7 = 55,
		Num8 = 56,
		Num9 = 57,
		Semicolon = 59,  /* ; */
		Equal = 61,  /* = */
		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,
		LeftBracket = 91,  /* [ */
		Backslash = 92,  /* \ */
		RightBracket = 93,  /* ] */
		Tilde = 96,  /* ` */
		World1 = 161, /* non-US #1 */
		World2 = 162, /* non-US #2 */

		Escape = 256,
		Enter = 257,
		TAB = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		ArrowRight = 262,
		ArrowLeft = 263,
		ArrowDown = 264,
		ArrowUp = 265,
		PageUp = 266,
		Pagedown = 267,
		HOME = 268,
		END = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,
		KP_0 = 320,
		KP_1 = 321,
		KP_2 = 322,
		KP_3 = 323,
		KP_4 = 324,
		KP_5 = 325,
		KP_6 = 326,
		KP_7 = 327,
		KP_8 = 328,
		KP_9 = 329,
		KP_DECIMAL = 330,
		KP_DIVIDE = 331,
		KP_MULTIPLY = 332,
		KP_SUBTRACT = 333,
		KP_ADD = 334,
		KP_ENTER = 335,
		KP_EQUAL = 336,
		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		MENU = 348,
	}

	public enum GamepadButton : int
	{
		// Generic Naming //
		South = 0,
		East = 1,
		West = 2,
		North = 3,

		// XBox Naming //
		A = South,
		B = East,
		X = West,
		Y = North,

		// PlayStation Naming //
		Cross = A,
		Circle = B,
		Square = X,
		Triangle = Y,

		// Universal //
		LeftBumper = 4,
		RightBumper = 5,
		Back = 6,
		Start = 7,
		Guide = 8,
		LeftThumb = 9,
		RightThumb = 10,

		// DPad //
		DPadUp = 11,
		DPadRight = 12,
		DPadDown = 13,
		DPadLeft = 14
	}

	public enum GamepadAxis : int
	{
		AxisLeftX = 0,
		AxisLeftY = 1,
		AxisRightX = 2,
		AxisRightY = 3,
		AxisLeftTrigger = 4,
		AxisRightTrigger = 5
	}

	public enum MouseButton : byte
	{
		Left	= 0,
		Right	= 1,
		Middle	= 2
	}

	public enum MouseState : int
	{
		/// <summary>
		/// Normal, visible state of mouse
		/// </summary>
		Normal = 0,

		/// <summary>
		/// Hides the mouse, but is still moving
		/// </summary>
		Hidden,

		/// <summary>
		/// Hides the mouse and prevents movement
		/// </summary>
		Disabled
	};
}
