using AquaEngine;
using System.IO;
using System.Linq;
using System.Net.NetworkInformation;

namespace AquaEditor.Views
{
	public class ResourcesView : View
	{
		private VFSFile[] m_Files;
		private string m_CurrentDirectory = RootDirectory;

		private const string RootDirectory = "project://Assets";

		[MenuItem("Window/Resources")]
		private static void MenuCallback() => EditorUIService.Open<ResourcesView>();

		protected override void Opened() => OpenDirectory(RootDirectory);

		protected override void Draw()
		{
			bool isOpen = true;
			if (ImGUI.Begin("Resources", ref isOpen))
			{
				try { DrawBreadcrumbs(); }
				catch(System.Exception e) { Log.Exception(e); }
				foreach (VFSFile file in m_Files)
				{
					ImGUI.Selectable(file.FileName);
					if (file.IsDirectory && ImGUI.IsItemClicked())
						OpenDirectory($"{m_CurrentDirectory}/{file.FileName}");
				}
			}
			ImGUI.End();

			// Check if window requested to be closed
			if (!isOpen)
				EditorUIService.Close<ResourcesView>();
		}

		private void DrawBreadcrumbs()
		{
			ImGUI.PushStyleColour(ImGUI.StyleColour.Button, new Colour(0, 0, 0, 0));

			Vector2 size = ImGUI.ContentRegionAvailable;
			size.y = ImGUI.TextLineHeight * 2f;
			ImGUI.BeginChild("ResourcesBreadcrumbs", size);
			string[] directories = m_CurrentDirectory.Replace(RootDirectory, "").TrimStart('/').Split('/');

			// Root directory
			if (ImGUI.Button("Assets"))
				OpenDirectory(RootDirectory);

			// Subdirectories
			for(int i = 0; i < directories.Length; i++)
			{
				if (string.IsNullOrEmpty(directories[i]))
					continue;
				ImGUI.SameLine(0, 0);
				ImGUI.Text("/", Colour.Grey);
				ImGUI.SameLine(0, 0);
				if (ImGUI.Button(directories[i]) && i != directories.Length - 1)
					OpenDirectory(RootDirectory + "/" + string.Join("/", directories.Take(i + 1).ToArray()));
			}
			ImGUI.EndChild();

			ImGUI.PopStyleColour();
		}

		private void OpenDirectory(string directory)
		{
			if(!VFS.Exists(directory))
			{
				Log.Warning($"Opening directory '{directory}' but does not exist");
				return;
			}

			m_CurrentDirectory = directory;
			m_Files = VFS.GetFiles(m_CurrentDirectory);
			Log.Trace($"[Resource View] Opened directory '{m_CurrentDirectory}'");
		}
	}
}