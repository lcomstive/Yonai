using Yonai;
using System;
using YonaiEditor.Systems;
using System.Collections.Generic;

namespace YonaiEditor.Views
{
	public class SearchView : View
	{
		private struct SearchResult
		{
			public string Value;

			public List<(string, bool)> Splits;

			public SearchResult(string value)
			{
				Value = value;
				Splits = new List<(string, bool)>();
			}

			public SearchResult(string value, string substring)
			{
				Value = value;
				Splits = new List<(string, bool)>();
				Highlight(substring);
			}

			public void Highlight(string substring)
			{
				substring = substring.ToLower();
				Splits.Clear();

				string value = string.Copy(Value);
				int index = Value.ToLower().IndexOf(substring);
				while(index >= 0)
				{
					if(index > 0)
						Splits.Add((value.Substring(0, index), false));
					Splits.Add((value.Substring(index, substring.Length), true));
					value = value.Substring(index + substring.Length);

					index = value.ToLower().IndexOf(substring);
				}

				Splits.Add((value, false));
			}

			public static implicit operator string(SearchResult result) => result.Value;
			public static implicit operator SearchResult(string result) => new SearchResult(result);
		}

		private int m_HighlightedIndex = 0;
		private string m_SearchInput = string.Empty;

		private static string[] m_AllValues = null;
		private static List<SearchResult> m_SearchResults = new List<SearchResult>();
		private static Action<string> m_Callback = null;

		/// <summary>
		/// Last ID used when opening search in immediate mode.
		/// Used with <see cref="IsSearchFinished(string, out string)"/> to validate if value should be returned.
		/// </summary>
		private static string m_IMSearchID = string.Empty;

		/// <summary>
		/// Last result returned from a search
		/// </summary>
		private static string m_IMSearchResult = string.Empty;

		private const float InputPadding = 5;
		private const int MinInputLength = 3;

		protected override void Draw()
		{
			if (m_SearchResults == null)
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
				if(ImGUI.Input(string.Empty, ref m_SearchInput, 256))
					DoSearch(m_SearchInput);

				ImGUI.PopStyleColour();

				// Search results //
				Colour selectedColour = ImGUI.GetStyleColour(ImGUI.StyleColour.ButtonHovered);
				ImGUI.PushStyleColour(ImGUI.StyleColour.WindowBg, new Colour(0.25f, 0.25f, 0.25f));
				ImGUI.PushStyleVar(ImGUI.StyleVar.ButtonTextAlign, new Vector2(0, 0.5f));
				ImGUI.BeginChild("SearchResults");
				for(int i = 0; i < m_SearchResults.Count; i++)
				{
					ImGUI.PushStyleColour(ImGUI.StyleColour.Button,
						i == m_HighlightedIndex ? selectedColour : Colour.None);

					// if (ImGUI.Button(m_SearchResults[i].ToString(), new Vector2(contentRegion.x, 0)))
					if (DrawSearchResult(m_SearchResults[i], contentRegion.x))
						selectedIndex = i;
					if (ImGUI.IsItemHovered())
						m_HighlightedIndex = i;

					ImGUI.PopStyleColour();
				}

				ImGUI.EndChild();
				ImGUI.PopStyleVar();
				ImGUI.PopStyleColour();

				// Handle key input //
				if (ImGUI.IsKeyPressed(Key.ArrowUp))   m_HighlightedIndex = MathUtils.Clamp(m_HighlightedIndex - 1, 0, m_SearchResults.Count);
				if (ImGUI.IsKeyPressed(Key.ArrowDown)) m_HighlightedIndex = MathUtils.Clamp(m_HighlightedIndex + 1, 0, m_SearchResults.Count - 1);

				if (ImGUI.IsKeyPressed(Key.Enter) || ImGUI.IsKeyPressed(Key.KP_ENTER))
					// Select the highlighted item
					selectedIndex = m_HighlightedIndex;
			}
			ImGUI.End();

			// Check if window requested to be closed
			if (!isOpen || selectedIndex >= 0)
			{
				Select(selectedIndex);
				EditorUIService.Close<SearchView>();
			}
		}

		private bool DrawSearchResult(SearchResult value, float width)
		{
			Vector2 size = new Vector2(width - 20, ImGUI.FontSize * 1.5f);
			Vector2 pos = ImGUI.GetCursorScreenPos();
			bool clicked = ImGUI.InvisibleButton(value, size);

			Colour buttonColour = ImGUI.IsItemHovered() ?
				ImGUI.GetStyleColour(ImGUI.StyleColour.ButtonHovered) :
				ImGUI.GetStyleColour(ImGUI.StyleColour.Button);

			Colour textColour = ImGUI.GetStyleColour(ImGUI.StyleColour.Text);
			Colour textHighlightedColour = ImGUI.GetStyleColour(ImGUI.StyleColour.ButtonActive);

			Vector2 offset = new Vector2(5, 2.5f);

			ImGUI.AddRectFilled(pos, pos + size, buttonColour, 2.5f);

			ImGUI.SetCursorScreenPos(pos + offset);

			foreach ((string text, bool highlight) in value.Splits)
			{
				ImGUI.SetCursorScreenPos(pos + new Vector2(offset.x));
				ImGUI.Text(text, highlight ? textHighlightedColour : textColour);
				offset.x += ImGUI.CalculateTextWidth(text).x;
			}

			if (value.Splits.Count == 0)
			{
				ImGUI.SetCursorScreenPos(pos + offset);
				ImGUI.Text(value, textColour);
			}

			return clicked;
		}

		private void Select(int index)
		{
			m_IMSearchResult = index >= 0 ? m_SearchResults[index].Value : string.Empty;

			m_Callback?.Invoke(m_IMSearchResult);
			m_Callback = null;
			m_AllValues = null;
		}

		/// <summary>
		/// Opens search window with <paramref name="values"/>
		/// </summary>
		/// <param name="values"></param>
		/// <param name="callback"></param>
		/// <returns>False if search already in progress</returns>
		public static bool Search<T>(T[] values, Action<string> callback)
		{
			if (m_Callback != null)
				return false;

			SearchView view = EditorUIService.Open<SearchView>();

			m_Callback = callback;
			PopulateValues(values);

			view.DoSearch(string.Empty);

			return true;
		}

		/// <summary>
		/// Opens a search window in an immediate-mode fashion
		/// </summary>
		/// <param name="ID"></param>
		/// <param name="values"></param>
		public static void OpenSearch<T>(string ID, T[] values)
		{
			SearchView view = EditorUIService.Open<SearchView>();
			m_IMSearchID = ID;
			m_IMSearchResult = string.Empty;
			PopulateValues(values);

			view.DoSearch(string.Empty);
		}

		private static void PopulateValues<T>(T[] values)
		{
			m_AllValues = new string[values.Length];
			for (int i = 0; i < values.Length; i++)
				m_AllValues[i] = values[i].ToString();
		}

		public static bool IsSearchOpen(string ID) => m_IMSearchID.Equals(ID);

		/// <summary>
		/// Checks if search has been completed
		/// </summary>
		/// <param name="result">Search output, or <see cref="string.Empty"/> if cancelled</param>
		/// <returns>True if search has been finished and result returned</returns>
		public static bool IsSearchFinished(string ID, out string result)
		{
			result = string.Empty;
			if (IsSearchOpen(ID) && !EditorUIService.IsViewOpen<SearchView>())
			{
				result = m_IMSearchResult;
				m_IMSearchID = m_IMSearchResult = string.Empty;
				return true;
			}
			return false;
		}

		private void DoSearch(string input)
		{
			input = input.ToLower();
			m_SearchResults.Clear();
			m_HighlightedIndex = 0;

			// Input length not long enough, add all search results
			if(input.Length < MinInputLength)
			{
				foreach (string value in m_AllValues)
					m_SearchResults.Add(new SearchResult(value));
				return;
			}

			foreach (string value in m_AllValues)
			{
				if (value.ToLower().Contains(input))
					m_SearchResults.Add(new SearchResult(value, input));
			}
		}
	}
}