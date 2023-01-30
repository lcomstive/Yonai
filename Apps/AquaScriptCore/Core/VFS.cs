using System;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public static class VFS
	{
		public static string CurrentDirectory => _aqua_internal_GetCurrentDirectory();

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _aqua_internal_GetCurrentDirectory();
		#endregion
	}
}