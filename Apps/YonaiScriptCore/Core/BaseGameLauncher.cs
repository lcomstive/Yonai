using System;
using Yonai.IO;
using System.Linq;
using Yonai.Systems;
using Yonai.Graphics;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.Runtime.CompilerServices;

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

			GraphicsAPI graphicsAPI = GraphicsAPI.Vulkan;
			if (!Application.HasArg("GraphicsAPI") || !Enum.TryParse(Application.GetArg("GraphicsAPI"), out graphicsAPI))
				if (JSON.ContainsKey("GraphicsAPI"))
					Enum.TryParse(JSON["GraphicsAPI"].Value<string>(), out graphicsAPI);
			YonaiSystem.Add<RenderSystem>().API = graphicsAPI;

			if (JSON.ContainsKey("GlobalSystems") && JSON["GlobalSystems"].GetType() == typeof(JArray))
				LoadGlobalSystems();

			if (JSON.ContainsKey("Scenes") && JSON["Scenes"].GetType() == typeof(JArray))
				LoadInitialScene();

			Window._Show();
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
			string scenePath = JSON["Scenes"].Values<string>().FirstOrDefault();
			/*
			World initialWorld = World.Create(VFS.ReadJSON(scenePath));
			if (initialWorld && initialWorld.ID != UUID.Invalid)
				SceneManager.Load(initialWorld);
			else
				Log.Warning($"Failed to load initial scene '{scenePath}'");
			*/
			World world = Resource.Load<World>(scenePath);
			if (world)
				SceneManager.Load(world);
		}

		private static void LoadBaseSystems()
		{
			YonaiSystem.Add<AudioSystem>();
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

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _LoadAssembly(byte[] data, string friendlyName);
	}
}