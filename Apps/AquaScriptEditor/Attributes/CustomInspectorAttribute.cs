using System;

namespace AquaEditor
{
	[AttributeUsage(AttributeTargets.Class)]
	public class CustomInspectorAttribute : Attribute
	{
		public Type TargetType { get; private set; }

		public CustomInspectorAttribute(Type type) => TargetType = type;
	}
}