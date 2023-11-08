using System.Runtime.CompilerServices;

namespace Yonai.Graphics
{
	public static class Graphics
	{
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool _SetAPI(int backend);

		private static void _CreateVertexShader() => _UploadVertexShader(IO.VFS.Read("app://Assets/Shaders/vert.spv"));
		private static void _CreateFragmentShader() => _UploadFragmentShader(IO.VFS.Read("app://Assets/Shaders/frag.spv"));

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _UploadVertexShader(byte[] data);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _UploadFragmentShader(byte[] data);
	}
}