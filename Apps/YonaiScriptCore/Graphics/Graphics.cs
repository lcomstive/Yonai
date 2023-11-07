using System.Runtime.CompilerServices;

namespace Yonai.Graphics
{
	public enum GraphicsAPI : int
	{
		None = 0,
		Vulkan,
		// DirectX
	}

	public static class Graphics
	{

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool _SetAPI(int backend);
	}
}