using System;
using Yonai.IO;
using System.Runtime.CompilerServices;

namespace Yonai
{
	public struct SoundImportSettings : IImportSettings {}

	public class Sound : NativeResourceBase
	{
		protected override void OnLoad()
		{
			ulong resourceID = ResourceID;
			_Load(ResourcePath, out resourceID, out IntPtr handle);

			ResourceID = resourceID;
			Handle = handle;

			OnImported();
		}

		protected override void OnImported()
		{
			SoundImportSettings settings;
			if (TryGetImportSettings(out settings))
				settings = new SoundImportSettings();

			_Import(Handle, VFS.Read(ResourcePath));
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load(string path, out ulong resourceID, out IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Import(IntPtr handle, byte[] data);
		#endregion
	}
}