using System;

namespace Yonai
{
	/// <summary>
	/// Hides public fields and properties from the inspector
	/// </summary>
	[AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]
	public class HideInInspectorAttribute : Attribute { }
}