using System;
using System.IO;
using System.Runtime.CompilerServices;

namespace AquaEngine.Graphics
{
	public class MaterialImportSettings : IImportSettings
	{
		public UUID Shader = UUID.Invalid;
		public Vector4 Albedo = Vector4.One;
		public UUID AlbedoMap = UUID.Invalid;
		public bool AlphaClipping = false;
		public float AlphaThreshold = 0.1f;
	}

	public class Material : NativeResourceBase
	{
		private MaterialImportSettings m_Settings;

		private Shader m_Shader = null;
		public Shader Shader
		{
			get => m_Shader;
			set
			{
				// Check for change
				if(m_Shader != value)
					_SetShader(Handle, (m_Shader = value)?.ResourceID ?? UUID.Invalid);
			}
		}

		public Vector4 Albedo
		{
			get => m_Settings.Albedo;
			set
			{
				// Check for change
				if(m_Settings.Albedo != value)
				{
					m_Settings.Albedo = value;
					_SetAlbedo(Handle, ref value);
				}
			}
		}

		private Texture m_AlbedoMap = null;
		public Texture AlbedoMap
		{
			get => m_AlbedoMap;
			set
			{
				// Check for change
				if(m_AlbedoMap != value)
					_SetAlbedoMap(Handle, (m_AlbedoMap = value)?.ResourceID ?? UUID.Invalid);
			}
		}

		public bool AlphaClipping
		{
			get => m_Settings.AlphaClipping;
			set
			{
				// Check for change
				if(m_Settings.AlphaClipping != value)
					_SetAlphaClipping(Handle, m_Settings.AlphaClipping = value);
			}
		}

		public float AlphaThreshold
		{
			get => m_Settings.AlphaThreshold;
			set
			{
				// Check for change
				if(m_Settings.AlphaThreshold != value)
					_SetAlphaClipThreshold(Handle, m_Settings.AlphaThreshold = value);
			}
		}

		protected override void OnLoad()
		{
			_Load(ResourcePath, out ulong resourceID, out IntPtr handle);

			Handle = handle;
			ResourceID = resourceID;
		}

		protected override void OnImported()
		{
			if (!TryGetImportSettings(out m_Settings))
				// Reset to default values
				m_Settings = new MaterialImportSettings();

			m_Shader = Resource.Get<Shader>(m_Settings.Shader);
			m_AlbedoMap = Resource.Get<Texture>(m_Settings.AlbedoMap);
		}

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load(string path, out ulong resourceID, out IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong _GetShader(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetShader(IntPtr handle, ulong value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetAlbedo(IntPtr handle, out Vector4 value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetAlbedo(IntPtr handle, ref Vector4 value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong _GetAlbedoMap(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetAlbedoMap(IntPtr handle, ulong value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _GetAlphaClipping(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetAlphaClipping(IntPtr handle, bool value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _GetAlphaClipThreshold(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetAlphaClipThreshold(IntPtr handle, float value);
	}
}

