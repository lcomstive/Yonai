using AquaEngine;
using System.Linq;
using System.Collections.Generic;

namespace AquaEditor.BuildProcess
{
	public static class GameBuilder
	{
		/// <summary>
		/// The <see cref="IBuildProcess"/> associated with the build <see cref="Platform"/>
		/// </summary>
		private static Dictionary<Platform, IBuildProcess> BuildProcesses = new Dictionary<Platform, IBuildProcess>()
		{
			{ Platform.Windows, new WindowsBuildProcess() }
		};

		/// <summary>
		/// What <see cref="Platform"/>s the host <see cref="Platform"/> can build for (e.g. Windows -> [ Windows, Android ])
		/// </summary>
		private static Dictionary<Platform, Platform[]> BuildPlatformMatrix = new Dictionary<Platform, Platform[]>()
		{
			{ Platform.Windows, new Platform[] { Platform.Windows } }
		};

		/// <summary>
		/// Maps a <see cref="Platform"/> to the <see cref="AquaEngine.IO.VFS"/> filepath of icon texture
		/// </summary>
		private static Dictionary<Platform, string> PlatformIcons = new Dictionary<Platform, string>()
		{
			{ Platform.Windows, "Windows" }
		};

		public static Platform ActivePlatform { get; private set; } = Platform.Unknown;
		public static IBuildProcess BuildProcess { get; private set; } = null;
		public static Platform[] AvailableBuildPlatforms => BuildPlatformMatrix[Application.Platform];

		/// <returns>True if <see cref="ActivePlatform"/> was set to <paramref name="platform"/></returns>
		public static bool SelectPlatform(Platform platform)
		{
			if (platform == ActivePlatform)
				return true; // No change

			Platform hostPlatform = Application.Platform;

			// Check this host can build for desired platform
			if (!BuildPlatformMatrix[hostPlatform].Contains(platform))
			{
				Log.Error($"Cannot select platform '{platform}' for host '{hostPlatform}' - not supported");
				return false;
			}

			ActivePlatform = platform;
			BuildProcess = BuildProcesses[platform];
			return true;
		}

		/// <summary>
		/// Changes to the new platform, starts the build process, and changes back to the previous platform
		/// </summary>
		/// <param name="platform"></param>
		public static void StartBuild(Platform platform)
		{
			Platform previousPlatform = ActivePlatform;
			if (!SelectPlatform(platform))
				return;

			StartBuild(platform);
			SelectPlatform(previousPlatform);
		}

		public static void StartBuild()
		{
			ProjectFile project = AquaSystem.Get<EditorService>().Project;
			string outputFolder = FileDialog.OpenFolder("Build Output", "project://");
			if (!string.IsNullOrEmpty(outputFolder))
				BuildProcess.Execute(outputFolder, project);
		}

		internal static void Initialise()
		{
			// Set platform to host platform by default
			// TODO: Cache selected platform somewhere in a local user settings file?
			SelectPlatform(Application.Platform);
		}

		internal static void ConstructBuildMenu()
		{
			EditorUIService editorUI = AquaSystem.Get<EditorUIService>();
			editorUI.AddMenuItemDirectory("Build");

			foreach (Platform platform in AvailableBuildPlatforms)
				editorUI.AddMenuItem(() => SelectPlatform(platform), new MenuItemAttribute($"Build/Platform/{platform}") { Icon = PlatformIcons[platform] });

			editorUI.AddMenuItem(StartBuild, new MenuItemAttribute("Build/Start Build") { PrependSeparator = true, Shortcut = "CTRL+SHIFT+B" });
		}
	}
}