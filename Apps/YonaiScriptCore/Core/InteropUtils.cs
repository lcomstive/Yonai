using System;
using System.Runtime.InteropServices;

namespace Yonai._Internal
{
	internal static class InteropUtils
	{
		/// <summary>
		/// Size, in bytes, for an IntPtr / void*
		/// </summary>
		public const int PtrSize = 8; // x64

		/// <summary>
		/// Creates a copy of an array of structs in unmanaged memory.
		/// Don't forget to call <see cref="Marshal.FreeHGlobal(IntPtr)"/> to free the memory!
		/// </summary>
		/// <returns>Pointer to unmanaged memory</returns>
		internal static IntPtr CreateNativeHandle<T>(params T[] values) where T : struct
		{
			IntPtr ptr, currentPtr;

			int structSize = Marshal.SizeOf(typeof(T));
			ptr = Marshal.AllocHGlobal(structSize * values.Length);
			currentPtr = ptr;

			for (int i = 0; i < values.Length; i++, currentPtr += structSize)
				Marshal.StructureToPtr(values[i], currentPtr, false);

			return ptr;
		}
	}
}