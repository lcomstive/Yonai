using AquaEngine;
using AquaEngine.Graphics;
using System.Runtime.CompilerServices;

namespace AquaEditor
{
	public static class ImGUI
	{
		public struct Viewport
		{
			public uint ID, ParentID;
			public Vector2 Position, Size, WorkPosition, WorkSize;
		}

		[System.Flags]
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

		[System.Flags]
		public enum WindowFlags
		{
			None = 0,

			/// <summary>
			/// Disable title-bar
			/// </summary>
			NoTitleBar = 1 << 0,

			/// <summary>
			/// Disable user resizing with the lower-right grip
			/// </summary>
			NoResize = 1 << 1,

			/// <summary>
			/// Disable user moving the window
			/// </summary>
			NoMove = 1 << 2,

			/// <summary>
			/// Disable scrollbars (window can still scroll with mouse or programmatically)
			/// </summary>
			NoScrollbar = 1 << 3,

			/// <summary>
			/// Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
			/// </summary>
			NoScrollWithMouse = 1 << 4,

			/// <summary>
			/// Disable user collapsing window by double-clicking on it. Also referred to as Window Menu Button (e.g. within a docking node).
			/// </summary>
			NoCollapse = 1 << 5,

			/// <summary>
			/// Resize every window to its content every frame
			/// </summary>
			AlwaysAutoResize = 1 << 6,

			/// <summary>
			/// Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
			/// </summary>
			NoBackground = 1 << 7,

			/// <summary>
			/// Never load/save settings in .ini file
			/// </summary>
			NoSavedSettings = 1 << 8,

			/// <summary>
			/// Disable catching mouse, hovering test with pass through.
			/// </summary>
			NoMouseInputs = 1 << 9,

			/// <summary>
			/// Has a menu-bar
			/// </summary>
			MenuBar = 1 << 10,

			/// <summary>
			/// Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(Vector2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
			/// </summary>
			HorizontalScrollbar = 1 << 11,

			/// <summary>
			/// Disable taking focus when transitioning from hidden to visible state
			/// </summary>
			NoFocusOnAppearing = 1 << 12,

			/// <summary>
			/// Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
			/// </summary>
			NoBringToFrontOnFocus = 1 << 13,

			/// <summary>
			/// Always show vertical scrollbar (even if ContentSize.y < Size.y)
			/// </summary>
			AlwaysVerticalScrollbar = 1 << 14,

			/// <summary>
			/// Always show horizontal scrollbar (even if ContentSize.x < Size.x)
			/// </summary>
			AlwaysHorizontalScrollbar = 1 << 15,

			/// <summary>
			/// Ensure child windows without border uses style.WindowPadding (ignored by default for non-bordered child windows, because more convenient)
			/// </summary>
			AlwaysUseWindowPadding = 1 << 16,

			/// <summary>
			/// No gamepad/keyboard navigation within the window
			/// </summary>
			NoNavInputs = 1 << 18,

			/// <summary>
			/// No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
			/// </summary>
			NoNavFocus = 1 << 19,

			/// <summary>
			/// Display a dot next to the title. When used in a tab/docking context, tab is selected when clicking the X + closure is not assumed (will wait for user to stop submitting the tab). Otherwise closure is assumed when pressing the X, so if you keep submitting the tab may reappear at end of tab bar.
			/// </summary>
			UnsavedDocument = 1 << 20,

			/// <summary>
			/// Disable docking of this window
			/// </summary>
			NoDocking = 1 << 21,

			NoNav = NoNavInputs | NoNavFocus,
			NoDecoration = NoTitleBar | NoResize | NoScrollbar | NoCollapse,
			NoInputs = NoMouseInputs | NoNavInputs | NoNavFocus
		};

		public enum StyleVar : int
		{
			/// <summary>
			/// float
			/// </summary>
			Alpha,

			/// <summary>
			/// float 
			/// </summary>
			DisabledAlpha,

			/// <summary>
			/// Vector2
			/// </summary>
			WindowPadding,

			/// <summary>
			/// float 
			/// </summary>
			WindowRounding,

			/// <summary>
			/// float 
			/// </summary>
			WindowBorderSize,

			/// <summary>
			/// Vector2
			/// </summary>
			WindowMinSize,

			/// <summary>
			/// Vector2
			/// </summary>
			WindowTitleAlign,

			/// <summary>
			/// float 
			/// </summary>
			ChildRounding,

			/// <summary>
			/// float 
			/// </summary>
			ChildBorderSize,

			/// <summary>
			/// float 
			/// </summary>
			PopupRounding,

			/// <summary>
			/// float 
			/// </summary>
			PopupBorderSize,

			/// <summary>
			/// Vector2
			/// </summary>
			FramePadding,

			/// <summary>
			/// float 
			/// </summary>
			FrameRounding,

			/// <summary>
			/// float 
			/// </summary>
			FrameBorderSize,

			/// <summary>
			/// Vector2
			/// </summary>
			ItemSpacing,

			/// <summary>
			/// Vector2
			/// </summary>
			ItemInnerSpacing,

			/// <summary>
			/// float 
			/// </summary>
			IndentSpacing,

			/// <summary>
			/// Vector2
			/// </summary>
			CellPadding,

			/// <summary>
			/// float     
			/// </summary>
			ScrollbarSize,

			/// <summary>
			/// float     
			/// </summary>
			ScrollbarRounding,

			/// <summary>
			/// float     
			/// </summary>
			GrabMinSize,

			/// <summary>
			/// float     
			/// </summary>
			GrabRounding,

			/// <summary>
			/// float     
			/// </summary>
			TabRounding,

			/// <summary>
			/// Vector2
			/// </summary>
			ButtonTextAlign,

			/// <summary>
			/// Vector2
			/// </summary>
			SelectableTextAlign,

			/// <summary>
			/// float
			/// </summary>
			SeparatorTextBorderSize,

			/// <summary>
			/// Vector2
			/// </summary>
			SeparatorTextAlign,  

			/// <summary>
			/// Vector2
			/// </summary>
			SeparatorTextPadding,
		};

		#region Window
		public static void Begin(string label, WindowFlags flags = WindowFlags.None) => _Begin(label, (int)flags);
		public static bool Begin(string label, ref bool open, WindowFlags flags = WindowFlags.None) => _BeginClosable(label, ref open, (int)flags);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Begin(string label, int flags);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _BeginClosable(string label, ref bool isOpen, int flags);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void End();

		public static bool BeginChild(string id, bool border = false, WindowFlags flags = WindowFlags.None)
			=> BeginChild(id, Vector2.Zero, border, flags);

		public static bool BeginChild(string id, Vector2 size, bool border = false, WindowFlags flags = WindowFlags.None)
			=> _BeginChild(id, ref size, border, (int)flags);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _BeginChild(string id, ref Vector2 size, bool border, int flags);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void EndChild();

		public static void SetNextWindowPos(Vector2 position)
			=> _SetNextWindowPos(ref position);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _SetNextWindowPos(ref Vector2 position);

		public static void SetNextWindowSize(Vector2 position)
			=> _SetNextWindowSize(ref position);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _SetNextWindowSize(ref Vector2 position);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void SetNextWindowViewport(uint viewportID);
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

		public static void DockSpace(string id) => DockSpace(id, Vector2.Zero);
		public static void DockSpace(string id, Vector2 size) => _DockSpace(id, ref size);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _DockSpace(string id, ref Vector2 size);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool Foldout(string label, bool openByDefault = false);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Indent(float width = 0.0f);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Unindent(float width = 0.0f);
		#endregion

		#region Controls
		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void SameLine();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Space();

		public static void Text(string label) => _Text(label);
		public static void Text(string label, Colour colour) => _TextColoured(label, ref colour);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Text(string label);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _TextColoured(string label, ref Colour colour);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool Button(string label);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool Checkbox(string label, ref bool checkState);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool ColourEdit3(string label, ref Colour colour);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool ColourEdit4(string label, ref Colour colour);

		public static void PlotLines(string label, float[] points, string overlayText = "")
			=> PlotLines(label, points, overlayText, Vector2.Zero);

		public static void PlotLines(string label, float[] points, string overlayText, Vector2 size)
			=> _PlotLines(label, points, overlayText, ref size);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _PlotLines(string label, float[] points, string overlayText, ref Vector2 size);
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

		public static bool Drag(string label, ref Vector4 value, float speed = 1.0f, float min = 0.0f, float max = 0.0f, string format = "%.3f")
			=> _DragFloat4(label, ref value, speed, min, max, format);
		
		public static bool Drag(string label, ref int value, float speed = 1, int min = 0, int max = 0, string format = "%i")
			=> _DragInt(label, ref value, speed, min, max, format);

		public static bool Drag(string label, ref IVector2 value, float speed = 1, int min = 0, int max = 0, string format = "%i")
			=> _DragInt2(label, ref value, speed, min, max, format);

		public static bool Drag(string label, ref IVector3 value, float speed = 1, int min = 0, int max = 0, string format = "%i")
			=> _DragInt3(label, ref value, speed, min, max, format);

		/// <summary>
		/// Shows drag prompt for Quaternion, in euler angles
		/// </summary>
		/// <returns>True if a value was changed this frame</returns>
		public static bool Drag(string label, ref Quaternion value, float speed = 1, int min = 0, int max = 0)
		{
			Vector3 euler = value.Euler;
			if(_DragFloat3(label, ref euler, speed, min, max, "%.2f"))
			{
				value.Euler = euler;
				return true;
			}
			return false;
		}

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
		private static extern bool _DragFloat4(
			string label,
			ref Vector4 value,
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

		#region Viewport
		public static bool IsWindowFocused => _IsWindowFocused();
		public static bool IsWindowHovered => _IsWindowHovered();
		public static bool IsWindowCollapsed => _IsWindowCollapsed();

		public static Vector2 WindowContentRegionMin
		{ get { _GetWindowContentRegionMin(out Vector2 output); return output; } }

		public static Vector2 WindowContentRegionMax
		{ get { _GetWindowContentRegionMax(out Vector2 output); return output; } }

		public static Vector2 ContentRegionAvailable
		{ get { _GetContentRegionAvail(out Vector2 output); return output; } }

		public static Vector2 WindowPosition
		{ get { _GetWindowPosition(out Vector2 output); return output; } }

		public static Viewport GetViewport(uint ID)
		{
			Viewport viewport = new Viewport() { ID = ID };
			_GetViewport(
				viewport.ID,
				out viewport.Position,
				out viewport.Size,
				out viewport.WorkPosition,
				out viewport.WorkSize,
				out viewport.ParentID
			);
			return viewport;
		}

		public static Viewport GetMainViewport()
			=> GetViewport(GetMainViewportID());

		public static Viewport GetWindowViewport()
			=> GetViewport(GetWindowViewportID());

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _GetViewport(
			uint ID,
			out Vector2 position,
			out Vector2 size,
			out Vector2 workPosition,
			out Vector2 workSize,
			out uint parentID
			);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern uint GetMainViewportID();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern uint GetWindowViewportID();

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _GetWindowContentRegionMin(out Vector2 output);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _GetWindowContentRegionMax(out Vector2 output);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _GetContentRegionAvail(out Vector2 output);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _GetWindowPosition(out Vector2 output);


		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool _IsWindowFocused();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool _IsWindowHovered();


		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _IsWindowCollapsed();
		#endregion

		#region Style
		public static void PushStyleVar(StyleVar style, float value)
			=> _PushStyleVar((int)style, value);

		public static void PushStyleVar(StyleVar style, Vector2 value)
			=> _PushStyleVarV((int)style, ref value);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _PushStyleVar(int flag, float value);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _PushStyleVarV(int flag, ref Vector2 value);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void PopStyleVar(int amount = 1);

		/// <summary>
		/// Height of a line of text, in pixels
		/// </summary>
		public static float TextLineHeight => _GetTextLineHeight();

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern float _GetTextLineHeight();
		#endregion

		#region Menu
		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool BeginMenuBar();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void EndMenuBar();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool BeginMenu(string label, bool enabled = true);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void EndMenu();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool MenuItem(string label, string shortcut = "", bool enabled = true);
		#endregion
	}
}