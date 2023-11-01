using Yonai;
using System;
using YonaiEditor.Systems;

namespace YonaiEditor.Views
{
	public class SearchView<T> : View
	{
		private string m_SearchInput = string.Empty;
		private int m_HighlightedIndex = 0;

		private static T[] m_Values = null;
		private static Action<object> m_Callback = null;

		private const float InputPadding = 5;

		protected override void Draw()
		{
			if (m_Values == null)
				return; // Nothing to show

			bool isOpen = true;
			int selectedIndex = -1;
			if (ImGUI.Begin("Search", ref isOpen))
			{
				Vector2 contentRegion = ImGUI.ContentRegionAvailable;

				// Search text input //
				if(ImGUI.IsWindowFocused(ImGUI.FocusedFlags.RootAndChildWindows) && !ImGUI.IsAnyMouseClicked() && !ImGUI.IsAnyItemActive())
					ImGUI.SetKeyboardFocusHere();
				ImGUI.PushStyleColour(ImGUI.StyleColour.FrameBg, Colour.None);

				ImGUI.SetCursorPosY(ImGUI.GetCursorPosY() + (InputPadding * 2));
				ImGUI.HorizontalSpace(InputPadding);
				ImGUI.SetNextItemWidth(contentRegion.x - (InputPadding * 2));
				ImGUI.Input(string.Empty, ref m_SearchInput, 256);

				ImGUI.PopStyleColour();

				// Search results //
				Colour selectedColour = ImGUI.GetStyleColour(ImGUI.StyleColour.ButtonHovered);
				ImGUI.PushStyleColour(ImGUI.StyleColour.WindowBg, new Colour(0.25f, 0.25f, 0.25f));
				ImGUI.PushStyleVar(ImGUI.StyleVar.ButtonTextAlign, new Vector2(0, 0.5f));
				ImGUI.BeginChild("SearchResults");
				for(int i = 0; i < m_Values.Length; i++)
				{
					ImGUI.PushStyleColour(ImGUI.StyleColour.Button,
						i == m_HighlightedIndex ? selectedColour : Colour.None);

					if (ImGUI.Button(m_Values[i].ToString(), new Vector2(contentRegion.x, 0)))
						selectedIndex = i;
					if (ImGUI.IsItemHovered())
						m_HighlightedIndex = i;

					ImGUI.PopStyleColour();
				}
				ImGUI.EndChild();
				ImGUI.PopStyleVar();
				ImGUI.PopStyleColour();

				// Handle key input //
				if (ImGUI.IsKeyPressed(Key.ArrowUp)) m_HighlightedIndex = MathUtils.Clamp(m_HighlightedIndex - 1, 0, m_Values.Length);
				if (ImGUI.IsKeyPressed(Key.ArrowDown)) m_HighlightedIndex = MathUtils.Clamp(m_HighlightedIndex + 1, 0, m_Values.Length);

				if (ImGUI.IsKeyPressed(Key.Enter) || ImGUI.IsKeyPressed(Key.KP_ENTER))
					// Select the highlighted item
					selectedIndex = m_HighlightedIndex;
			}
			ImGUI.End();

			// Check if window requested to be closed
			if (!isOpen || selectedIndex >= 0)
			{
				Select(selectedIndex);
				EditorUIService.Close<SearchView<T>>();
			}
		}

		private void Select(int index)
		{
			m_Callback.Invoke(index >= 0 ? (object)m_Values[index] : null);
			m_Callback = null;
			m_Values = null;
		}

		/// <summary>
		/// Opens search window with <paramref name="values"/>
		/// </summary>
		/// <param name="values"></param>
		/// <param name="callback"></param>
		/// <returns>False if search already in progress</returns>
		public static bool Search(T[] values, Action<object> callback)
		{
			if (m_Callback != null)
				return false;

			EditorUIService.Open<SearchView<T>>();

			m_Callback = callback;
			m_Values = values;

			return true;
		}
	}
}