using System;

namespace AquaEditor
{
	[AttributeUsage(AttributeTargets.Method)]
	public class MenuItemAttribute : Attribute
	{
		public string Path { get; private set; }

		public string Shortcut { get; set; }

		public bool PrependSeparator { get; set; } = false;

		public MenuItemAttribute(string path) => Path = path;
	}
}