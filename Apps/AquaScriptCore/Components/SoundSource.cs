using System;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public enum SoundState : uint
	{
		Stopped = 0,
		Playing,
		Paused
	}

	public class SoundSource : NativeComponent
	{
		private uint m_Sound = uint.MaxValue;
		public uint Sound
		{
			get => m_Sound;
			set
			{
				if (m_Sound != value) // Check for change
					_SetSoundClip(Handle, (m_Sound = value));
			}
		}

		public bool IsPlaying => _IsPlaying(Handle);

		public SoundState State => (SoundState)_GetState(Handle);

		public void Play()	=> _Play(Handle);
		public void Pause() => _Pause(Handle);
		public void Stop()	=> _Stop(Handle);

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _GetSoundClip(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetSoundClip(IntPtr handle, uint soundClip);
	
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Play(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Pause(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Stop(IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _IsPlaying(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _GetState(IntPtr handle);
		#endregion
	}
}

