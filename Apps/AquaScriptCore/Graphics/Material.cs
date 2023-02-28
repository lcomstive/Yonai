using System;
using System.Runtime.CompilerServices;

namespace AquaEngine.Graphics
{
	public class Material : NativeResourceBase
	{
		private Shader m_Shader = null;
		public Shader Shader
		{
			get => m_Shader;
			set
			{
				// Check for change
				if(m_Shader != value)
					_SetShader(Handle, m_Shader = value);
			}
		}

		private Vector4 m_Albedo = Vector4.One;
		public Vector4 Albedo
		{
			get => m_Albedo;
			set
			{
				// Check for change
				if(m_Albedo != value)
				{
					m_Albedo = value;
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
					_SetAlbedoMap(Handle, (m_AlbedoMap = value).ResourceID);
			}
		}

		private bool m_AlphaClipping = false;
		public bool AlphaClipping
		{
			get => m_AlphaClipping;
			set
			{
				// Check for change
				if(m_AlphaClipping != value)
					_SetAlphaClipping(Handle, m_AlphaClipping = value);
			}
		}

		private float m_AlphaClipThreshold = 0.1f;
		public float AlphaThreshold
		{
			get => m_AlphaClipThreshold;
			set
			{
				// Check for change
				if(m_AlphaClipThreshold != value)
					_SetAlphaClipThreshold(Handle, m_AlphaClipThreshold = value);
			}
		}

		internal override bool Load(string path, params object[] args)
		{
			_Load(path, out uint resourceID, out IntPtr handle);

			Handle = handle;
			ResourceID = resourceID;

			return true;
		}

		internal override void OnLoad()
		{
			_GetAlbedo(Handle, out m_Albedo);
			
			m_AlphaClipping = _GetAlphaClipping(Handle);
			m_Shader = Resource.Get<Shader>(_GetShader(Handle));
			m_AlphaClipThreshold = _GetAlphaClipThreshold(Handle);
			m_AlbedoMap = Resource.Get<Texture>(_GetAlbedoMap(Handle));
		}

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Load(string path, out uint resourceID, out IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _GetShader(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetShader(IntPtr handle, uint value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetAlbedo(IntPtr handle, out Vector4 value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetAlbedo(IntPtr handle, ref Vector4 value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _GetAlbedoMap(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetAlbedoMap(IntPtr handle, uint value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _GetAlphaClipping(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetAlphaClipping(IntPtr handle, bool value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern float _GetAlphaClipThreshold(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetAlphaClipThreshold(IntPtr handle, float value);
	}
}

