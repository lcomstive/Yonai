using System;
using System.Runtime.CompilerServices;

namespace AquaEngine.Systems
{
	public class NativeSystem : AquaSystem
	{
		private IntPtr m_Handle = IntPtr.Zero;
		protected IntPtr Handle
		{
			get
			{
				if (m_Handle == IntPtr.Zero)
					m_Handle = _GetHandle(World.ID, GetType());
				return m_Handle;
			}
		}

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern IntPtr _GetHandle(ulong worldID, Type type);
	}
}

