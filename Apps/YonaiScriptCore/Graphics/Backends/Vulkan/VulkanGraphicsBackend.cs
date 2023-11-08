using System.Runtime.CompilerServices;

namespace Yonai.Graphics.Backends.Vulkan
{
	public class VulkanGraphicsBackend : IGraphicsBackend
	{
		public VulkanInstance Instance { get; private set; }

		public string[] AvailableExtensions { get; private set; }

		public void Create()
		{
			Log.Trace("Creating vulkan graphics backend");

			Instance = new VulkanInstance("Yonai Application", new Version(0, 0, 1));

			AvailableExtensions = _GetAvailableExtensions();
			Log.Debug($"Available extensions: ({AvailableExtensions.Length})");
			foreach (string extension in AvailableExtensions)
				Log.Debug($" - {extension}");
		}

		public void Destroy()
		{
			Log.Trace("Destroying vulkan graphics backend");

			Instance?.Dispose();
		}

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern string[] _GetAvailableExtensions();

	}
}