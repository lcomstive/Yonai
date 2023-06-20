using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public struct AudioDevice
	{
		public uint Index { get; internal set; }
		public string Name { get; internal set; }

		public override int GetHashCode() => Index.GetHashCode();
		public override bool Equals(object obj) => obj is AudioDevice device && ((AudioDevice)obj).Index == Index;

		public static bool operator ==(AudioDevice a, AudioDevice b) => a.Index == b.Index;
		public static bool operator !=(AudioDevice a, AudioDevice b) => a.Index != b.Index;
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
			uint output = _GetOutputDevice();
			if(output > Devices.Length)
				return; // Out of bounds

			m_OutputDevice = Devices[_GetOutputDevice()];
			OutputDeviceChanged?.Invoke();
			Log.Debug($"Audio device changed to '{OutputDevice.Name}'");
		}

		public delegate void OnDevicesChanged();
		public delegate void OnDefaultDeviceChanged();

		/// <summary>
		/// Called when the device list has been refreshed
		/// </summary>
		public static event OnDevicesChanged DevicesChanged;

		/// <summary>
		/// Called when the output device has been changed
		/// </summary>
		public static event OnDefaultDeviceChanged OutputDeviceChanged;

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _GetDeviceCount();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _GetDefaultDevice();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _GetOutputDevice();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _GetDeviceName(uint index);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetOutputDevice(uint index);
		#endregion
	}
}
