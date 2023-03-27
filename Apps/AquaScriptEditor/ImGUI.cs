using AquaEngine;
using AquaEngine.Graphics;
using System.Runtime.CompilerServices;

namespace AquaEditor
{
	public static class ImGUI
	{
		public enum TextFlags : int
		{
			None = 0,

			/// <summary>
			/// Allow 0123456789.+-*/
			/// </summary>
			CharsDecimal = 1 << 0,

			/// <summary>
			/// Allow 0123456789ABCDEFabcdef
			/// </summary>
			CharsHexadecimal = 1 << 1,

			/// <summary>
			/// Turn a..z into A..Z
			/// </summary>
			CharsUppercase = 1 << 2,

			/// <summary>
			/// Filter out spaces and tabs
			/// </summary>
			CharsNoBlank = 1 << 3,

			/// <summary>
			/// Select entire text when first taking mouse focus
			/// </summary>
			AutoSelectAll = 1 << 4,

			/// <summary>
			/// Return 'true' when Enter is pressed (as opposed to every time the value was modified). Consider looking at the IsItemDeactivatedAfterEdit() function.
			/// </summary>
			EnterReturnsTrue = 1 << 5,

			/// <summary>
			/// Pressing TAB input places a '\t' character into the text field
			/// </summary>
			AllowTabInput = 1 << 10,

			/// <summary>
			/// In multi-line mode, unfocus with Enter, add new line with Ctrl+Enter (default is opposite: unfocus with Ctrl+Enter, add line with Enter).
			/// </summary>
			CtrlEnterForNewLine = 1 << 11,

			/// <summary>
			/// Disable following the cursor horizontally
			/// </summary>
			NoHorizontalScroll = 1 << 12,

			/// <summary>
			/// Overwrite mode
			/// </summary>
			AlwaysOverwrite = 1 << 13,

			/// <summary>
			/// Read-only mode
			/// </summary>
			ReadOnly = 1 << 14,

			/// <summary>
			/// Password mode, display all characters as '*'
			/// </summary>
			Password = 1 << 15,

			/// <summary>
			/// Disable undo/redo
			/// </summary>
			NoUndoRedo = 1 << 16,

			/// <summary>
			/// Allow 0123456789.+-*/eE (Scientific notation input)
			/// </summary>
			CharsScientific = 1 << 17,

			/// <summary>
			/// Escape key clears content if not empty, and deactivate otherwise (contrast to default behavior of Escape to revert)
			/// </summary>
			EscapeClearsAll = 1 << 20,
		}

		#region Window
		public static void Begin(string label) => _Begin(label);
		public static bool Begin(string label, ref bool open) => _BeginClosable(label, ref open);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Begin(string label);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _BeginClosable(string label, ref bool isOpen);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void End();
		#endregion

		#region State
		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool IsItemHovered();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool IsItemClicked();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool IsItemEdited();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void BeginDisabled();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void EndDisabled();
		#endregion

		#region Controls
		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void SameLine();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Space();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Text(string label);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool Button(string label);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool Checkbox(string label, ref bool checkState);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool ColourEdit3(string label, ref Colour colour);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool ColourEdit4(string label, ref Colour colour);
		#endregion

		#region Input
		public static bool Input(string label, ref string value, int maxCharacters = 128, TextFlags flags = TextFlags.None)
			=> _InputText(label, ref value, maxCharacters, (int)flags);

		public static bool InputTextMultiline(string label, ref string value, int maxCharacters = 2048, TextFlags flags = TextFlags.None)
			=> _InputTextMultiline(label, ref value, maxCharacters, (int)flags);

		public static bool InputPassword(string label, ref string value, int maxCharacters = 32, TextFlags flags = TextFlags.None)
			=> _InputText(label, ref value, maxCharacters, (int)(flags | TextFlags.Password));

		public static bool Input(string label, ref float value, float step = 0.0f, float stepFast = 0.0f, string format = "%.3f")
			=> _InputFloat(label, ref value, step, stepFast, format);

		public static bool Input(string label, ref Vector2 value, string format = "%.3f") => _InputFloat2(label, ref value, format);
		public static bool Input(string label, ref Vector3 value, string format = "%.3f") => _InputFloat3(label, ref value, format);
		public static bool Input(string label, ref Vector4 value, string format = "%.3f") => _InputFloat4(label, ref value, format);
		public static bool Input(string label, ref Quaternion value, string format = "%.3f") => _InputQuat(label, ref value, format);

		public static bool Input(string label, ref int value, int step = 1, int stepFast = 100)
			=> _InputInt(label, ref value, step, stepFast);

		public static bool Input(string label, ref IVector2 value) => _InputInt2(label, ref value);
		public static bool Input(string label, ref IVector3 value) => _InputInt3(label, ref value);
		public static bool Input(string label, ref IVector4 value) => _InputInt4(label, ref value);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _InputText(string label, ref string value, int maxCharacters, int flags);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _InputTextMultiline(string label, ref string value, int maxCharacters, int flags);
		
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _InputFloat(string label, ref float value, float step, float stepFast, string format);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _InputFloat2(string label, ref Vector2 value, string format);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _InputFloat3(string label, ref Vector3 value, string format);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _InputFloat4(string label, ref Vector4 value, string format);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _InputQuat(string label, ref Quaternion value, string format);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _InputInt(string label, ref int value, int step, int stepFast);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _InputInt2(string label, ref IVector2 value);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _InputInt3(string label, ref IVector3 value);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _InputInt4(string label, ref IVector4 value);
		#endregion

		#region Image
		private static Colour s_ImageDefaultColour = Colour.White;
		private static Colour s_ImageDefaultBorder = new Colour(0, 0, 0, 0);
		private static Colour s_ButtonImageDefaultBackground = new Colour(0, 0, 0, 0);

		public static void Image(uint textureID, Vector2 size) => _Image(textureID, ref size, ref s_ImageDefaultColour, ref s_ImageDefaultBorder);
		public static void Image(uint textureID, Vector2 size, Colour tint) => _Image(textureID, ref size, ref tint, ref s_ImageDefaultBorder);
		public static void Image(uint textureID, Vector2 size, Colour tint, Colour border) => _Image(textureID, ref size, ref tint, ref border);

		public static void Image(Texture texture, Vector2 size) => _Image(texture?.ResourceID ?? 0, ref size, ref s_ImageDefaultColour, ref s_ImageDefaultBorder);
		public static void Image(Texture texture, Vector2 size, Colour tint) => _Image(texture?.ResourceID ?? 0, ref size, ref tint, ref s_ImageDefaultBorder);
		public static void Image(Texture texture, Vector2 size, Colour tint, Colour border) => _Image(texture?.ResourceID ?? 0, ref size, ref tint, ref border);

		public static bool ButtonImage(uint textureID, Vector2 size, int framePadding = -1) => _ImageButton(textureID, ref size, framePadding, ref s_ImageDefaultColour, ref s_ButtonImageDefaultBackground);
		public static bool ButtonImage(uint textureID, Vector2 size, Colour tint, int framePadding = -1) => _ImageButton(textureID, ref size, framePadding, ref tint, ref s_ButtonImageDefaultBackground);
		public static bool ButtonImage(uint textureID, Vector2 size, Colour tint, Colour backgroundColour, int framePadding = -1) => _ImageButton(textureID, ref size, framePadding, ref tint, ref backgroundColour);

		public static bool ButtonImage(Texture texture, Vector2 size, int framePadding = -1) => _ImageButton(texture?.ResourceID ?? 0, ref size, framePadding, ref s_ImageDefaultColour, ref s_ButtonImageDefaultBackground);
		public static bool ButtonImage(Texture texture, Vector2 size, Colour tint, int framePadding = -1) => _ImageButton(texture?.ResourceID ?? 0, ref size, framePadding, ref tint, ref s_ButtonImageDefaultBackground);
		public static bool ButtonImage(Texture texture, Vector2 size, Colour tint, Colour backgroundColour, int framePadding = -1) => _ImageButton(texture?.ResourceID ?? 0, ref size, framePadding, ref tint, ref backgroundColour);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Image(uint textureID, ref Vector2 size, ref Colour tint, ref Colour borderTint);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _ImageButton(uint textureID, ref Vector2 size, int framePadding, ref Colour tint, ref Colour backgroundColour);
		#endregion

		#region Drag
		public static bool Drag(string label, ref float value, float speed = 1.0f, float min = 0.0f, float max = 0.0f, string format = "%.3f")
			=> _DragFloat(label, ref value, speed, min, max, format);

		public static bool Drag(string label, ref Vector2 value, float speed = 1.0f, float min = 0.0f, float max = 0.0f, string format = "%.3f")
			=> _DragFloat2(label, ref value, speed, min, max, format);

		public static bool Drag(string label, ref Vector3 value, float speed = 1.0f, float min = 0.0f, float max = 0.0f, string format = "%.3f")
			=> _DragFloat3(label, ref value, speed, min, max, format);

		public static bool Drag(string label, ref int value, float speed = 1, int min = 0, int max = 0, string format = "%i")
			=> _DragInt(label, ref value, speed, min, max, format);

		public static bool Drag(string label, ref IVector2 value, float speed = 1, int min = 0, int max = 0, string format = "%i")
			=> _DragInt2(label, ref value, speed, min, max, format);

		public static bool Drag(string label, ref IVector3 value, float speed = 1, int min = 0, int max = 0, string format = "%i")
			=> _DragInt3(label, ref value, speed, min, max, format);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _DragFloat(
			string label,
			ref float value,
			float speed,
			float min,
			float max,
			string format
		);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _DragFloat2(
			string label,
			ref Vector2 value,
			float speed,
			float min,
			float max,
			string format
		);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _DragFloat3(
			string label,
			ref Vector3 value,
			float speed,
			float min,
			float max,
			string format
		);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _DragInt(
			string label,
			ref int value,
			float speed,
			int min,
			int max,
			string format
		);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _DragInt2(
			string label,
			ref IVector2 value,
			float speed,
			int min,
			int max,
			string format
		);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _DragInt3(
			string label,
			ref IVector3 value,
			float speed,
			int min,
			int max,
			string format
		);
		#endregion

		#region Slider
		public static bool Slider(string label, ref float value, float min = 0.0f, float max = 0.0f, string format = "%.3f")
			=> _SliderFloat(label, ref value, min, max, format);

		public static bool Slider(string label, ref Vector2 value, float min = 0.0f, float max = 0.0f, string format = "%.3f")
			=> _SliderFloat2(label, ref value, min, max, format);

		public static bool Slider(string label, ref Vector3 value, float min = 0.0f, float max = 0.0f, string format = "%.3f")
			=> _SliderFloat3(label, ref value, min, max, format);

		public static bool Slider(string label, ref int value, int min = 0, int max = 0, string format = "%i")
			=> _SliderInt(label, ref value, min, max, format);

		public static bool Slider(string label, ref IVector2 value, int min = 0, int max = 0, string format = "%i")
			=> _SliderInt2(label, ref value, min, max, format);

		public static bool Slider(string label, ref IVector3 value, int min = 0, int max = 0, string format = "%i")
			=> _SliderInt3(label, ref value, min, max, format);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool SliderAngle(string label, ref float valueRads, float degreesMin = -360.0f, float degreesMax = 360.0f, string format = "%.0f degrees");

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _SliderFloat(
			string label,
			ref float value,
			float min,
			float max,
			string format
		);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _SliderFloat2(
			string label,
			ref Vector2 value,
			float min,
			float max,
			string format
		);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _SliderFloat3(
			string label,
			ref Vector3 value,
			float min,
			float max,
			string format
		);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _SliderInt(
			string label,
			ref int value,
			int min,
			int max,
			string format
		);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _SliderInt2(
			string label,
			ref IVector2 value,
			int min,
			int max,
			string format
		);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _SliderInt3(
			string label,
			ref IVector3 value,
			int min,
			int max,
			string format
		);
		#endregion

		#region Tooltip
		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void SetTooltip(string label);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool BeginTooltip();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void EndTooltip();
		#endregion
	}
}