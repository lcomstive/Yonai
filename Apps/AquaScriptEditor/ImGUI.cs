using AquaEngine;
using System;
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

		#region Drag
		public static bool Drag(string label, ref float value, float speed = 1.0f, float min = 0.0f, float max = 0.0f, string format = "%.3f")
			=> _DragFloat(label, ref value, speed, min, max, format);

		public static bool Drag(string label, ref Vector2 value, float speed = 1.0f, float min = 0.0f, float max = 0.0f, string format = "%.3f")
			=> _DragFloat2(label, ref value, speed, min, max, format);

		public static bool Drag(string label, ref Vector3 value, float speed = 1.0f, float min = 0.0f, float max = 0.0f, string format = "%.3f")
			=> _DragFloat3(label, ref value, speed, min, max, format);
		
		public static bool Drag(string label, ref int value, int speed = 1, int min = 0, int max = 0, string format = "%i")
			=> _DragInt(label, ref value, speed, min, max, format);
		
		public static bool Drag(string label, ref IVector2 value, int speed = 1, int min = 0, int max = 0, string format = "%i")
			=> _DragInt2(label, ref value, speed, min, max, format);
		
		public static bool Drag(string label, ref IVector3 value, int speed = 1, int min = 0, int max = 0, string format = "%i")
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
			int speed,
			int min,
			int max,
			string format
		);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _DragInt2(
			string label,
			ref IVector2 value,
			int speed,
			int min,
			int max,
			string format
		);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _DragInt3(
			string label,
			ref IVector3 value,
			int speed,
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