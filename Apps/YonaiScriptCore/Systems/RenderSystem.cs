using Newtonsoft.Json.Linq;
using System;
using System.Threading;
using Yonai.Graphics;
using Yonai.Graphics.Backends;
using Yonai.Graphics.Backends.Vulkan;

namespace Yonai.Systems
{
	public class RenderSystem : YonaiSystem
	{
		private IGraphicsBackend m_Backend { get; set; } = null;

		private GraphicsAPI m_API = GraphicsAPI.None;
		public GraphicsAPI API
		{
			get => m_API;
			set => SetBackend(value);
		}

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

			try { m_Backend.Draw(); }
			catch(Exception e) { Log.Exception(e); }
		}

		private void Draw(Camera camera)
		{

		}

		private void SetBackend(GraphicsAPI backend)
		{
			if (m_API == backend) return;

			Log.Trace("Graphics API set to " + Enum.GetName(typeof(GraphicsAPI), backend));

			m_Backend?.Destroy();
			m_Backend = null;

			m_API = backend;

			switch (m_API)
			{
				default:
				case GraphicsAPI.None: return;
				case GraphicsAPI.Vulkan: m_Backend = new VulkanGraphicsBackend(); break;
			}

			try { m_Backend?.Create(); }
			catch(Exception e) { Log.Exception(e); }
		}

		/// <summary>
		/// Called from C++ to get current API
		/// </summary>
		private static int GetGraphicsAPI() => (int)Get<RenderSystem>().m_API;
	}
}