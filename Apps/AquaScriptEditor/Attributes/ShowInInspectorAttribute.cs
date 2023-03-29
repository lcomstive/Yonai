using System;

namespace AquaEditor
{
	/// <summary>
	/// Reveals private fields and properties in the inspector
	/// </summary>
	[AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]
	public class ShowInInspectorAttribute : Attribute { }
}