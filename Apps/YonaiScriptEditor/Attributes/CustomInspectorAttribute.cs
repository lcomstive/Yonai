using System;

namespace YonaiEditor
{
	[AttributeUsage(AttributeTargets.Class)]
	public class CustomInspectorAttribute : Attribute
	{
		/// <summary>
		/// Type to draw inspector for.
		/// </summary>
		public Type TargetType { get; set; } = null;

		/// <summary>
		/// File extensions supported by this inspector.
		/// When a matching file type is selected in the resources view,
		/// <see cref="CustomInspector.Target"/> is set to the relevant <see cref="Yonai.VFSFile"/>.
		/// </summary>
		public string[] FileExtensions { get; set; } = null;

		public CustomInspectorAttribute(Type type) => TargetType = type;
		public CustomInspectorAttribute(params string[] fileExtensions) => FileExtensions = fileExtensions;

		public CustomInspectorAttribute(Type type, params string[] fileExtensions)
		{
			TargetType = type;
			FileExtensions = fileExtensions;
		}
	}
}