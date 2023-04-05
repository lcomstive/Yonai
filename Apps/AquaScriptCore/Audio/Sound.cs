using System;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public class SoundImportSettings : IImportSettings
	{
		public string FilePath;
	}

	public class Sound : NativeResourceBase
	{
		protected override void OnLoad()
		{
			ulong resourceID;
			IntPtr handle;

			_Load(ResourcePath, out resourceID, out handle);

			ResourceID = resourceID;
			Handle = handle;
		}

		protected override void OnImported()
		{
			if(TryGetImportSettings(out SoundImportSettings importSettings))
				_Import(Handle, importSettings.FilePath);
			else
				_Import(Handle, string.Empty);
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load(string path, out ulong resourceID, out IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Import(IntPtr handle, string audioFilePath);
		#endregion
	}
}