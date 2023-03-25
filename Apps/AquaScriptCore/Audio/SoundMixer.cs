using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AquaEngine
{
	public class SoundMixer : NativeResourceBase
	{
		public string Name
		{
			get => _GetName(Handle);
			set => _SetName(Handle, value);
		}

		public float Volume
		{
			get => _GetVolume(Handle);
			set => _SetVolume(Handle, value);
		}

		/*
		 * Args: <optional: string mixerName> <optional: (uint or SoundMixer) parentMixerID>
		 */
		internal override bool Load(string path, params object[] args)
		{
			uint resourceID;
			IntPtr handle;

			string name = args.Length > 0 && args[0] is string ? "Mixer" : (string)args[0];
			if(args.Length < 2)
				_Load0(path, name, out resourceID, out handle);
			else
			{
				if(args[2] is SoundMixer)
					_Load1(path, name, ((SoundMixer)args[2]).ResourceID, out resourceID, out handle);
				else if(args[2] is uint)
					_Load1(path, name, (uint)args[2], out resourceID, out handle);
				else
					_Load0(path, name, out resourceID, out handle);
			}

			ResourceID = resourceID;
			Handle = handle;
			return true;
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load0(string path, string mixerName, out uint resourceID, out IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load1(string path, string mixerName, uint parentMixerID, out uint resourceID, out IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _GetVolume(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetVolume(IntPtr handle, float value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _GetName(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetName(IntPtr handle, string value);
		#endregion

	}
}
