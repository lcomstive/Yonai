using System;

namespace AquaEngine
{
	/// <summary>
	/// Overrides default serialize behaviour
	/// </summary>
	[AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]
	public class ShouldSerializeAttribute : Attribute
	{
		public bool ShouldSerialize = true;

		public ShouldSerializeAttribute(bool serialize = true) => ShouldSerialize = serialize;
	}
}