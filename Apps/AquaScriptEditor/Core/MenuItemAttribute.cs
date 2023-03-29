using System;

namespace AquaEditor
{
	[AttributeUsage(AttributeTargets.Method)]
	public class MenuItemAttribute : Attribute
	{
		public string Path { get; private set; }

		public string Shortcut { get; set; }

		/// <summary>
		/// Lower order is displayed first
		/// </summary>
		public uint Order { get; set; } = 99;

		public MenuItemAttribute(string path) => Path = path;
	}
}