using AquaEngine;
using AquaEngine.Graphics;
using System;
using System.Runtime.CompilerServices;

namespace AquaEditor
{
	public static class ImGUI
	{
		public struct Viewport
		{
			public ulong ID, ParentID;
			public Vector2 Position, Size, WorkPosition, WorkSize;
		}

		[Flags]
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

		[Flags]
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

		public enum StyleColour : int
		{
			Text,
			TextDisabled,
			WindowBg,              // Background of normal windows
			ChildBg,               // Background of child windows
			PopupBg,               // Background of popups, menus, tooltips windows
			Border,
			BorderShadow,
			FrameBg,               // Background of checkbox, radio button, plot, slider, text input
			FrameBgHovered,
			FrameBgActive,
			TitleBg,
			TitleBgActive,
			TitleBgCollapsed,
			MenuBarBg,
			ScrollbarBg,
			ScrollbarGrab,
			ScrollbarGrabHovered,
			ScrollbarGrabActive,
			CheckMark,
			SliderGrab,
			SliderGrabActive,
			Button,
			ButtonHovered,
			ButtonActive,
			Header,                // Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
			HeaderHovered,
			HeaderActive,
			Separator,
			SeparatorHovered,
			SeparatorActive,
			ResizeGrip,            // Resize grip in lower-right and lower-left corners of windows.
			ResizeGripHovered,
			ResizeGripActive,
			Tab,                   // TabItem in a TabBar
			TabHovered,
			TabActive,
			TabUnfocused,
			TabUnfocusedActive,
			DockingPreview,        // Preview overlay color when about to docking something
			DockingEmptyBg,        // Background color for empty node (e.g. CentralNode with no window docked into it)
			PlotLines,
			PlotLinesHovered,
			PlotHistogram,
			PlotHistogramHovered,
			TableHeaderBg,         // Table header background
			TableBorderStrong,     // Table outer and header borders (prefer using Alpha=1.0 here)
			TableBorderLight,      // Table inner borders (prefer using Alpha=1.0 here)
			TableRowBg,            // Table row background (even rows)
			TableRowBgAlt,         // Table row background (odd rows)
			TextSelectedBg,
			DragDropTarget,        // Rectangle highlighting a drop target
			NavHighlight,          // Gamepad/keyboard: current highlighted item
			NavWindowingHighlight, // Highlight window when using CTRL+TAB
			NavWindowingDimBg,     // Darken/colorize entire screen behind the CTRL+TAB window list, when active
			ModalWindowDimBg,      // Darken/colorize entire screen behind a modal window, when one is active
		}

		[Flags]
		public enum TableFlags : int
		{
			// Features

			None = 0,

			/// <summary>
			/// Enable resizing columns.
			/// </summary>
			Resizable = 1 << 0,

			/// <summary>
			/// Enable reordering columns in header row (need calling TableSetupColumn() + TableHeadersRow() to display headers)
			/// </summary>
			Reorderable = 1 << 1,

			/// <summary>
			/// Enable hiding/disabling columns in context menu.
			/// </summary>
			Hideable = 1 << 2,

			/// <summary>
			/// Enable sorting. Call TableGetSortSpecs() to obtain sort specs. Also see SortMulti and SortTristate.
			/// </summary>
			Sortable = 1 << 3,

			/// <summary>
			/// Disable persisting columns order, width and sort settings in the .ini file.
			/// </summary>
			NoSavedSettings = 1 << 4,

			/// <summary>
			/// Right-click on columns body/contents will display table context menu. By default it is available in TableHeadersRow().
			/// </summary>
			ContextMenuInBody = 1 << 5,

			// Decorations

			/// <summary>
			/// Set each RowBg color with ImGuiCol_TableRowBg or ImGuiCol_TableRowBgAlt (equivalent of calling TableSetBgColor with ImGuiTableBgFlags_RowBg0 on each row manually)
			/// </summary>
			RowBg = 1 << 6,

			/// <summary>
			/// Draw horizontal borders between rows.
			/// </summary>
			BordersInnerH = 1 << 7,

			/// <summary>
			/// Draw horizontal borders at the top and bottom.
			/// </summary>
			BordersOuterH = 1 << 8,

			/// <summary>
			/// Draw vertical borders between columns.
			/// </summary>
			BordersInnerV = 1 << 9,

			/// <summary>
			/// Draw vertical borders on the left and right sides.
			/// </summary>
			BordersOuterV = 1 << 10,

			/// <summary>
			/// Draw horizontal borders.
			/// </summary>
			BordersH = BordersInnerH | BordersOuterH,

			/// <summary>
			/// Draw vertical borders.
			/// </summary>
			BordersV = BordersInnerV | BordersOuterV,

			/// <summary>
			/// Draw inner borders.
			/// </summary>
			BordersInner = BordersInnerV | BordersInnerH,

			/// <summary>
			/// Draw outer borders.
			/// </summary>
			BordersOuter = BordersOuterV | BordersOuterH,

			/// <summary>
			/// Draw all borders.
			/// </summary>
			Borders = BordersInner | BordersOuter,

			/// <summary>
			/// [ALPHA] Disable vertical borders in columns Body (borders will always appear in Headers). -> May move to style
			/// </summary>
			NoBordersInBody = 1 << 11,

			/// <summary>
			/// [ALPHA] Disable vertical borders in columns Body until hovered for resize (borders will always appear in Headers). -> May move to style
			/// </summary>
			NoBordersInBodyUntilResize = 1 << 12,

			// Sizing Policy (read above for defaults)

			/// <summary>
			/// Columns default to _WidthFixed or _WidthAuto (if resizable or not resizable), matching contents width.
			/// </summary>
			SizingFixedFit = 1 << 13,

			/// <summary>
			/// Columns default to _WidthFixed or _WidthAuto (if resizable or not resizable), matching the maximum contents width of all columns. Implicitly enable NoKeepColumnsVisible.
			/// </summary>
			SizingFixedSame = 2 << 13,

			/// <summary>
			/// Columns default to _WidthStretch with default weights proportional to each columns contents widths.
			/// </summary>
			SizingStretchProp = 3 << 13,

			/// <summary>
			/// Columns default to _WidthStretch with default weights all equal, unless overridden by TableSetupColumn().
			/// </summary>
			SizingStretchSame = 4 << 13,

			// Sizing Extra Options

			/// <summary>
			/// Make outer width auto-fit to columns, overriding outer_size.x value. Only available when ScrollX/ScrollY are disabled and Stretch columns are not used.
			/// </summary>
			NoHostExtendX = 1 << 16,

			/// <summary>
			/// Make outer height stop exactly at outer_size.y (prevent auto-extending table past the limit). Only available when ScrollX/ScrollY are disabled. Data below the limit will be clipped and not visible.
			/// </summary>
			NoHostExtendY = 1 << 17,

			/// <summary>
			/// Disable keeping column always minimally visible when ScrollX is off and table gets too small. Not recommended if columns are resizable.
			/// </summary>
			NoKeepColumnsVisible = 1 << 18,

			/// <summary>
			/// Disable distributing remainder width to stretched columns (width allocation on a 100-wide table with 3 columns: Without this flag: 33,33,34. With this flag: 33,33,33). With larger number of columns, resizing will appear to be less smooth.
			/// </summary>
			PreciseWidths = 1 << 19,

			// Clipping

			/// <summary>
			/// Disable clipping rectangle for every individual columns (reduce draw command count, items will be able to overflow into other columns). Generally incompatible with TableSetupScrollFreeze().
			/// </summary>
			NoClip = 1 << 20,

			// Padding

			/// <summary>
			/// Default if BordersOuterV is on. Enable outermost padding. Generally desirable if you have headers.
			/// </summary>
			PadOuterX = 1 << 21,

			/// <summary>
			/// Default if BordersOuterV is off. Disable outermost padding.
			/// </summary>
			NoPadOuterX = 1 << 22,

			/// <summary>
			/// Disable inner padding between columns (double inner padding if BordersOuterV is on, single inner padding if BordersOuterV is off).
			/// </summary>
			NoPadInnerX = 1 << 23,

			// Scrolling

			/// <summary>
			/// Enable horizontal scrolling. Require 'outer_size' parameter of BeginTable() to specify the container size. Changes default sizing policy. Because this creates a child window, ScrollY is currently generally recommended when using ScrollX.
			/// </summary>
			ScrollX = 1 << 24,

			/// <summary>
			/// Enable vertical scrolling. Require 'outer_size' parameter of BeginTable() to specify the container size.
			/// </summary>
			ScrollY = 1 << 25,

			// Sorting

			/// <summary>
			/// Hold shift when clicking headers to sort on multiple column. TableGetSortSpecs() may return specs where (SpecsCount > 1).
			/// </summary>
			SortMulti = 1 << 26,

			/// <summary>
			/// Allow no sorting, disable default sorting. TableGetSortSpecs() may return specs where (SpecsCount == 0).
			/// </summary>
			SortTristate = 1 << 27,
		};

		[Flags]
		public enum TableColumnFlags : int
		{
			// Input configuration flags
			None = 0,

			/// <summary>
			/// Overriding/master disable flag: hide column, won't show in context menu (unlike calling TableSetColumnEnabled() which manipulates the user accessible state)
			/// </summary>
			Disabled = 1 << 0,

			/// <summary>
			/// Default as a hidden/disabled column.
			/// </summary>
			DefaultHide = 1 << 1,

			/// <summary>
			/// Default as a sorting column.
			/// </summary>
			DefaultSort = 1 << 2,

			/// <summary>
			/// Column will stretch. Preferable with horizontal scrolling disabled (default if table sizing policy is _SizingStretchSame or _SizingStretchProp).
			/// </summary>
			WidthStretch = 1 << 3,

			/// <summary>
			/// Column will not stretch. Preferable with horizontal scrolling enabled (default if table sizing policy is _SizingFixedFit and table is resizable).
			/// </summary>
			WidthFixed = 1 << 4,

			/// <summary>
			/// Disable manual resizing.
			/// </summary>
			NoResize = 1 << 5,

			/// <summary>
			/// Disable manual reordering this column, this will also prevent other columns from crossing over this column.
			/// </summary>
			NoReorder = 1 << 6,

			/// <summary>
			/// Disable ability to hide/disable this column.
			/// </summary>
			NoHide = 1 << 7,

			/// <summary>
			/// Disable clipping for this column (all NoClip columns will render in a same draw command).
			/// </summary>
			NoClip = 1 << 8,

			/// <summary>
			/// Disable ability to sort on this field (even if ImGuiTableFlags_Sortable is set on the table).
			/// </summary>
			NoSort = 1 << 9,

			/// <summary>
			/// Disable ability to sort in the ascending direction.
			/// </summary>
			NoSortAscending = 1 << 10,

			/// <summary>
			/// Disable ability to sort in the descending direction.
			/// </summary>
			NoSortDescending = 1 << 11,

			/// <summary>
			/// TableHeadersRow() will not submit label for this column. Convenient for some small columns. Name will still appear in context menu.
			/// </summary>
			NoHeaderLabel = 1 << 12,

			/// <summary>
			/// Disable header text width contribution to automatic column width.
			/// </summary>
			NoHeaderWidth = 1 << 13,

			/// <summary>
			/// Make the initial sort direction Ascending when first sorting on this column (default).
			/// </summary>
			PreferSortAscending = 1 << 14,

			/// <summary>
			/// Make the initial sort direction Descending when first sorting on this column.
			/// </summary>
			PreferSortDescending = 1 << 15,

			/// <summary>
			/// Use current Indent value when entering cell (default for column 0).
			/// </summary>
			IndentEnable = 1 << 16,

			/// <summary>
			/// Ignore current Indent value when entering cell (default for columns > 0). Indentation changes _within_ the cell will still be honored.
			/// </summary>
			IndentDisable = 1 << 17,

			// Output status flags, read-only via TableGetColumnFlags()

			/// <summary>
			/// Status: is enabled == not hidden by user/api (referred to as "Hide" in _DefaultHide and _NoHide) flags.
			/// </summary>
			IsEnabled = 1 << 24,  

			/// <summary>
			/// Status: is visible == is enabled AND not clipped by scrolling.
			/// </summary>
			IsVisible = 1 << 25,  

			/// <summary>
			/// Status: is currently part of the sort specs
			/// </summary>
			IsSorted = 1 << 26,   

			/// <summary>
			/// Status: is hovered by mouse
			/// </summary>
			IsHovered = 1 << 27,  


		}

		[Flags]
		public enum SelectableFlags : int
		{
			None = 0,

			/// <summary>
			/// Clicking this doesn't close parent popup window
			/// </summary>
			DontClosePopups		= 1 << 0,

			/// <summary>
			/// Selectable frame can span all columns (text will still fit in current column)
			/// </summary>
			SpanAllColumns		= 1 << 1,

			/// <summary>
			/// Generate press events on double clicks too
			/// </summary>
			AllowDoubleClick	= 1 << 2,

			/// <summary>
			/// Cannot be selected, display grayed out text
			/// </summary>
			Disabled			= 1 << 3,

			/// <summary>
			/// (WIP) Hit testing to allow subsequent widgets to overlap this one
			/// </summary>
			AllowItemOverlap	= 1 << 4
		};

		[Flags]
		public enum PopupFlags : int
		{
			None = 0,

			/// <summary>
			/// For BeginPopupContext*(): open on Left Mouse release. Guaranteed to always be == 0 (same as ImGuiMouseButton_Left)
			/// </summary>
			MouseButtonLeft			= 0,

			/// <summary>
			/// For BeginPopupContext*(): open on Right Mouse release. Guaranteed to always be == 1 (same as ImGuiMouseButton_Right)
			/// </summary>
			MouseButtonRight		= 1,

			/// <summary>
			/// For BeginPopupContext*(): open on Middle Mouse release. Guaranteed to always be == 2 (same as ImGuiMouseButton_Middle)
			/// </summary>
			MouseButtonMiddle		= 2,

			/// <summary>
			/// For OpenPopup*(), BeginPopupContext*(): don't open if there's already a popup at the same level of the popup stack
			/// </summary>
			NoOpenOverExistingPopup = 1 << 5,

			/// <summary>
			/// For BeginPopupContextWindow(): don't return true when hovering items, only when hovering empty space
			/// </summary>
			NoOpenOverItems			= 1 << 6,

			/// <summary>
			/// For IsPopupOpen(): ignore the ImGuiID parameter and test for any popup.
			/// </summary>
			AnyPopupId				= 1 << 7,

			/// <summary>
			/// For IsPopupOpen(): search/test at any level of the popup stack (default test in the current level)
			/// </summary>
			AnyPopupLevel			= 1 << 8,

			AnyPopup = AnyPopupId | AnyPopupLevel
		};

		public enum MouseCursor : int
		{
			None = -1,

			Arrow = 0,

			/// <summary>
			/// When hovering over InputText, etc.
			/// </summary>
			TextInput,         

			/// <summary>
			/// (Unused by Dear ImGui functions)
			/// </summary>
			ResizeAll,         

			/// <summary>
			/// When hovering over a horizontal border
			/// </summary>
			ResizeNS,

			/// <summary>
			/// When hovering over a vertical border or a column
			/// </summary>
			ResizeEW,

			/// <summary>
			/// When hovering over the bottom-left corner of a window
			/// </summary>
			ResizeNESW,

			/// <summary>
			/// When hovering over the bottom-right corner of a window
			/// </summary>
			ResizeNWSE,

			/// <summary>
			/// (Unused by Dear ImGui functions. Use for e.g. hyperlinks)
			/// </summary>
			Hand,

			/// <summary>
			/// When hovering something with disallowed interaction. Usually a crossed circle.
			/// </summary>
			NotAllowed
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
		public static extern void SetNextWindowViewport(ulong viewportID);
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

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Separator();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void PushItemWidth(float width);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void PopItemWidth();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void SetNextItemWidth(float width);

		public static void SetMouseCursor(MouseCursor cursor) => _SetMouseCursor((int)cursor);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetMouseCursor(int type);

		public static MouseCursor GetMouseCursor() => (MouseCursor)_GetMouseCursor();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern int _GetMouseCursor();

		public static Vector2 GetCursorPos()
		{
			_GetCursorPos(out Vector2 pos);
			return pos;
		}
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetCursorPos(out Vector2 value);

		public static void SetCursorPos(Vector2 value) => _SetCursorPos(ref value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetCursorPos(ref Vector2 value);

		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void SetCursorPosX(float value);
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void SetCursorPosY(float value);

		[MethodImpl(MethodImplOptions.InternalCall)] public static extern float GetCursorPosX();
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern float GetCursorPosY();
		
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void BeginGroup();
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void EndGroup();
		#endregion

		#region Controls
		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void SameLine(float offset = 0, float spacing = -1);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Space();

		public static void Text(string label) => _Text(label);
		public static void Text(string label, Colour colour) => _TextColoured(label, ref colour);

		public static void Text(string label, Texture icon) => Text(label, icon, Colour.White);
		public static void Text(string label, Texture icon, Colour colour, bool tintIcon = true)
		{
			float size = TextLineHeight * 1.25f;
			Image(icon, new Vector2(size, size), tintIcon ? colour : Colour.White);
			SameLine();
			Text(label, colour);
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Text(string label);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _TextColoured(string label, ref Colour colour);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool Button(string label);

		public static bool Selectable(string label, bool selected = false, SelectableFlags flags = SelectableFlags.None) =>
			Selectable(label, Vector2.Zero, selected, flags);
		public static bool Selectable(string label, Vector2 size, bool selected = false, SelectableFlags flags = SelectableFlags.None) =>
			_Selectable(label, selected, (int)flags, ref size);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _Selectable(string label, bool selected, int flags, ref Vector2 size);

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

		public static void Image(ulong textureID, Vector2 size) => _Image(textureID, ref size, ref s_ImageDefaultColour, ref s_ImageDefaultBorder);
		public static void Image(ulong textureID, Vector2 size, Colour tint) => _Image(textureID, ref size, ref tint, ref s_ImageDefaultBorder);
		public static void Image(ulong textureID, Vector2 size, Colour tint, Colour border) => _Image(textureID, ref size, ref tint, ref border);

		public static void Image(Texture texture, Vector2 size) => _Image(texture?.ResourceID ?? 0, ref size, ref s_ImageDefaultColour, ref s_ImageDefaultBorder);
		public static void Image(Texture texture, Vector2 size, Colour tint) => _Image(texture?.ResourceID ?? 0, ref size, ref tint, ref s_ImageDefaultBorder);
		public static void Image(Texture texture, Vector2 size, Colour tint, Colour border) => _Image(texture?.ResourceID ?? 0, ref size, ref tint, ref border);

		public static void Image(RenderTexture texture, Vector2 size) => _ImageRenderTexture(texture?.Handle ?? IntPtr.Zero, ref size, ref s_ImageDefaultColour, ref s_ImageDefaultBorder);
		public static void Image(RenderTexture texture, Vector2 size, Colour tint) => _ImageRenderTexture(texture?.Handle ?? IntPtr.Zero, ref size, ref tint, ref s_ImageDefaultBorder);
		public static void Image(RenderTexture texture, Vector2 size, Colour tint, Colour border) => _ImageRenderTexture(texture?.Handle ?? IntPtr.Zero, ref size, ref tint, ref border);

		public static bool ButtonImage(ulong textureID, Vector2 size, int framePadding = -1) => _ImageButton(textureID, ref size, framePadding, ref s_ImageDefaultColour, ref s_ButtonImageDefaultBackground);
		public static bool ButtonImage(ulong textureID, Vector2 size, Colour tint, int framePadding = -1) => _ImageButton(textureID, ref size, framePadding, ref tint, ref s_ButtonImageDefaultBackground);
		public static bool ButtonImage(ulong textureID, Vector2 size, Colour tint, Colour backgroundColour, int framePadding = -1) => _ImageButton(textureID, ref size, framePadding, ref tint, ref backgroundColour);

		public static bool ButtonImage(Texture texture, Vector2 size, int framePadding = -1) => _ImageButton(texture?.ResourceID ?? 0, ref size, framePadding, ref s_ImageDefaultColour, ref s_ButtonImageDefaultBackground);
		public static bool ButtonImage(Texture texture, Vector2 size, Colour tint, int framePadding = -1) => _ImageButton(texture?.ResourceID ?? 0, ref size, framePadding, ref tint, ref s_ButtonImageDefaultBackground);
		public static bool ButtonImage(Texture texture, Vector2 size, Colour tint, Colour backgroundColour, int framePadding = -1) => _ImageButton(texture?.ResourceID ?? 0, ref size, framePadding, ref tint, ref backgroundColour);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Image(ulong textureID, ref Vector2 size, ref Colour tint, ref Colour borderTint);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _ImageRenderTexture(IntPtr handle, ref Vector2 size, ref Colour tint, ref Colour borderTint);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _ImageButton(ulong textureID, ref Vector2 size, int framePadding, ref Colour tint, ref Colour backgroundColour);
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
			if (_DragFloat3(label, ref euler, speed, min, max, "%.2f"))
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

		#region Popups
		public static bool BeginPopup(string id, WindowFlags flags = WindowFlags.None) =>
			_BeginPopup(id, (int)flags);

		public static bool BeginPopupContextItem(string id, PopupFlags flags = PopupFlags.None) =>
			_BeginPopupContextItem(id, (int)flags);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _BeginPopupContextItem(string id, int flags);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _BeginPopup(string id, int flags);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void EndPopup();
		#endregion

		#region Viewport
		public static bool IsWindowFocused => _IsWindowFocused();
		public static bool IsWindowHovered => _IsWindowHovered();
		public static bool IsWindowCollapsed => _IsWindowCollapsed();

		public static IVector2 WindowContentRegionMin
		{ get { _GetWindowContentRegionMin(out IVector2 output); return output; } }

		public static IVector2 WindowContentRegionMax
		{ get { _GetWindowContentRegionMax(out IVector2 output); return output; } }

		public static IVector2 ContentRegionAvailable
		{ get { _GetContentRegionAvail(out IVector2 output); return output; } }

		public static IVector2 WindowPosition
		{ get { _GetWindowPosition(out IVector2 output); return output; } }

		public static Viewport GetViewport(ulong ID)
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
			ulong ID,
			out Vector2 position,
			out Vector2 size,
			out Vector2 workPosition,
			out Vector2 workSize,
			out ulong parentID
			);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern ulong GetMainViewportID();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern ulong GetWindowViewportID();

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _GetWindowContentRegionMin(out IVector2 output);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _GetWindowContentRegionMax(out IVector2 output);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _GetContentRegionAvail(out IVector2 output);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _GetWindowPosition(out IVector2 output);


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

		public static void PushStyleColour(StyleColour style, Colour value)
			=> _PushStyleColour((int)style, ref value);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _PushStyleVar(int flag, float value);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _PushStyleVarV(int flag, ref Vector2 value);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _PushStyleColour(int flag, ref Colour value);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void PopStyleVar(int amount = 1);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void PopStyleColour(int amount = 1);

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

		#region Tables
		public static bool BeginTable(string label, ulong columns, TableFlags flags = TableFlags.None, float innerWidth = 0) =>
			BeginTable(label, columns, Vector2.Zero, flags, innerWidth);

		public static bool BeginTable(string label, ulong columns, Vector2 outerSize, TableFlags flags = TableFlags.None, float innerWidth = 0) =>
			_BeginTable(label, columns, (int)flags, ref outerSize, innerWidth);

		public static void TableSetupColumn(string label, TableColumnFlags flags = TableColumnFlags.None, float size = 0) =>
			_TableSetupColumn(label, (int)flags, size);

		/// <returns>
		/// Column flags so you can query their Enabled/Visible/Sorted/Hovered status flags. Pass -1 to use current column.
		/// </returns>
		public static TableColumnFlags TableGetColumnFlags(int column = -1) =>
			(TableColumnFlags)_TableGetColumnFlags(column);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _BeginTable(string label, ulong columns, int flags, ref Vector2 outerSize, float innerWidth);

		/// <summary>
		/// Only call if <b>BeginTable</b> returns true
		/// </summary>
		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void EndTable();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void TableNextRow(float minHeight = 0.0f, bool isHeader = false);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void TableNextColumn();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void TableHeadersRow();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void TableSetColumnEnabled(int column, bool enabled);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void TableSetColumnIndex(int column);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _TableSetupColumn(string label, int flags, float size);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int _TableGetColumnFlags(int column);
		#endregion

		#region Columns
		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Columns(int count, string id = "", bool border = true);

		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void NextColumn();
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void SetColumnWidth(int column, float width);
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void SetColumnOffset(int column, float offset);
		#endregion
	}
}