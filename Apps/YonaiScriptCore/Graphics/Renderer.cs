using Yonai.Graphics.Pipelines;
using System;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics
{
	public enum DrawMode : byte 
	{
		Points,
		Lines,
		LineLoop,
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
		Colour	= 0x00004000,
		Depth	= 0x00000100,
		Stencil	= 0x00000400
	}

	public enum RenderFeature : uint
	{
		AlphaTest = 0x0BC0,
		Blend = 0x0BE2,
		CullFace = 0x0B44,
		DepthTest = 0x0B71,
		LineSmooth = 0x0B20,
		ScissorTest = 0x0C11,
		StencilTest = 0x82AD
	}

	public enum CullFace : uint
	{
		Front = 0x0404,
		Back = 0x0405,
		FrontAndBack = 0x0408
	}

	public enum FillMode : uint
	{
		Point = 0x1B00,
		Line  = 0x1B01,
		Fill  = 0x1B02
	}

	public static class Renderer
	{
		public static IRenderPipeline Pipeline { get; private set; }

		public static void SetPipeline(IRenderPipeline pipeline)
		{
			Pipeline?.Dispose();
			Pipeline = pipeline;
		}

		public static void ClearColour(Colour colour) => _ClearColour(ref colour);
		public static void ClearColour(float r, float g, float b, float a = 1.0f) =>
			ClearColour(new Colour(r, g, b, a));

		public static void Clear(BufferFlags flags = BufferFlags.Colour | BufferFlags.Depth) =>
			_Clear((uint)flags);

		public static void SetViewport(IVector4 viewport) => _SetViewport(ref viewport);
		public static void SetViewport(IVector2 offset, IVector2 resolution) => SetViewport(new IVector4(offset.x, offset.y, resolution.x, resolution.y));
		public static void SetViewport(int x, int y, int width, int height) =>
			SetViewport(new IVector4(x, y, width, height));

		public static void DrawMesh(Mesh mesh)
		{
			if(mesh != null)
				_DrawMesh(mesh.GetHandle());
		}

		/// <summary>
		/// Uploads camera data to to the shader.
		/// <paramref name="resolution"/> is used to generate projection matrix.
		/// </summary>
		public static void UploadCamera(Shader shader, Camera camera, Vector2 resolution) =>
			_UploadCamera(shader.GetHandle(), camera.Handle, ref resolution);

		public static void UploadTransform(Shader shader, Transform transform) =>
			_UploadTransform(shader.GetHandle(), transform.Handle);

		public static void Enable(RenderFeature capability) =>
			_Enable((uint)capability);

		public static void Disable(RenderFeature capability) =>
			_Disable((uint)capability);

		public static void CullFace(CullFace face) =>
			_CullFace((uint)face);

		public static void BeginDebugGroup(string name) =>
			_BeginDebugGroup(name);

		public static void EndDebugGroup() =>
			_EndDebugGroup();

		public static void SetFill(FillMode mode) =>
			_PolygonMode((uint)mode);

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _ClearColour(ref Colour colour);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _SetViewport(ref IVector4 viewport);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _DrawMesh(IntPtr meshHandle);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _UploadCamera(IntPtr shader, IntPtr camera, ref Vector2 resolution);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _UploadTransform(IntPtr shader, IntPtr transform);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Clear(uint flags);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Enable(uint flag);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _Disable(uint flag);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _CullFace(uint flag);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _BeginDebugGroup(string name);

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _EndDebugGroup();

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void _PolygonMode(uint mode);
		#endregion
	}
}

