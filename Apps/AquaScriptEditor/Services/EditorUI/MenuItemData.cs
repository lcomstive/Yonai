using AquaEngine;
using System.Reflection;
using System.Collections.Generic;

namespace AquaEditor.EditorUI
{
	internal class MenuItemData
	{
		public string Name;
		public MethodInfo Callback = null;
		public MenuItemAttribute Attribute;

		public List<MenuItemData> Children = new List<MenuItemData>();

		public bool IsDirectory => Callback == null || Attribute == null;

		public MenuItemData(string name, MenuItemAttribute attribute = null, MethodInfo callback = null)
		{
			Name = name;
			Callback = callback;
			Attribute = attribute;
		}

		public void Render()
		{
			if (IsDirectory)
				RenderDirectory();
			else if (ImGUI.MenuItem(Name, Attribute.Shortcut))
				Callback?.Invoke(null, null);
		}

		private void RenderDirectory()
		{
			if (!ImGUI.BeginMenu(Name))
				return;

			foreach (MenuItemData item in Children)
				item.Render();

			ImGUI.EndMenu();
		}

		public void AddDirectory(string path)
			=> Add(null, path, null);

		public void Add(MethodInfo method, MenuItemAttribute attribute)
			=> Add(method, attribute.Path, attribute);

		public void Add(MethodInfo method, string path, MenuItemAttribute attribute)
		{
			// Check for valid path
			if (string.IsNullOrEmpty(path))
				return;

			string[] splitPath = path.Split('/');

			// No more directories, add as child
			if (splitPath.Length == 1)
			{
				string subpath = splitPath[splitPath.Length - 1];
				// Check if method and attribute are available, if not could just be creating an empty directory
				if (method != null && attribute != null)
					Children.Add(new MenuItemData(subpath, attribute, method));
				else // Add empty directory
					Children.Add(new MenuItemData(subpath));
				return;
			}

			// Search for child with matching directory (paths[dirIndex])
			foreach (MenuItemData child in Children)
			{
				if (child.Name == splitPath[0] && child.IsDirectory)
				{
					child.Add(method, path.Substring(splitPath[0].Length + 1), attribute);
					return;
				}
			}

			// No child directory found, create new one
			MenuItemData directory = new MenuItemData(splitPath[0]);
			directory.Add(method, path.Substring(splitPath[0].Length + 1), attribute);
			Children.Add(directory);

			Log.Debug($"Created menu item directory {splitPath[0]}");
		}

		public void Sort()
		{

		}
	}
}