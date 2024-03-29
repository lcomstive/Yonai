using Yonai;
using Yonai.Graphics;
using Newtonsoft.Json;
using System;
using System.Runtime.CompilerServices;

namespace YonaiEditor
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

		[Flags]
		public enum ComboFlags : int
		{
			None = 0,

			/// <summary>
			/// Align the popup toward the left by default
			/// </summary>
			PopupAlignLeft = 1 << 0,
			
			/// <summary>
			/// Max ~4 items visible. Tip: If you want your combo popup to be a specific size you can use SetNextWindowSizeConstraints() prior to calling BeginCombo()
			/// </summary>
			HeightSmall = 1 << 1,

			/// <summary>
			/// Max ~8 items visible (default)
			/// </summary>
			HeightRegular = 1 << 2,

			/// <summary>
			/// Max ~20 items visible
			/// </summary>
			HeightLarge = 1 << 3,
			/// <summary>
			/// As many fitting items as possible
			/// </summary>
			HeightLargest = 1 << 4,
			/// <summary>
			/// Display on the preview box without the square arrow button
			/// </summary>
			NoArrowButton = 1 << 5,

			/// <summary>
			/// Display only a square arrow button
			/// </summary>
			NoPreview = 1 << 6
		};

		[Flags]
		public enum DrawFlags : int
		{
			None = 0,
			Closed = 1 << 0, // PathStroke(), AddPolyline(): specify that shape should be closed (Important: this is always == 1 for legacy reason)
			RoundCornersTopLeft = 1 << 4, // AddRect(), AddRectFilled(), PathRect(): enable rounding top-left corner only (when rounding > 0.0f, we default to all corners). Was 0x01.
			RoundCornersTopRight = 1 << 5, // AddRect(), AddRectFilled(), PathRect(): enable rounding top-right corner only (when rounding > 0.0f, we default to all corners). Was 0x02.
			RoundCornersBottomLeft = 1 << 6, // AddRect(), AddRectFilled(), PathRect(): enable rounding bottom-left corner only (when rounding > 0.0f, we default to all corners). Was 0x04.
			RoundCornersBottomRight = 1 << 7, // AddRect(), AddRectFilled(), PathRect(): enable rounding bottom-right corner only (when rounding > 0.0f, we default to all corners). Wax 0x08.
			RoundCornersNone = 1 << 8, // AddRect(), AddRectFilled(), PathRect(): disable rounding on all corners (when rounding > 0.0f). This is NOT zero, NOT an implicit flag!
			RoundCornersTop = RoundCornersTopLeft | RoundCornersTopRight,
			RoundCornersBottom = RoundCornersBottomLeft | RoundCornersBottomRight,
			RoundCornersLeft = RoundCornersBottomLeft | RoundCornersTopLeft,
			RoundCornersRight = RoundCornersBottomRight | RoundCornersTopRight,
			RoundCornersAll = RoundCornersTopLeft | RoundCornersTopRight | RoundCornersBottomLeft | RoundCornersBottomRight,
			RoundCornersDefault_ = RoundCornersAll, // Default to ALL corners if none of the _RoundCornersXX flags are specified.
			RoundCornersMask_ = RoundCornersAll | RoundCornersNone,
		};

		[Flags]
		public enum DragDropFlags
		{
			None = 0,

			// BeginDragDropSource() flags //

			/// <summary>
			/// Disable preview tooltip. By default, a successful call to BeginDragDropSource opens a tooltip so you can display a preview or description of the source contents. This flag disables this behavior.
			/// </summary>
			SourceNoPreviewTooltip = 1 << 0,  

			/// <summary>
			/// By default, when dragging we clear data so that IsItemHovered() will return false, to avoid subsequent user code submitting tooltips. This flag disables this behavior so you can still call IsItemHovered() on the source item.
			/// </summary>
			SourceNoDisableHover = 1 << 1,  

			/// <summary>
			/// Disable the behavior that allows to open tree nodes and collapsing header by holding over them while dragging a source item.
			/// </summary>
			SourceNoHoldToOpenOthers = 1 << 2,  

			/// <summary>
			/// Allow items such as Text(), Image() that have no unique identifier to be used as drag source, by manufacturing a temporary identifier based on their window-relative position. This is extremely unusual within the dear imgui ecosystem and so we made it explicit.
			/// </summary>
			SourceAllowNullID = 1 << 3,  

			/// <summary>
			/// External source (from outside of dear imgui), won't attempt to read current item/window info. Will always return true. Only one Extern source can be active simultaneously.
			/// </summary>
			SourceExtern = 1 << 4,  

			/// <summary>
			/// Automatically expire the payload if the source cease to be submitted (otherwise payloads are persisting while being dragged)
			/// </summary>
			SourceAutoExpirePayload = 1 << 5,  

			// AcceptDragDropPayload() flags //

			/// <summary>
			/// AcceptDragDropPayload() will returns true even before the mouse button is released. You can then call IsDelivery() to test if the payload needs to be delivered.
			/// </summary>
			AcceptBeforeDelivery = 1 << 10, 

			/// <summary>
			/// Do not draw the default highlight rectangle when hovering over target.
			/// </summary>
			AcceptNoDrawDefaultRect = 1 << 11, 

			/// <summary>
			/// Request hiding the BeginDragDropSource tooltip from the BeginDragDropTarget site.
			/// </summary>
			AcceptNoPreviewTooltip = 1 << 12, 

			/// <summary>
			/// For peeking ahead and inspecting the payload before delivery.
			/// </summary>
			AcceptPeekOnly = AcceptBeforeDelivery | AcceptNoDrawDefaultRect
		};

		[Flags]
		public enum ManipulateOperation : uint
		{
			TranslateX		= (1u << 0),
			TranslateY		= (1u << 1),
			TranslateZ		= (1u << 2),

			RotateX			= (1u << 3),
			RotateY			= (1u << 4),
			RotateZ			= (1u << 5),

			RotateScreen	= (1u << 6),

			ScaleX			= (1u << 7),
			ScaleY			= (1u << 8),
			ScaleZ			= (1u << 9),

			Bounds			= (1u << 10),

			ScaleXU			= (1u << 11),
			ScaleYU			= (1u << 12),
			ScaleZU			= (1u << 13),

			Translate		= TranslateX | TranslateY | TranslateZ,
			Rotate			= RotateX | RotateY | RotateZ | RotateScreen,
			Scale			= ScaleX | ScaleY | ScaleZ,
			ScaleU			= ScaleXU | ScaleYU | ScaleZU, // universal
			Universal		= Translate | Rotate | ScaleU
		};

		[Flags]
		public enum FocusedFlags : int
		{
			None				= 0,
			
			/// <summary>
			/// Return true if any children of the window is focused
			/// </summary>
			ChildWindows		= 1 << 0,
			
			/// <summary>
			/// Test from root window (top most parent of the current hierarchy)
			/// </summary>
			RootWindow			= 1 << 1,
			
			/// <summary>
			/// Return true if any window is focused. Important: If you are trying to tell how to dispatch your low-level inputs, do NOT use this. Use 'io.WantCaptureMouse' instead! Please read the FAQ!
			/// </summary>
			AnyWindow			= 1 << 2,
			
			/// <summary>
			/// Do not consider popup hierarchy (do not treat popup emitter as parent of popup) (when used with _ChildWindows or _RootWindow)
			/// </summary>
			NoPopupHierarchy	= 1 << 3,
			
			/// <summary>
			/// Consider docking hierarchy (treat dockspace host as parent of docked window) (when used with _ChildWindows or _RootWindow)
			/// </summary>
			DockHierarchy		= 1 << 4,
			RootAndChildWindows = RootWindow | ChildWindows,
		};

		[Flags]
		public enum HoveredFlags : int
		{
			/// <summary>
			/// Return true if directly over the item/window, not obstructed by another window, not obstructed by an active popup or modal blocking inputs under them.
			/// </summary>
			None						 = 0,     

			/// <summary>
			/// IsWindowHovered() only: Return true if any children of the window is hovered
			/// </summary>
			ChildWindows				 = 1 << 0,
			
			/// <summary>
			/// IsWindowHovered() only: Test from root window (top most parent of the current hierarchy)
			/// </summary>
			RootWindow					 = 1 << 1,
			
			/// <summary>
			/// IsWindowHovered() only: Return true if any window is hovered
			/// </summary>
			AnyWindow					 = 1 << 2,
			
			/// <summary>
			/// IsWindowHovered() only: Do not consider popup hierarchy (do not treat popup emitter as parent of popup) (when used with _ChildWindows or _RootWindow)
			/// </summary>
			NoPopupHierarchy			 = 1 << 3,
			
			/// <summary>
			/// IsWindowHovered() only: Consider docking hierarchy (treat dockspace host as parent of docked window) (when used with _ChildWindows or _RootWindow)
			/// </summary>
			DockHierarchy				 = 1 << 4,
			
			/// <summary>
			/// Return true even if a popup window is normally blocking access to this item/window
			/// </summary>
			AllowWhenBlockedByPopup		 = 1 << 5,

			/// <summary>
			/// Return true even if an active item is blocking access to this item/window. Useful for Drag and Drop patterns.
			/// </summary>
			AllowWhenBlockedByActiveItem = 1 << 7,   
			
			/// <summary>
			/// IsItemHovered() only: Return true even if the position is obstructed or overlapped by another window
			/// </summary>
			AllowWhenOverlapped			 = 1 << 8,   
			
			/// <summary>
			/// IsItemHovered() only: Return true even if the item is disabled
			/// </summary>
			AllowWhenDisabled			 = 1 << 9,   
			
			/// <summary>
			/// Disable using gamepad/keyboard navigation state when active, always query mouse.
			/// </summary>
			NoNavOverride				 = 1 << 10,  

			RectOnly = AllowWhenBlockedByPopup | AllowWhenBlockedByActiveItem | AllowWhenOverlapped,

			RootAndChildWindows			 = RootWindow | ChildWindows,

			// Hovering delays (for tooltips) //
			/// <summary>
			/// Return true after io.HoverDelayNormal elapsed (~0.30 sec)
			/// </summary>
			DelayNormal					 = 1 << 11,
			
			/// <summary>
			/// Return true after io.HoverDelayShort elapsed (~0.10 sec)
			/// </summary>
			DelayShort					 = 1 << 12,
			
			/// <summary>
			/// Disable shared delay system where moving from one item to the next keeps the previous timer for a short time (standard for tooltips with long delays)
			/// </summary>
			NoSharedDelay				 = 1 << 13,
		};

		[Flags]
		public enum TreeNodeFlags : int
		{
			None                 = 0,
			Selected             = 1 << 0,   // Draw as selected
			Framed               = 1 << 1,   // Draw frame with background (e.g. for CollapsingHeader)
			AllowOverlap         = 1 << 2,   // Hit testing to allow subsequent widgets to overlap this one
			NoTreePushOnOpen     = 1 << 3,   // Don't do a TreePush() when open (e.g. for CollapsingHeader) = no extra indent nor pushing on ID stack
			NoAutoOpenOnLog      = 1 << 4,   // Don't automatically and temporarily open node when Logging is active (by default logging will automatically open tree nodes)
			DefaultOpen          = 1 << 5,   // Default node to be open
			OpenOnDoubleClick    = 1 << 6,   // Need double-click to open node
			OpenOnArrow          = 1 << 7,   // Only open when clicking on the arrow part. If ImGuiTreeNodeFlags_OpenOnDoubleClick is also set, single-click arrow or double-click all box to open.
			Leaf                 = 1 << 8,   // No collapsing, no arrow (use as a convenience for leaf nodes).
			Bullet               = 1 << 9,   // Display a bullet instead of arrow
			FramePadding         = 1 << 10,  // Use FramePadding (even for an unframed text node) to vertically align text baseline to regular widget height. Equivalent to calling AlignTextToFramePadding().
			SpanAvailWidth       = 1 << 11,  // Extend hit box to the right-most edge, even if not framed. This is not the default in order to allow adding other items on the same line. In the future we may refactor the hit system to be front-to-back, allowing natural overlaps and then this can become the default.
			SpanFullWidth        = 1 << 12,  // Extend hit box to the left-most and right-most edges (bypass the indented area).
			NavLeftJumpsBackHere = 1 << 13,  // (WIP) Nav: left direction may move to this TreeNode() from any of its child (items submitted between TreeNode and TreePop)
			//NoScrollOnOpen     = 1 << 14,  // FIXME: TODO: Disable automatic scroll on TreePop() if node got just open and contents is not visible
			CollapsingHeader     = Framed | NoTreePushOnOpen | NoAutoOpenOnLog,
		}

		public enum Condition : int
		{
			None          = 0,        // No condition (always set the variable), same as _Always
			Always        = 1 << 0,   // No condition (always set the variable), same as _None
			Once          = 1 << 1,   // Set the variable once per runtime session (only the first call will succeed)
			FirstUseEver  = 1 << 2,   // Set the variable if the object/window has no persistently saved data (no entry in .ini file)
			Appearing     = 1 << 3,   // Set the variable if the object/window is appearing after being hidden/inactive (or the first time)
		};

		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void SetIniFilename(string path);

		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void SaveIniSettingsToDisk(string path);
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void LoadIniSettingsFromDisk(string path);

		public static void SetDisplayFramebufferScale(Vector2 scale) => SetDisplayFramebufferScale(scale.x, scale.y);
		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void SetDisplayFramebufferScale(float scaleX, float scaleY);

		#region Font
		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void SetFontGlobalScale(float scale);

		public static float FontSize => _GetFontSize();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern float _GetFontSize();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void AddFont(byte[] data, int fontSize = 18);
		#endregion

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

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void SetNextWindowFocus();

		public static void SetWindowFocus() => _SetWindowFocus();
		public static void SetWindowFocus(string windowTitle) => _SetWindowFocusNamed(windowTitle);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _SetWindowFocus();

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _SetWindowFocusNamed(string windowTitle);

		public static void SetNextWindowSize(Vector2 size)
			=> _SetNextWindowSize(ref size);

		public static void SetNextWindowSize(float width, float height)
			=> SetNextWindowSize(new Vector2(width, height));

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _SetNextWindowSize(ref Vector2 position);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void SetNextWindowViewport(ulong viewportID);

		public static bool SetNextItemOpen(bool value, Condition condition = Condition.None) =>
			_SetNextItemOpen(value, (int)condition);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _SetNextItemOpen(bool value, int condition);
		#endregion

		#region State
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern void SetCurrentContext();

		public static bool IsItemHovered(HoveredFlags flags = HoveredFlags.None) => _IsItemHovered((int)flags);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _IsItemHovered(int flags);

		[MethodImpl(MethodImplOptions.InternalCall)] public static extern bool IsItemClicked();
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern bool IsItemEdited();
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern bool IsItemActive();
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern bool IsAnyItemActive();

		[MethodImpl(MethodImplOptions.InternalCall)] public static extern bool IsItemActivated();
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern bool IsItemDeactivatedAfterEdit();

		public static bool IsMouseClicked(MouseButton button) => _IsMouseClicked((int)button);
		public static bool IsMouseDoubleClicked(MouseButton button) => _IsMouseDoubleClicked((int)button);
		public static bool IsMouseDragging(MouseButton button) => _IsMouseDragging((int)button);
		public static bool IsMouseReleased(MouseButton button) => _IsMouseReleased((int)button);

		public static bool IsAnyMouseClicked() =>
			IsMouseClicked(MouseButton.Left) || IsMouseClicked(MouseButton.Right) || IsMouseClicked(MouseButton.Middle);
		public static bool IsAnyMouseDoubleClicked() =>
			IsMouseDoubleClicked(MouseButton.Left) || IsMouseDoubleClicked(MouseButton.Right) || IsMouseDoubleClicked(MouseButton.Middle);
		public static bool IsAnyMouseDragging() =>
			IsMouseDragging(MouseButton.Left) || IsMouseDragging(MouseButton.Right) || IsMouseDragging(MouseButton.Middle);
		public static bool IsAnyMouseReleased() =>
			IsMouseReleased(MouseButton.Left) || IsMouseReleased(MouseButton.Right) || IsMouseReleased(MouseButton.Middle);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _IsMouseClicked(int button);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _IsMouseDoubleClicked(int button);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _IsMouseDragging(int button);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _IsMouseReleased(int button);

		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void BeginDisabled();
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void EndDisabled();
		
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void SetItemDefaultFocus();
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void SetKeyboardFocusHere(int offset = 0);

		/// <returns>True if <paramref name="key"/> is not held down</returns>
		public static bool IsKeyUp(Key key) => !IsKeyDown(key);

		/// <returns>True if <paramref name="key"/> is held down</returns>
		public static bool IsKeyDown(Key key) => _IsKeyDown((int)key);

		public static bool IsKeyDown(params Key[] keys)
		{
			foreach (Key key in keys)
				if (_IsKeyDown((int)key))
					return true;
			return false;
		}

		/// <returns>True if <paramref name="key"/> went down this frame</returns>
		public static bool IsKeyPressed(Key key) => _IsKeyPressed((int)key);

		/// <returns>True if <paramref name="key"/> was released this frame</returns>
		public static bool IsKeyReleased(Key key) => _IsKeyReleased((int)key);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _IsKeyDown(int key);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _IsKeyPressed(int key);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _IsKeyReleased(int key);

		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void PushID(string id);
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void PopID();

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

		public static Vector2 CalculateTextWidth(string text)
		{
			_CalculateTextWidth(text, out Vector2 output);
			return output;
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _CalculateTextWidth(string text, out Vector2 output);

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

		public static Vector2 GetCursorScreenPos()
		{
			_GetCursorScreenPos(out Vector2 pos);
			return pos;
		}
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetCursorScreenPos(out Vector2 value);

		public static void SetCursorPos(Vector2 value) => _SetCursorPos(ref value);
		public static void SetCursorPos(float x, float y) => SetCursorPos(new Vector2(x, y));
		public static void SetCursorScreenPos(Vector2 value) => _SetCursorScreenPos(ref value);
		public static void SetCursorScreenPos(float x, float y) => SetCursorScreenPos(new Vector2(x, y));

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetCursorPos(ref Vector2 value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetCursorScreenPos(ref Vector2 value);

		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void SetCursorPosX(float value);
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void SetCursorPosY(float value);

		[MethodImpl(MethodImplOptions.InternalCall)] public static extern float GetCursorPosX();
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern float GetCursorPosY();
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern float GetCursorScreenPosX();
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern float GetCursorScreenPosY();
		
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void BeginGroup();
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void EndGroup();

		[MethodImpl(MethodImplOptions.InternalCall)] public static extern float GetMouseDeltaX();
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern float GetMouseDeltaY();

		public static Vector2 GetMouseDelta()
		{
			_GetMouseDelta(out Vector2 output);
			return output;
		}
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void _GetMouseDelta(out Vector2 output);

		public static void PushClipRect(Vector2 min, Vector2 max, bool intersectWithCurrentClipRect) => _PushClipRect(ref min, ref max, intersectWithCurrentClipRect);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _PushClipRect(ref Vector2 min, ref Vector2 max, bool intersectWithCurrentClipRect);
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void PopClipRect();

		public static void DrawListPushClipRect(Vector2 min, Vector2 max, bool intersectWithCurrentClipRect) => _DrawListPushClipRect(ref min, ref max, intersectWithCurrentClipRect);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _DrawListPushClipRect(ref Vector2 min, ref Vector2 max, bool intersectWithCurrentClipRect);
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void DrawListPushClipRectFullScreen();
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void DrawListPopClipRect();
		#endregion

		#region Controls
		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void SameLine(float offset = 0, float spacing = -1);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Space();

		public static void HorizontalSpace(float width)
		{
			ImGUI.Dummy(width, 0);
			ImGUI.SameLine();
		}

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

		public static bool Button(string label) => Button(label, Vector2.Zero);
		public static bool Button(string label, Vector2 size) => _Button(label, ref size);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _Button(string label, ref Vector2 size);

		public static bool InvisibleButton(string label, Vector2 size) => _InvisibleButton(label, ref size);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _InvisibleButton(string label, ref Vector2 size);

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

		public static void Dummy(float sizeX, float sizeY) => Dummy(new Vector2(sizeX, sizeY));
		public static void Dummy(Vector2 size) => _Dummy(ref size);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Dummy(ref Vector2 size);
		#endregion

		#region Tree Node
		public static bool TreeNode(string label, TreeNodeFlags flags = TreeNodeFlags.None) => _TreeNode(label, (int)flags);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _TreeNode(string label, int flags);

		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void TreePush(string label);
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void TreePop();
		#endregion


		#region Input
		public static bool Input(string label, ref string value, int maxCharacters = 128, TextFlags flags = TextFlags.None)
			=> _InputText(label, ref value, maxCharacters, (int)flags);

		public static bool InputTextMultiline(string label, ref string value, int maxCharacters = 2048, TextFlags flags = TextFlags.None)
			=> InputTextMultiline(label, ref value, Vector2.Zero, maxCharacters, flags);

		public static bool InputTextMultiline(string label, ref string value, Vector2 size, int maxCharacters = 2048, TextFlags flags = TextFlags.None)
			=> _InputTextMultiline(label, ref value, maxCharacters, ref size, (int)flags);

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
		private static extern bool _InputTextMultiline(string label, ref string value, int maxCharacters, ref Vector2 size, int flags);

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

		public static bool ButtonImage(ulong textureID, Vector2 size) => _ImageButton(textureID, ref size, ref s_ImageDefaultColour, ref s_ButtonImageDefaultBackground);
		public static bool ButtonImage(ulong textureID, Vector2 size, Colour tint) => _ImageButton(textureID, ref size, ref tint, ref s_ButtonImageDefaultBackground);
		public static bool ButtonImage(ulong textureID, Vector2 size, Colour tint, Colour backgroundColour) => _ImageButton(textureID, ref size, ref tint, ref backgroundColour);

		public static bool ButtonImage(Texture texture, Vector2 size) => _ImageButton(texture?.ResourceID ?? 0, ref size, ref s_ImageDefaultColour, ref s_ButtonImageDefaultBackground);
		public static bool ButtonImage(Texture texture, Vector2 size, Colour tint) => _ImageButton(texture?.ResourceID ?? 0, ref size, ref tint, ref s_ButtonImageDefaultBackground);
		public static bool ButtonImage(Texture texture, Vector2 size, Colour tint, Colour backgroundColour) => _ImageButton(texture?.ResourceID ?? 0, ref size, ref tint, ref backgroundColour);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Image(ulong textureID, ref Vector2 size, ref Colour tint, ref Colour borderTint);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _ImageRenderTexture(IntPtr handle, ref Vector2 size, ref Colour tint, ref Colour borderTint);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _ImageButton(ulong textureID, ref Vector2 size, ref Colour tint, ref Colour backgroundColour);
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

		public static void OpenPopup(string id, WindowFlags flags = WindowFlags.None) =>
			_OpenPopup(id, (int)flags);

		public static bool BeginPopupModal(string id, WindowFlags flags = WindowFlags.None) =>
			_BeginPopupModal(id, (int)flags);

		public static bool BeginPopupModal(string id, bool isOpen, WindowFlags flags = WindowFlags.None) =>
			_BeginPopupModalOpen(id, ref isOpen, (int)flags);

		public static bool BeginPopupModal(string id, ref bool isOpen, WindowFlags flags = WindowFlags.None) =>
			_BeginPopupModalOpen(id, ref isOpen, (int)flags);

		public static bool BeginPopupContextItem(string id, PopupFlags flags = PopupFlags.None) =>
			_BeginPopupContextItem(id, (int)flags);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _BeginPopupContextItem(string id, int flags);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool IsPopupOpen(string id);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _BeginPopup(string id, int flags);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _BeginPopupModal(string id, int flags);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _BeginPopupModalOpen(string id, ref bool isOpen, int flags);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _OpenPopup(string id, int flags);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void EndPopup();

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void CloseCurrentPopup();
		#endregion

		#region Viewport
		public static bool WindowFocused => IsWindowFocused();
		public static bool WindowHovered => IsWindowHovered();
		public static bool WindowCollapsed => _IsWindowCollapsed();

		public static bool IsWindowFocused(FocusedFlags flags = FocusedFlags.None) => _IsWindowFocused((int)flags);
		public static bool IsWindowHovered(HoveredFlags flags = HoveredFlags.None) => _IsWindowHovered((int)flags);

		public static Vector2 WindowContentRegionMin
		{ get { _GetWindowContentRegionMin(out Vector2 output); return output; } }

		public static Vector2 WindowContentRegionMax
		{ get { _GetWindowContentRegionMax(out Vector2 output); return output; } }

		public static Vector2 ContentRegionAvailable
		{ get { _GetContentRegionAvail(out Vector2 output); return output; } }

		public static Vector2 WindowPosition
		{ get { _GetWindowPosition(out Vector2 output); return output; } }
		
		public static Vector2 WindowSize
		{ get { _GetWindowSize(out Vector2 output); return output; } }

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

		[MethodImpl(MethodImplOptions.InternalCall)] public static extern ulong GetMainViewportID();
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern ulong GetWindowViewportID();

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetWindowContentRegionMin(out Vector2 output);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetWindowContentRegionMax(out Vector2 output);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetContentRegionAvail(out Vector2 output);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetWindowPosition(out Vector2 output);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetWindowSize(out Vector2 output);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool _IsWindowFocused(int flags);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool _IsWindowHovered(int flags);

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
		
		public static Colour GetStyleColour(StyleColour style)
		{
			_GetStyleColour((int)style, out Colour colour);
			return colour;
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _GetStyleColour(int flag, out Colour value);

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

		#region Combos
		public static bool BeginCombo(string label, string preview = "", ComboFlags flags = ComboFlags.None) => _BeginCombo(label, preview, (int)flags);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _BeginCombo(string label, string preview, int flags);
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void EndCombo();

		public static T EnumCombo<T>(string label, T currentValue, ComboFlags flags = ComboFlags.None) where T : Enum
		{
			Type type = typeof(T);
			string currentValueStr = Enum.GetName(type, currentValue);
			if (!BeginCombo(label, currentValueStr, flags))
				return currentValue;
			string[] values = Enum.GetNames(type);
			for(int i = 0; i < values.Length; i++)
			{
				if (Selectable(values[i], values[i].Equals(currentValueStr)))
					currentValue = (T)Enum.GetValues(type).GetValue(i);
			}
			EndCombo();

			return currentValue;
		}
		
		public static bool EnumCombo(string label, Type enumType, ref object currentValue, ComboFlags flags = ComboFlags.None)
		{
			string currentValueStr = Enum.GetName(enumType, currentValue);
			if (!BeginCombo(label, currentValueStr, flags))
				return false;
			string[] values = Enum.GetNames(enumType);
			bool changedValue = false;
			for(int i = 0; i < values.Length; i++)
			{
				bool selected = values[i] == currentValueStr;
				if (!Selectable(values[i], selected))
					continue;
				currentValue = Enum.GetValues(enumType).GetValue(i);
				changedValue = true;

				if (selected)
					SetItemDefaultFocus();
			}
			EndCombo();
			return changedValue;
		}
		#endregion

		#region Draw List
		public static void AddLine(Vector2 p1, Vector2 p2, Colour colour, float thickness = 1.0f) =>
			_AddLine(ref p1, ref p2, ref colour, thickness);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _AddLine(ref Vector2 p1, ref Vector2 p2, ref Colour colour, float thickness);

		public static void AddRect(Vector2 p1, Vector2 p2, Colour colour, float rounding = 0, float thickness = 1.0f, DrawFlags flags = DrawFlags.None) =>
			_AddRect(ref p1, ref p2, ref colour, rounding, (int)flags, thickness);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _AddRect(ref Vector2 p1, ref Vector2 p2, ref Colour colour, float rounding, int flags, float thickness);

		public static void AddRectFilled(Vector2 p1, Vector2 p2, Colour colour, float rounding = 0, DrawFlags flags = DrawFlags.None) =>
			_AddRectFilled(ref p1, ref p2, ref colour, rounding, (int)flags);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _AddRectFilled(ref Vector2 p1, ref Vector2 p2, ref Colour colour, float rounding, int flags);

		public static void AddQuad(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4, Colour colour, float thickness) =>
			_AddQuad(ref p1, ref p2, ref p3, ref p4, ref colour, thickness);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _AddQuad(ref Vector2 p1, ref Vector2 p2, ref Vector2 p3, ref Vector2 p4, ref Colour colour, float thickness);

		public static void AddQuadFilled(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4, Colour colour) =>
			_AddQuadFilled(ref p1, ref p2, ref p3, ref p4, ref colour);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _AddQuadFilled(ref Vector2 p1, ref Vector2 p2, ref Vector2 p3, ref Vector2 p4, ref Colour colour);

		public static void AddTriangle(Vector2 p1, Vector2 p2, Vector2 p3, Colour colour, float thickness) =>
			_AddTriangle(ref p1, ref p2, ref p3, ref colour, thickness);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _AddTriangle(ref Vector2 p1, ref Vector2 p2, ref Vector2 p3, ref Colour colour, float thickness);

		public static void AddTriangleFilled(Vector2 p1, Vector2 p2, Vector2 p3, Colour colour) =>
			_AddTriangleFilled(ref p1, ref p2, ref p3, ref colour);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _AddTriangleFilled(ref Vector2 p1, ref Vector2 p2, ref Vector2 p3, ref Colour colour);

		public static void AddCircle(Vector2 center, float radius, Colour colour, float thickness = 1.0f, int numSegments = 0) =>
			_AddCircle(ref center, radius, ref colour, numSegments, thickness);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _AddCircle(ref Vector2 center, float radius, ref Colour colour, int numSegments, float thickness);

		public static void AddCircleFilled(Vector2 center, float radius, Colour colour, int numSegments = 0) =>
			_AddCircleFilled(ref center, radius, ref colour, numSegments);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _AddCircleFilled(ref Vector2 center, float radius, ref Colour colour, int numSegments);

		public static void AddNgon(Vector2 center, float radius, Colour colour, int numSegments, float thickness) =>
			_AddNgon(ref center, radius, ref colour, numSegments, thickness);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _AddNgon(ref Vector2 center, float radius, ref Colour colour, int numSegments, float thickness);

		public static void AddNgonFilled(Vector2 center, float radius, Colour colour, int numSegments) =>
			_AddNgonFilled(ref center, radius, ref colour, numSegments);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _AddNgonFilled(ref Vector2 center, float radius, ref Colour colour, int numSegments);

		public static void AddText(Vector2 position, string text, float fontSize = 16) =>
			AddText(position, text, Colour.White, fontSize);
		public static void AddText(Vector2 position, string text, Colour colour, float fontSize = 16) =>
			_AddText(ref position, text, ref colour, fontSize);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _AddText(ref Vector2 position, string textRaw, ref Colour colour, float fontSize);
		
		public static void AddBezierCubic(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4, Colour colour, float thickness = 1.0f, int segments = 0) =>
			_AddBezierCubic(ref p1, ref p2, ref p3, ref p4, ref colour, thickness, segments);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _AddBezierCubic(ref Vector2 p1, ref Vector2 p2, ref Vector2 p3, ref Vector2 p4, ref Colour colour, float thickness, int segments);

		public static void AddBezierQuadratic(Vector2 p1, Vector2 p2, Vector2 p3, Colour colour, float thickness = 1.0f, int segments = 0) =>
			_AddBezierQuadratic(ref p1, ref p2, ref p3, ref colour, thickness, segments);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _AddBezierQuadratic(ref Vector2 p1, ref Vector2 p2, ref Vector2 p3, ref Colour colour, float thickness, int segments);

		public static void AddImage(UUID textureID, Vector2 min, Vector2 max, Colour colour) =>
			_AddImage(textureID, ref min, ref max, ref colour);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _AddImage(ulong textureID, ref Vector2 min, ref Vector2 max, ref Colour colour);

		public static void AddImageRounded(UUID textureID, Vector2 min, Vector2 max, Colour colour, float rounding, DrawFlags flags = DrawFlags.None) =>
			_AddImageRounded(textureID, ref min, ref max, ref colour, rounding, (int)flags);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _AddImageRounded(ulong textureID, ref Vector2 min, ref Vector2 max, ref Colour colour, float rounding, int flags);
		#endregion

		#region Drag and Drop
		public static bool BeginDragDropSource(DragDropFlags flags = DragDropFlags.None) => _BeginDragDropSource((int)flags);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _BeginDragDropSource(int flags);

		[MethodImpl(MethodImplOptions.InternalCall)] public static extern bool EndDragDropSource();

		[MethodImpl(MethodImplOptions.InternalCall)] public static extern bool BeginDragDropTarget();
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern bool EndDragDropTarget();

		[MethodImpl(MethodImplOptions.InternalCall)] public static extern void SetDragDropPayload(string type, object value);
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern object GetDragDropPayload();
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern string GetDragDropPayloadType();
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern bool IsDragDropPayloadType(string type);
		[MethodImpl(MethodImplOptions.InternalCall)] public static extern bool DragDropPayloadIsDelivery();

		public static object AcceptDragDropPayload(string type, DragDropFlags flags = DragDropFlags.None) => _AcceptDragDropPayload(type, (int)flags);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern object _AcceptDragDropPayload(string type, int flags);
		#endregion

		#region Gizmos
		public static class Gizmo
		{
			public static bool IsOver => Gizmo_IsOver();
			public static bool IsUsing => Gizmo_IsUsing();

			public static void SetRect(float x, float y, float width, float height) => Gizmo_SetRect(x, y, width, height);
			public static void SetRect(Vector2 position, Vector2 size) => Gizmo_SetRect(position.x, position.y, size.x, size.y);

			public static void SetDrawList() => Gizmo_SetDrawList();

			public static void Enable(bool enable = true) => Gizmo_Enable(enable);

			public static void Manipulate(Camera viewPoint, Transform target, Vector2 drawRegion, ManipulateOperation operation, bool isLocalSpace = true, float snapping = 0)
			{
				if (viewPoint != null && target != null)
					_Gizmo_Manipulate(viewPoint.Handle, target.Handle, ref drawRegion, (uint)operation, isLocalSpace, snapping);
			}
		}

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool Gizmo_IsOver();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool Gizmo_IsUsing();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void Gizmo_SetRect(float x, float y, float width, float height);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void Gizmo_SetDrawList();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void Gizmo_Enable(bool enable);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Gizmo_Manipulate(IntPtr cameraHandle, IntPtr transformHandle, ref Vector2 drawRegion, uint operation, bool localSpace, float snapping);
		#endregion
	}
}