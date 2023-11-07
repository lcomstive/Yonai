using Newtonsoft.Json.Linq;
using System;
using System.Runtime.CompilerServices;
using Yonai.IO;

namespace Yonai
{
	public enum SoundState : uint
	{
		Stopped = 0,
		Playing,
		Paused
	}

	public enum AttenuationModel : int
	{
		None,          /* No distance attenuation and no spatialization. */
		Inverse,       /* Equivalent to OpenAL's AL_INVERSE_DISTANCE_CLAMPED. */
		Linear,        /* Linear attenuation. Equivalent to OpenAL's AL_LINEAR_DISTANCE_CLAMPED. */
		Exponential    /* Exponential attenuation. Equivalent to OpenAL's AL_EXPONENT_DISTANCE_CLAMPED. */
	}

	public class AudioSource : Component
	{
		public struct AttenuationSettings : ISerializable
		{
			public Vector2 Distance;
			public Vector2 Gain;
			public float Factor;
			public AttenuationModel Model;

			public void OnDeserialize(JObject json)
			{
				Distance.OnDeserialize(json["Distance"].Value<JObject>());
				Gain.OnDeserialize(json["Gain"].Value<JObject>());
				Factor = json["Factor"].Value<float>();
				Model = (AttenuationModel)json["Model"].Value<int>();
			}

			public JObject OnSerialize() => new JObject(
				new JProperty("Distance", Distance.OnSerialize()),
				new JProperty("Gain", Gain.OnSerialize()),
				new JProperty("Factor", Factor),
				new JProperty("Model", (int)Model)
			);

			public static bool operator ==(AttenuationSettings a, AttenuationSettings b) =>
				a.Distance == b.Distance &&
				a.Gain == b.Gain &&
				a.Factor == b.Factor &&
				a.Model == b.Model;

			public static bool operator !=(AttenuationSettings a, AttenuationSettings b) =>
				a.Distance != b.Distance ||
				a.Gain != b.Gain ||
				a.Factor != b.Factor ||
				a.Model != b.Model;

			public override int GetHashCode() => base.GetHashCode();
			public override bool Equals(object obj) => obj is AttenuationSettings && ((AttenuationSettings)obj) == this;
		}

		private float m_Pitch;
		private AudioData m_Sound;
		private UUID m_SoundID;
		private UUID m_MixerID;
		private float m_Volume;
		private float m_Panning;
		private bool m_IsLooping;
		private bool m_Is3D;
		private AudioMixer m_Mixer;
		private SoundState m_State;
		private AttenuationSettings m_Attenuation;

		public UUID SoundID
		{
			get => m_SoundID;
			set
			{
				if (m_SoundID == value) return; // No change
				_SetSound(Handle, m_SoundID = value);
				m_Sound = m_SoundID == UUID.Invalid ? null : Resource.Get<AudioData>(m_SoundID);
			}
		}

		[Serialize(false)]
		public AudioData Sound
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
				m_Mixer = m_MixerID == UUID.Invalid ? null : Resource.Get<AudioMixer>(m_MixerID);
			}
		}

		[Serialize(false)]
		public AudioMixer Mixer
		{
			get => m_Mixer;
			// Sets mixer ID to desired mixer, or UUID.Invalid if null, which results in output directly to master output
			set => MixerID = value?.ResourceID ?? UUID.Invalid;
		}

		/// <summary>
		/// When enabled, sound is altered based on position in game world relative to listener
		/// </summary>
		public bool Is3D
		{
			get => m_Is3D;
			set { if(m_Is3D != value) _SetIs3D(Handle, m_Is3D = value); }
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

		/// <summary>
		/// The maximum distance this sound can be heard from, when <see cref="Is3D"/> is enabled.
		/// </summary>
		public AttenuationSettings Attenuation
		{
			get => m_Attenuation;
			set
			{
				if (m_Attenuation == value)
					return; // No change

				m_Attenuation = value;

				_SetRolloffRange(Handle, ref m_Attenuation.Distance);
				_SetRolloffGain(Handle, ref m_Attenuation.Gain);
				_SetAttenuationRolloff(Handle, m_Attenuation.Factor);
				_SetAttenuationModel(Handle, (int)m_Attenuation.Model);
			}
		}

		public bool PlayOnStart { get; set; } = true;

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
		[Serialize(false)]
		public SoundState State
		{
			get => m_State;
			set
			{
				if (m_State == value) return; // No change

				switch(value)
				{
					default:
					case SoundState.Stopped: Stop(); break;
					case SoundState.Paused: Pause(); break;
					case SoundState.Playing:
						if (m_State == SoundState.Stopped)
							Play();
						else
							Resume();
						break;
				}
				m_State = value;
			}
		}

		public void Play()	=> _Play(Handle);
		public void Pause() => _Pause(Handle);
		public void Resume()=> _Resume(Handle);
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
			m_Is3D = _GetIs3D(Handle);
			m_State = (SoundState)_GetState(Handle);

			m_Attenuation = new AttenuationSettings();
			_GetRolloffRange(Handle, out m_Attenuation.Distance);
			_GetRolloffGain(Handle, out m_Attenuation.Gain);
			m_Attenuation.Factor = _GetAttenuationRolloff(Handle);
			m_Attenuation.Model = (AttenuationModel)_GetAttenuationModel(Handle);

			m_Sound = m_SoundID == UUID.Invalid ? null : Resource.Get<AudioData>(m_SoundID);
			m_Mixer = m_MixerID == UUID.Invalid ? null : Resource.Get<AudioMixer>(m_MixerID);
		}

		#region Internal Calls
		// Called from unmanaged code
		private void UpdateState(uint newState) => m_State = (SoundState)newState;

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong _GetSound(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetSound(IntPtr handle, ulong soundClip);
	
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Play(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Pause(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Resume(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Stop(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Seek(IntPtr handle, float seconds);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _GetState(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _IsPlaying(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _GetLength(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _GetPlayTime(IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _GetLooping(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetLooping(IntPtr handle, bool value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _GetIs3D(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetIs3D(IntPtr handle, bool value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong _GetMixer(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetMixer(IntPtr handle, ulong value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _GetVolume(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetVolume(IntPtr handle, float value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _GetPanning(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetPanning(IntPtr handle, float value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _GetPitch(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetPitch(IntPtr handle, float value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetRolloffRange(IntPtr handle, out Vector2 range);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetRolloffRange(IntPtr handle, ref Vector2 value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetRolloffGain(IntPtr handle, out Vector2 range);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetRolloffGain(IntPtr handle, ref Vector2 range);
		
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _GetAttenuationRolloff(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetAttenuationRolloff(IntPtr handle, float value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern int _GetAttenuationModel(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetAttenuationModel(IntPtr handle, int value);
		#endregion
	}
}

