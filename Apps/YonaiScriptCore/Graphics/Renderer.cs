using Yonai.Graphics.Pipelines;
using System;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics
{
	public enum DrawMode : byte 
	{
		Points,
		Lines,
		LineStrip,
		Triangles,
		TriangleStrip,
		TriangleFan,
		Quads,
		QuadStrip
	}

	public enum TextureFormat : byte
	{
		None = 0,

		// Colour
		RGBA8,
		RGBA16,
		RGBA16F,
		RedInteger,

		Cubemap,

		// Depth & Stencil
		Depth24Stencil8,

		// Aliases
		Depth = Depth24Stencil8
	}

	[Flags]
	public enum BufferFlags : uint
	{
		Colour,
		Depth
	}

	public static class Renderer
	{
		private static NativeRenderPipeline m_RenderPipeline = null;
		public static IRenderPipeline Pipeline
		{
			get
			{
				IntPtr handle = _GetPipeline();

				// Check if was set previously but now cleared
				if (handle == IntPtr.Zero && m_RenderPipeline != null)
					m_RenderPipeline = null;

				// Cache pipeline
				if (m_RenderPipeline == null && handle != IntPtr.Zero)
					m_RenderPipeline = new NativeRenderPipeline(handle);

				return m_RenderPipeline;
			}
		}

		public static void Draw() => _Draw();
		public static void Draw(Camera camera) => _Draw(camera.World.ID, camera.Entity.ID);

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Draw();
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _Draw(ulong worldID, ulong entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern IntPtr _GetPipeline();
		#endregion
	}
}

