using System;
using Yonai.Graphics;
using Yonai.Graphics.Backends;
using Yonai.Graphics.Backends.Vulkan;

namespace Yonai.Systems
{
	public class RenderSystem : YonaiSystem
	{
		public static IGraphicsBackend Backend { get; private set; } = null;

		public static GraphicsAPI API { get; private set; }

		public override bool IsGlobal => true;

		protected override void Draw()
		{
			/*
			World[] worlds = SceneManager.GetActiveScenes();
			foreach(World world in worlds)
			{
				Camera[] cameras = world.GetComponents<Camera>();
				foreach (Camera camera in cameras)
					Draw(camera);
			}
			*/

			try { Backend.Draw(); }
			catch(Exception e) { Log.Exception(e); }
		}

		private void Draw(Camera camera)
		{

		}

		public static void ChangeBackend(GraphicsAPI backend)
		{
			if (API == backend) return;
			API = backend;

			Log.Trace("Graphics API set to " + Enum.GetName(typeof(GraphicsAPI), backend));

			Backend?.Destroy();
			Backend = null;

			API = backend;

			switch (API)
			{
				default:
				case GraphicsAPI.None: return;
				case GraphicsAPI.Vulkan: Backend = new VulkanGraphicsBackend(); break;
			}

			try { Backend?.Create(); }
			catch(Exception e) { Log.Exception(e); }
		}
	}
}