using System;
using Yonai;
using System.Linq;
using YonaiEditor.Systems;
using System.Collections.Generic;

namespace YonaiEditor.BuildProcess
{
	public static class GameBuilder
	{
		/// <summary>
		/// The <see cref="IBuildProcess"/> associated with the build <see cref="Platform"/>
		/// </summary>
		private static Dictionary<Platform, IBuildProcess> BuildProcesses = new Dictionary<Platform, IBuildProcess>()
		{
			{ Platform.Windows, new WindowsBuildProcess() },
			{ Platform.Mac, new MacBuildProcess() }
		};

		/// <summary>
		/// What <see cref="Platform"/>s the host <see cref="Platform"/> can build for (e.g. Windows -> [ Windows, Android ])
		/// </summary>
		private static Dictionary<Platform, Platform[]> BuildPlatformMatrix = new Dictionary<Platform, Platform[]>()
		{
			{ Platform.Windows, new Platform[] { Platform.Windows } },
			{ Platform.Mac, new Platform[] { Platform.Mac} }
		};

		/// <summary>
		/// Maps a <see cref="Platform"/> to the <see cref="Yonai.IO.VFS"/> filepath of icon texture.
		/// Can also be from the path assets://Textures/OS/ and exclude the .png file extension
		/// </summary>
		private static Dictionary<Platform, string> PlatformIcons = new Dictionary<Platform, string>()
		{
			{ Platform.Windows, "Windows" },
			{ Platform.Mac, "Mac" }
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

		public static void StartBuild(string outputFolder = null) => BuildProcess.Execute(outputFolder, ProjectHubService.ActiveProject);
		public static void StartBuild(ProjectFile project, string outputFolder = null) => BuildProcess.Execute(outputFolder, project);

		internal static bool Initialise()
		{
			Platform platform = LocalProjectSettings.BuildTarget;

			if (!IsValidBuildPlatform(platform))
				platform = Application.Platform; // Default to host platform

			// If building from command line, check for platform arg and parse it.
			if (Application.HasArg("build") && Application.HasArg("platform") && // Check if args request building and have a platform
				Enum.TryParse(Application.GetArg("platform"), out Platform argsPlatform) && // Try to parse the platform
				IsValidBuildPlatform(argsPlatform)) // Ensure it's a valid buildable target from this host
					platform = argsPlatform;

			return SelectPlatform(platform);
		}

		internal static void ConstructBuildMenu()
		{
			EditorUIService editorUI = YonaiSystem.Get<EditorUIService>();
			editorUI.AddMenuItemDirectory("Build");

			foreach (Platform platform in AvailableBuildPlatforms)
				editorUI.AddMenuItem(() =>
				{
					LocalProjectSettings.BuildTarget = platform;
					LocalProjectSettings.Save();

					SelectPlatform(platform);
				}, new MenuItemAttribute($"Build/Platform/{platform}") { Icon = PlatformIcons[platform] });

			editorUI.AddMenuItem(StartBuildFromMenu, new MenuItemAttribute("Build/Start Build") { PrependSeparator = true, Shortcut = "CTRL+SHIFT+B" });
		}

		private static void StartBuildFromMenu() => StartBuild();
	}
}