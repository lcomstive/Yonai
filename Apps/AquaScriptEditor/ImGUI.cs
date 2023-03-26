using AquaEngine;
using AquaEngine.Graphics;
using System.Runtime.CompilerServices;

namespace AquaEditor
{
	public static class ImGUI
	{
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
	}
}