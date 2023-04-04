using System;
using System.Runtime.CompilerServices;

namespace AquaEngine.Graphics.Pipelines
{
	public class NativeRenderPipeline : IRenderPipeline
	{
		public IVector2 Resolution
		{
			get
			{
				_GetResolution(Handle, out IVector2 resolution);
				return resolution;
			}
			set => _SetResolution(Handle, ref value);
		}

		private Framebuffer m_Framebuffer = null;
		public Framebuffer Output
		{
			get
			{
				IntPtr outputHandle = _GetOutput(Handle);

				// Check if framebuffer handle previously existed, but has changed
				if (m_Framebuffer != null && outputHandle != m_Framebuffer.Handle)
				{
					m_Framebuffer.Dispose();
					m_Framebuffer = null;
				}

				// Cache framebuffer
				if (m_Framebuffer == null && outputHandle != IntPtr.Zero)
					m_Framebuffer = new Framebuffer(outputHandle);

				return m_Framebuffer;
			}
		}

		internal IntPtr Handle;

		internal NativeRenderPipeline(IntPtr handle) => Handle = handle;

		public void Draw(Camera camera) =>
			_Draw(Handle, camera.World.ID, camera.Entity.ID);

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetResolution(IntPtr handle, out IVector2 resolution);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetResolution(IntPtr handle, ref IVector2 resolution);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Draw(IntPtr handle, ulong cameraWorldID, ulong cameraEntityID);

		// Returns handle to framebuffer
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern IntPtr _GetOutput(IntPtr handle);
		#endregion
	}
}
