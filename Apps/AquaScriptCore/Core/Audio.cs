using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public struct AudioDevice
	{
		public uint Index { get; internal set; }
		public string Name { get; internal set; }
	}

	public static class Audio
	{
		public static AudioDevice[] Devices;

		private static AudioDevice m_OutputDevice;
		public static AudioDevice OutputDevice
		{
			get => m_OutputDevice;
			set => _SetOutputDevice(value.Index);
		}

		public static void PlayOnce(string filepath) => _PlayOnce(filepath);

		// Called from native code
		private static void _RefreshDevices()
		{
			uint deviceCount = _GetDeviceCount();
			Devices = new AudioDevice[deviceCount];
			for (uint i = 0; i < deviceCount; i++)
				Devices[i] = new AudioDevice()
				{
					Index = i,
					Name = _GetDeviceName(i)
				};

			DevicesChanged?.Invoke();
		}

		// Called from native code
		private static void _OutputDeviceChanged()
		{
			m_OutputDevice = Devices[_GetOutputDevice()];
			OutputDeviceChanged?.Invoke();
			Log.Debug($"Audio device changed to '{OutputDevice.Name}'");
		}

		public delegate void OnDevicesChanged();
		public static event OnDevicesChanged DevicesChanged;

		public delegate void OnDefaultDeviceChanged();
		public static event OnDefaultDeviceChanged OutputDeviceChanged;

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _GetDeviceCount();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _GetDefaultDevice();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _GetOutputDevice();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _GetDeviceName(uint index);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetOutputDevice(uint index);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _PlayOnce(string filepath);
		#endregion
	}
}
