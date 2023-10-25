using Yonai.IO;
using Newtonsoft.Json.Linq;
using System;
using System.Runtime.CompilerServices;

namespace Yonai
{
	public struct SoundMixerImportSettings : IImportSettings
	{
		public string Name;
		public float Volume;
		public UUID ParentMixer;
	}

	public class AudioMixer : NativeResourceBase, ISerializable
	{
		private string m_Name;
		private float m_Volume;
		private AudioMixer m_ParentMixer;
		private UUID m_ParentMixerID = UUID.Invalid;

		public string Name
		{
			get => m_Name;
			set
			{
				if(m_Name != value)
					_SetName(Handle, m_Name = value);
			}
		}

		public float Volume
		{
			get => m_Volume;
			set
			{
				if(m_Volume != value)
					_SetVolume(Handle, m_Volume = value);
			}
		}

		public UUID ParentMixerID
		{
			get => m_ParentMixerID;
			set
			{
				if (m_ParentMixerID == value)
					return; // No change

				_SetParent(Handle, m_ParentMixerID = value);
				ParentMixer = ParentMixerID == UUID.Invalid ? null : Resource.Get<AudioMixer>(ParentMixerID);
			}
		}

		public AudioMixer ParentMixer
		{
			get => m_ParentMixer;
			set => ParentMixerID = value?.ResourceID ?? UUID.Invalid;
		}

		protected override void OnLoad()
		{
			ulong resourceID = ResourceID;
			_Load(ResourcePath, out resourceID, out IntPtr handle);

			ResourceID = resourceID;
			Handle = handle;

			m_Name = _GetName(Handle);
			m_Volume = _GetVolume(Handle);
			m_ParentMixerID = _GetParent(Handle);
		}

		protected override void OnImported()
		{
			if (!TryGetImportSettings(out SoundMixerImportSettings settings))
				return;

			m_Name = settings.Name;
			m_Volume = settings.Volume;
			m_ParentMixerID = settings.ParentMixer;
			m_ParentMixer = ParentMixerID == UUID.Invalid ? null : Resource.Get<AudioMixer>(ParentMixerID);
		}

		public JObject OnSerialize() => new JObject(
				new JProperty("Name", Name),
				new JProperty("Volume", Volume),
				new JProperty("ParentMixer", ParentMixerID.ToString())
			);

		public void OnDeserialize(JObject json) => Import(new SoundMixerImportSettings()
		{
			Name		= json["Name"].Value<string>(),
			Volume		= json["Volume"].Value<float>(),
			ParentMixer = ulong.Parse(json["ParentMixer"].Value<string>())
		});

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load(string path, out ulong resourceID, out IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _GetVolume(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetVolume(IntPtr handle, float value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string _GetName(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetName(IntPtr handle, string value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong _GetParent(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetParent(IntPtr handle, ulong value);
		#endregion
	}
}
