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
		public uint Sound
		{
			get => _GetSound(Handle);
			set => _SetSound(Handle,value);
		}

		public bool IsPlaying => _IsPlaying(Handle);

		public SoundState State => (SoundState)_GetState(Handle);

		public void Play()	=> _Play(Handle);
		public void Pause() => _Pause(Handle);
		public void Stop()	=> _Stop(Handle);

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _GetSound(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetSound(IntPtr handle, uint soundClip);
	
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Play(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Pause(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Stop(IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _IsPlaying(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _GetState(IntPtr handle);
		#endregion
	}
}

