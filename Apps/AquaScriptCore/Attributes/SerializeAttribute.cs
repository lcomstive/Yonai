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

		/// <summary>
		/// Label to use when serializing and deserializing this data.
		/// Set to empty string to use default.
		/// </summary>
		public string Label { get; set; } = string.Empty;

		public SerializeAttribute(bool serialize = true) => ShouldSerialize = serialize;
	}
}