using System;
using System.Runtime.CompilerServices;

[assembly: InternalsVisibleTo("AquaScriptEditor")]
namespace AquaEngine.Graphics
{
	public class RenderTexture : IDisposable
	{
		private IVector2 m_Resolution = IVector2.Zero;
		public IVector2 Resolution
		{
			get => m_Resolution;
			set
			{
				m_Resolution = value;
				_SetResolution(Handle, ref m_Resolution);
			}
		}

		private uint m_Samples;
		public uint Samples
		{
			get => m_Samples;
			set => _SetSamples(Handle, m_Samples = value);
		}

		private TextureFormat m_Format = TextureFormat.None;
		public TextureFormat Format
		{
			get => m_Format;
			set => _SetFormat(Handle, (byte)(m_Format = value));
		}

		internal IntPtr Handle { get; private set; }
		private bool m_CreatedResource = false;

		public RenderTexture(IVector2 resolution, TextureFormat format = TextureFormat.RGBA8, uint samples = 1)
		{
			m_Resolution = resolution;
			m_Format = format;
			m_Samples = samples;
			m_CreatedResource = true;

			Handle = _Load(
				ref m_Resolution,
				(byte)m_Format,
				m_Samples
				);
		}

		internal RenderTexture(IntPtr handle)
		{
			Handle = handle;
			m_CreatedResource = false;

			m_Format = (TextureFormat)_GetFormat(Handle);
			m_Samples = _GetSamples(Handle);
			_GetResolution(Handle, ref m_Resolution);
		}

		public void Dispose()
		{
			Log.Warning("RenderTexture disposed");

			// if _Load is called memory is allocated, _Destroy frees that memory.
			// if RenderTexture(IntPtr) constructor is called instead, we don't manage this memory.
			if (m_CreatedResource && Handle != IntPtr.Zero)
			{
				_Destroy(Handle);
				Handle = IntPtr.Zero;
			}
		}

		public void Bind(uint textureIndex = 0) => _Bind(Handle, textureIndex);
		public void Unbind() => _Unbind(Handle);

		public void CopyTo(RenderTexture destination)
		{
			if (destination != null)
				_CopyTo(Handle, destination.Handle);
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private extern static IntPtr _Load(ref IVector2 resolution, byte textureFormat, uint samples);
		[MethodImpl(MethodImplOptions.InternalCall)] private extern static void _Destroy(IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private extern static void _Bind(IntPtr handle, uint index);
		[MethodImpl(MethodImplOptions.InternalCall)] private extern static void _Unbind(IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private extern static void _CopyTo(IntPtr handle, IntPtr destination);

		[MethodImpl(MethodImplOptions.InternalCall)] private extern static void _SetFormat(IntPtr handle, byte format);
		[MethodImpl(MethodImplOptions.InternalCall)] private extern static void _SetSamples(IntPtr handle, uint samples);
		[MethodImpl(MethodImplOptions.InternalCall)] private extern static void _SetResolution(IntPtr handle, ref IVector2 resolution);

		[MethodImpl(MethodImplOptions.InternalCall)] private extern static byte _GetFormat(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private extern static uint _GetSamples(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private extern static void _GetResolution(IntPtr handle, ref IVector2 resolution);
		#endregion
	}
}
