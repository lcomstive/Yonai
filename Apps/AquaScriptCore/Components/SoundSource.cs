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

	public class SoundSource : Component
	{
		public UUID Sound
		{
			get => _GetSound(Handle);
			set => _SetSound(Handle, value);
		}

		public UUID MixerID
		{
			get => _GetMixer(Handle);
			set => _SetMixer(Handle, value);
		}

		[Serialize(false)]
		public SoundMixer Mixer
		{
			get
			{
				UUID mixerID = MixerID;
				return mixerID == UUID.Invalid ? null : Resource.Get<SoundMixer>(mixerID);
			}

			// Sets mixer ID to desired mixer, or UUID.Invalid if null, which results in output directly to master output
			set => MixerID = value?.ResourceID ?? UUID.Invalid;
		}

		/// <summary>
		/// When enabled, sound is altered based on position in game world relative to listener
		/// </summary>
		public bool Spatialize
		{
			get => _GetSpatialization(Handle);
			set => _SetSpatialization(Handle, value);
		}

		/// <summary>
		/// Sets output volume of audio
		/// </summary>
		public float Volume
		{
			get => _GetVolume(Handle);
			set => _SetVolume(Handle, value);
		}

		/// <summary>
		/// Sets output of audio to left and right, ranging [-1, 1.0] respectively
		/// </summary>
		[Range(-1, 1)]
		public float Panning
		{
			get => _GetPanning(Handle);
			set => _SetPanning(Handle, value);
		}

		/// <summary>
		/// Changes pitch of the played sound. Higher is higher frequency, minimum 0.
		/// </summary>
		public float Pitch
		{
			get => _GetPitch(Handle);
			set => _SetPitch(Handle, value);
		}

		public bool IsPlaying => _IsPlaying(Handle);

		/// <summary>
		/// Time played of <see cref="Sound"/>, in seconds
		/// </summary>
		public float PlayTime => _GetPlayTime(Handle);

		/// <summary>
		/// Total length of <see cref="Sound"/>, in seconds
		/// </summary>
		public float Length => _GetLength(Handle);

		/// <summary>
		/// When true, continues playing sound from beginning if end of clip is reached
		/// </summary>
		public bool IsLooping
		{
			get => _GetLooping(Handle);
			set => _SetLooping(Handle, value);
		}

		/// <summary>
		/// Current play state of the sound source
		/// </summary>
		public SoundState State => (SoundState)_GetState(Handle);

		public void Play()	=> _Play(Handle);
		public void Pause() => _Pause(Handle);
		public void Stop()	=> _Stop(Handle);
		public void Seek(float seconds) => _Seek(Handle, seconds);

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong _GetSound(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetSound(IntPtr handle, ulong soundClip);
	
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Play(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Pause(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Stop(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Seek(IntPtr handle, float seconds);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _GetState(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _IsPlaying(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _GetLength(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _GetPlayTime(IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _GetLooping(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetLooping(IntPtr handle, bool value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _GetSpatialization(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetSpatialization(IntPtr handle, bool value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong _GetMixer(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetMixer(IntPtr handle, ulong value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _GetVolume(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetVolume(IntPtr handle, float value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _GetPanning(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetPanning(IntPtr handle, float value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _GetPitch(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetPitch(IntPtr handle, float value);
		#endregion
	}
}

