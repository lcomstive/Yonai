using AquaEngine;
using System.Reflection;
using System.Collections.Generic;
using System;
using AquaEngine.Graphics;

namespace AquaEditor.EditorUI
{
	internal class MenuItemData
	{
		public string Name;
		public Action Callback = null;
		public MenuItemAttribute Attribute;

		public List<MenuItemData> Children = new List<MenuItemData>();

		public bool IsDirectory => Callback == null || Attribute == null;

		public MenuItemData(string name, MenuItemAttribute attribute = null, Action callback = null)
		{
			Name = name;
			Callback = callback;
			Attribute = attribute;
		}

		public void Render()
		{
			if (Attribute?.PrependSeparator ?? false)
				ImGUI.Separator();
			if (IsDirectory)
				RenderDirectory();
			else
			{
				if (!string.IsNullOrEmpty(Attribute.Icon))
				{
					Texture icon = Icons.Has(Attribute.Icon) ? Icons.Get(Attribute.Icon) : Resource.Get<Texture>(Attribute.Icon);
					ImGUI.Image(icon, new Vector2(20, 20));
					ImGUI.SameLine();
				}
				if (ImGUI.MenuItem(Name, Attribute.Shortcut))
					Callback?.Invoke();
			}
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
			=> Add(path, (Action)null, null);

		public void Add(MethodInfo method, MenuItemAttribute attribute)
			=> Add(attribute.Path, method, attribute);

		public void Add(Action callback, MenuItemAttribute attribute)
			=> Add(attribute.Path, callback, attribute);

		public void Add(string path, MethodInfo methodInfo, MenuItemAttribute attribute) =>
			Add(path, (Action)Delegate.CreateDelegate(typeof(Action), methodInfo), attribute);
		public void Add(string path, Action callback, MenuItemAttribute attribute)
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
				if (callback != null && attribute != null)
					Children.Add(new MenuItemData(subpath, attribute, callback));
				else // Add empty directory
					Children.Add(new MenuItemData(subpath));
				return;
			}

			// Search for child with matching directory (paths[dirIndex])
			foreach (MenuItemData child in Children)
			{
				if (child.Name == splitPath[0] && child.IsDirectory)
				{
					child.Add(path.Substring(splitPath[0].Length + 1), callback, attribute);
					return;
				}
			}

			// No child directory found, create new one
			MenuItemData directory = new MenuItemData(splitPath[0]);
			directory.Add(path.Substring(splitPath[0].Length + 1), callback, attribute);
			Children.Add(directory);

			Log.Debug($"Created menu item directory {splitPath[0]}");
		}
	}
}