using System;
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

		/// <returns>True if this host platform can build for desired platform <paramref name="platform"/></returns>
		public static bool IsValidBuildPlatform(Platform platform) => AvailableBuildPlatforms.Contains(platform);

		/// <returns>True if <see cref="ActivePlatform"/> was set to <paramref name="platform"/></returns>
		public static bool SelectPlatform(Platform platform)
		{
			if (platform == ActivePlatform)
				return true; // No change

			// Check this host can build for desired platform
			if (!IsValidBuildPlatform(platform))
			{
				Log.Error($"Cannot select platform '{platform}' for host '{Application.Platform}' - not supported");
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
		public static void StartBuild(Platform platform, string outputFolder = null)
		{
			Platform previousPlatform = ActivePlatform;
			if (!SelectPlatform(platform))
				return;

			StartBuild(platform, outputFolder);
			SelectPlatform(previousPlatform);
		}

		public static void StartBuild(string outputFolder = null)
		{
			ProjectFile project = AquaSystem.Get<EditorService>().Project;
			if (string.IsNullOrEmpty(outputFolder))
				outputFolder = FileDialog.OpenFolder("Build Output", "project://");
			if (!string.IsNullOrEmpty(outputFolder))
				BuildProcess.Execute(outputFolder, project);
		}

		internal static bool Initialise()
		{
			// Default build platform is host platform
			Platform platform = Application.Platform;

			// Check for arg and parse it, if fails then set back to default
			if (Application.HasArg("platform") &&
				!Enum.TryParse(Application.GetArg("platform"), out platform))
					platform = Application.Platform;

			// TODO: Save project build settings to file and load desired platform from disk

			return SelectPlatform(platform);
		}

		internal static void ConstructBuildMenu()
		{
			EditorUIService editorUI = AquaSystem.Get<EditorUIService>();
			editorUI.AddMenuItemDirectory("Build");

			foreach (Platform platform in AvailableBuildPlatforms)
				editorUI.AddMenuItem(() => SelectPlatform(platform), new MenuItemAttribute($"Build/Platform/{platform}") { Icon = PlatformIcons[platform] });

			editorUI.AddMenuItem(StartBuildFromMenu, new MenuItemAttribute("Build/Start Build") { PrependSeparator = true, Shortcut = "CTRL+SHIFT+B" });
		}

		private static void StartBuildFromMenu() => StartBuild();
	}
}