using System;

namespace Yonai
{
	[System.Diagnostics.DebuggerDisplay("{m_Value}")]
	public struct UUID
	{
		private ulong m_Value;

		public static readonly UUID Invalid = 0;

		public UUID(ulong value) => m_Value = value;

		public override string ToString() => m_Value.ToString();

		public static implicit operator ulong(UUID uuid) => uuid.m_Value;
		public static implicit operator UUID(ulong id) => new UUID(id);

		public static bool operator ==(UUID a, UUID b) => a.m_Value == b.m_Value;
		public static bool operator !=(UUID a, UUID b) => a.m_Value != b.m_Value;

		public override int GetHashCode() => m_Value.GetHashCode();
		public override bool Equals(object obj) => obj is UUID && ((UUID)obj).m_Value == m_Value;
	}
}