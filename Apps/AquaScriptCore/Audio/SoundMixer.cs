using AquaEngine.IO;
using Newtonsoft.Json.Linq;
using System;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public struct SoundMixerImportSettings : IImportSettings
	{
		public string Name;
		public float Volume;
		public UUID ParentMixer;
	}

	public class SoundMixer : NativeResourceBase, ISerializable
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

		public UUID ParentMixerID
		{
			get => _GetParent(Handle);
			set => _SetParent(Handle, value);
		}

		public SoundMixer ParentMixer
		{
			get => Resource.Get<SoundMixer>(ParentMixerID);
			set => ParentMixerID = value?.ResourceID ?? UUID.Invalid;
		}

		protected override void OnLoad()
		{
			ulong resourceID = ResourceID;
			_Load(ResourcePath, out resourceID, out IntPtr handle);

			ResourceID = resourceID;
			Handle = handle;
		}

		protected override void OnImported()
		{
			if (!TryGetImportSettings(out SoundMixerImportSettings settings))
				return;

			Name = settings.Name;
			Volume = settings.Volume;
			ParentMixerID = settings.ParentMixer;
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
