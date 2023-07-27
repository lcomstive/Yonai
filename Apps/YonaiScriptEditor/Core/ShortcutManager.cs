using System;
using System.Collections.Generic;
using Yonai;
using YonaiEditor.Commands;

namespace YonaiEditor
{
	public static class ShortcutManager
	{
		/// <summary>
		/// Maps set of keys to a function, e.g. [ Key.Control, Key.S ] saves current scene
		/// </summary>
		private static Dictionary<Key[], Action> m_Shortcuts = new Dictionary<Key[], Action>();

		internal static void Update()
		{
			foreach (var shortcut in m_Shortcuts)
			{
				bool allKeysPressed = true;
				bool pressedThisFrame = false;
				foreach(var key in shortcut.Key)
				{
					if (ImGUI.IsKeyPressed(key))
					{
						pressedThisFrame = true;
						continue;
					}
					if (ImGUI.IsKeyDown(key))
						continue;

					allKeysPressed = false;
					break;
				}

				if (allKeysPressed && pressedThisFrame)
				{
					shortcut.Value?.Invoke();
					break;
				}
			}
		}

		public static void Map(Action callback, params Key[] keys) => m_Shortcuts.Add(keys, callback);
	}
}