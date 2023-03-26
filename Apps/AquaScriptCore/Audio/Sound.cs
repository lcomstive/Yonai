using System;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public class Sound : NativeResourceBase
	{
		internal override bool Load(string path, params object[] args)
		{
			uint resourceID;
			IntPtr handle;

			if (args.Length < 1)
			{
				Log.Warning("Args for sound: <audioFilePath>");
				return false;
			}

			_Load(path, (string)args[0], out resourceID, out handle);

			ResourceID = resourceID;
			Handle = handle;
			return true;
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load(string path, string audioFilePath, out uint resourceID, out IntPtr handle);
		#endregion
	}
}