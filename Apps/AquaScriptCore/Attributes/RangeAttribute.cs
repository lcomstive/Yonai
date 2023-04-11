using System;

namespace AquaEngine
{
	/// <summary>
	/// Hides public fields and properties from the inspector
	/// </summary>
	[AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]
	public class RangeAttribute : Attribute
	{
		public Vector2 Range { get; set; }

		public RangeAttribute(float max) => Range = new Vector2(0, max);
		public RangeAttribute(float min, float max) => Range = new Vector2(min, max);
	}
}