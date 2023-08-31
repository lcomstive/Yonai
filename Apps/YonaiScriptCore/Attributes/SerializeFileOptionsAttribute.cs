using System;

namespace Yonai
{
	/// <summary>
	/// Overrides default serialize behaviour
	/// </summary>
	[AttributeUsage(AttributeTargets.Field | AttributeTargets.Property | AttributeTargets.Class)]
	public class SerializeFileOptionsAttribute : Attribute
	{
		/// <summary>
		/// When true, saves and loads serialised data from file separate to <see cref="ResourceBase.ResourcePath"/>.<br></br>
		/// This is useful for serialising settings for an existing filetype (e.g. import settings for models and textures)
		/// </summary>
		public bool SaveSeparateFile = false;
	}
}