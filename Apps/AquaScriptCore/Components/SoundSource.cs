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
		private float m_Pitch;
		private Sound m_Sound;
		private UUID m_SoundID;
		private UUID m_MixerID;
		private float m_Volume;
		private float m_Panning;
		private bool m_IsLooping;
		private bool m_Spatialise;
		private SoundMixer m_Mixer;
		private SoundState m_State;

		public UUID SoundID
		{
			get => m_SoundID;
			set
			{
				if (m_SoundID == value) return; // No change
				_SetSound(Handle, m_SoundID = value);
				m_Sound = m_SoundID == UUID.Invalid ? null : Resource.Get<Sound>(m_SoundID);
			}
		}

		public Sound Sound
		{
			get => m_Sound;
			set => SoundID = value?.ResourceID ?? UUID.Invalid;
		}

		public UUID MixerID
		{
			get => m_MixerID;
			set
			{
				if (m_MixerID == value) return; // No change
				_SetMixer(Handle, m_MixerID = value);
				m_Mixer = m_MixerID == UUID.Invalid ? null : Resource.Get<SoundMixer>(m_MixerID);
			}
		}

		[Serialize(false)]
		public SoundMixer Mixer
		{
			get => m_Mixer;
			// Sets mixer ID to desired mixer, or UUID.Invalid if null, which results in output directly to master output
			set => MixerID = value?.ResourceID ?? UUID.Invalid;
		}

		/// <summary>
		/// When enabled, sound is altered based on position in game world relative to listener
		/// </summary>
		public bool Spatialise
		{
			get => m_Spatialise;
			set { if(m_Spatialise != value) _SetSpatialization(Handle, m_Spatialise = value); }
		}

		/// <summary>
		/// Sets output volume of audio
		/// </summary>
		public float Volume
		{
			get => m_Volume;
			set { if (m_Volume != value) _SetVolume(Handle, m_Volume = value); }
		}

		/// <summary>
		/// Sets output of audio to left and right, ranging [-1, 1.0] respectively
		/// </summary>
		[Range(-1, 1)]
		public float Panning
		{
			get => m_Panning;
			set { if (m_Panning != value) _SetPanning(Handle, m_Panning = value); }
		}

		/// <summary>
		/// Changes pitch of the played sound. Higher is higher frequency, minimum 0.
		/// </summary>
		public float Pitch
		{
			get => m_Pitch;
			set { if (m_Pitch != value) _SetPitch(Handle, m_Pitch = value); }
		}

		public bool IsPlaying => _IsPlaying(Handle);

		/// <summary>
		/// Time played of <see cref="SoundID"/>, in seconds
		/// </summary>
		public float PlayTime => _GetPlayTime(Handle);

		/// <summary>
		/// Total length of <see cref="SoundID"/>, in seconds
		/// </summary>
		public float Length => _GetLength(Handle);

		/// <summary>
		/// When true, continues playing sound from beginning if end of clip is reached
		/// </summary>
		public bool IsLooping
		{
			get => m_IsLooping;
			set { if (m_IsLooping != value) _SetLooping(Handle, m_IsLooping = value); }
		}

		/// <summary>
		/// Current play state of the sound source
		/// </summary>
		public SoundState State
		{
			get => m_State;
			set
			{
				if (m_State == value) return; // No change

				switch(m_State = value)
				{
					default:
					case SoundState.Stopped: Stop(); break;
					case SoundState.Playing: Play(); break;
					case SoundState.Paused: Pause(); break;
				}
			}
		}

		public void Play()	=> _Play(Handle);
		public void Pause() => _Pause(Handle);
		public void Stop()	=> _Stop(Handle);
		public void Seek(float seconds) => _Seek(Handle, seconds);

		protected internal override void OnNativeHandleSet()
		{
			// Get all the initial values from the native component
			m_Pitch = _GetPitch(Handle);
			m_SoundID = _GetSound(Handle);
			m_MixerID = _GetMixer(Handle);
			m_Volume = _GetVolume(Handle);
			m_Panning =	_GetPanning(Handle);
			m_IsLooping = _GetLooping(Handle);
			m_State = (SoundState)_GetState(Handle);
			m_Spatialise = _GetSpatialization(Handle);

			m_Sound = m_SoundID == UUID.Invalid ? null : Resource.Get<Sound>(m_SoundID);
			m_Mixer = m_MixerID == UUID.Invalid ? null : Resource.Get<SoundMixer>(m_MixerID);
		}

		#region Internal Calls
		// Called from unmanaged code
		private void UpdateState(uint newState)
		{
			if (newState != (uint)m_State)
				m_State = (SoundState)newState;
		}

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

