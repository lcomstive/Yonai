using System;
using System.Linq;
using Yonai.IO;
using Newtonsoft.Json;
using Yonai.Systems;
using Newtonsoft.Json.Linq;
using System.Runtime.CompilerServices;
using System.Reflection;
using Yonai.Graphics;

namespace Yonai
{
	internal static class BaseGameLauncher
	{
		private static JObject JSON = null;

		private const string AssemblyDir = "build://Assemblies/";

		private static JObject LoadJSON(string path) => JsonConvert.DeserializeObject<JObject>(VFS.ReadText(path));

		// Called from unmanaged code
		private static void Launch()
		{
			VFS.Mount("build://", Application.ExecutableDirectory + "/Assets");
			VFS.Mount("assets://", "build://Editor");
			VFS.Mount("assets://", "build://ProjectFiles");
			VFS.Mount("project://Assets/", "build://ProjectFiles");

			JSON = LoadJSON("build://Launch.json");

			LoadBaseSystems();

			if(JSON.ContainsKey("Assemblies"))
				LoadAssemblies();

			Resource.LoadDatabase("build://Resources.json");

			if (JSON.ContainsKey("GlobalSystems") && JSON["GlobalSystems"].GetType() == typeof(JArray))
				LoadGlobalSystems();

			LoadInitialScene();

			// TODO: Change render pipeline type based on project settings
			Renderer.SetPipeline(new Graphics.Pipelines.ForwardRenderPipeline());
		}

		private static void LoadAssemblies()
		{
			foreach (string path in JSON["Assemblies"].Values<string>())
			{
				VFSFile file = new VFSFile(path);
				_LoadAssembly(VFS.Read(AssemblyDir + path), file.FileNameWithoutExtension);
			}
		}

		private static void LoadInitialScene()
		{
			string scenePath = JSON["InitialScene"]?.Value<string>();
			if(string.IsNullOrEmpty(scenePath))
			{
				Log.Error("No initial scene is available");
				Application.Exit();
				return;
			}
			else
				Log.Trace($"Loading initial scene '{scenePath}'");

			if(!VFS.Exists(scenePath))
			{
				Log.Error($"Initial scene '{scenePath}' could not be found in the filesystem");
				Application.Exit();
				return;
			}

			World world = Resource.Load<World>(scenePath);
			SceneManager.Load(world);

			SpriteRenderer[] spriteRenderers = world.GetComponents<SpriteRenderer>();
			Log.Warning("Sprite renderers: " + spriteRenderers.Length);
		}

		private static void LoadBaseSystems()
		{
			YonaiSystem.Add<AudioSystem>();
			YonaiSystem.Add<RenderSystem>();
			YonaiSystem.Add<BehaviourSystem>();
		}

		private static void LoadGlobalSystems()
		{
			Type yonaiSystemType = typeof(YonaiSystem);
			foreach(string className in JSON["GlobalSystems"])
			{
				Log.Trace($"Loading global system '{className}'");
				Type systemType = Type.GetType(className);
				if (systemType != null && yonaiSystemType.IsAssignableFrom(systemType))
					YonaiSystem.Add(systemType).Enable(false);
				else
					Log.Warning($"Could not find YonaiSystem type '{className}'");
			}
		}

		// Copy renderer output to window.
		// Called from C++
		private static void _PostDraw()
		{
			if(!YonaiSystem.Has<RenderSystem>()) return;

			IRenderPipeline renderPipeline = Renderer.Pipeline;
			if(renderPipeline == null)
			{
				Log.Critical("Render system present but no render pipeline has been set?");
				Application.Exit();
				return;
			}

			renderPipeline.Output.BlitTo(null);
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool _LoadAssembly(byte[] data, string friendlyName);
	}
}