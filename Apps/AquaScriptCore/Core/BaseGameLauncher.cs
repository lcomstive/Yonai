using System;
using System.Linq;
using AquaEngine.IO;
using Newtonsoft.Json;
using AquaEngine.Systems;
using Newtonsoft.Json.Linq;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	internal static class BaseGameLauncher
	{
		private static JObject JSON = null;

		private const string AssemblyDir = "build://Assemblies/";

		private static JObject LoadJSON(string path) => JsonConvert.DeserializeObject<JObject>(VFS.ReadText(path));

		internal static void Launch()
		{
			VFS.Mount("assets://", "build://Editor");
			VFS.Mount("assets://", "build://ProjectFiles");
			VFS.Mount("project://Assets/", "build://ProjectFiles");

			JSON = LoadJSON("build://launch.json");

			LoadBaseSystems();

			if(JSON.ContainsKey("Assemblies"))
				LoadAssemblies();

			Resource.LoadDatabase("build://resources.json");

			if (JSON.ContainsKey("GlobalSystems") && JSON["GlobalSystems"].GetType() == typeof(JArray))
				LoadGlobalSystems();

			if (JSON.ContainsKey("Scenes") && JSON["Scenes"].GetType() == typeof(JArray))
				LoadInitialScene();

			Log.Debug("Loaded Worlds:");
			foreach (World world in World.GetAll())
				Log.Debug(" - " + world.Name);
		}

		private static void LoadAssemblies()
		{
			foreach(string path in JSON["Assemblies"].Values<string>())
				_LoadAssembly(AssemblyDir + path);
		}

		private static void LoadInitialScene()
		{
			string scenePath = JSON["Scenes"].Values<string>().FirstOrDefault();
			World initialWorld = World.Create(VFS.ReadJSON(scenePath));
			if (initialWorld && initialWorld.ID != UUID.Invalid)
				SceneManager.Load(initialWorld);
			else
				Log.Warning($"Failed to load initial scene '{scenePath}'");
		}

		private static void LoadBaseSystems()
		{
			AquaSystem.Add<AudioSystem>();
			AquaSystem.Add<BehaviourSystem>();
		}

		private static void LoadGlobalSystems()
		{
			Type aquaSystemType = typeof(AquaSystem);
			foreach(string className in JSON["GlobalSystems"])
			{
				Log.Trace($"Loading global system '{className}'");
				Type systemType = Type.GetType(className);
				if (systemType != null && aquaSystemType.IsAssignableFrom(systemType))
					AquaSystem.Add(systemType);
				else
					Log.Warning($"Could not find AquaSystem type '{className}'");
			}
		}

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern bool _LoadAssembly(string path);
	}
}