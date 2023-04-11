using System;

namespace AquaEngine
{
	/// <summary>
	/// Overrides default serialize behaviour
	/// </summary>
	[AttributeUsage(AttributeTargets.Field | AttributeTargets.Property | AttributeTargets.Class)]
	public class SerializeAttribute : Attribute
	{
		public bool ShouldSerialize = true;

		public SerializeAttribute(bool serialize = true) => ShouldSerialize = serialize;
	}
}