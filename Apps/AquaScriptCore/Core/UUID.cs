using System;

namespace AquaEngine
{
	[System.Diagnostics.DebuggerDisplay("{m_Value}")]
	public class UUID
	{
		private ulong m_Value;

		public static readonly UUID Invalid = 0;

		public UUID(ulong value) => m_Value = value;

		public override string ToString() => m_Value.ToString();

		public static implicit operator ulong(UUID uuid) => uuid.m_Value;
		public static implicit operator UUID(ulong id) => new UUID(id);
	}
}