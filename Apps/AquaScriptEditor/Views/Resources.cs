using AquaEngine;
using AquaEngine.Graphics;
using System.Linq;

namespace AquaEditor.Views
{
	public class ResourcesView : View
	{
		private float m_Padding = 16.0f;
		private float m_ThumbnailSize = 32;

		private VFSFile[] m_Files;
		private string m_CurrentDirectory = RootDirectory;

		private const string RootDirectory = "project://Assets";
		private static readonly IVector2 ThumbnailSizeRange = new IVector2(24, 64);

		[MenuItem("Window/Resources")]
		private static void MenuCallback() => EditorUIService.Open<ResourcesView>();

		protected override void Opened() => OpenDirectory(RootDirectory);

		protected override void Draw()
		{
			bool isOpen = true;
			if (ImGUI.Begin("Resources", ref isOpen))
			{
				DrawBreadcrumbs();
				DrawContents();
				DrawSizeSlider();
			}
			ImGUI.End();

			// Check if window requested to be closed
			if (!isOpen)
				EditorUIService.Close<ResourcesView>();
		}

		private void DrawBreadcrumbs()
		{
			ImGUI.PushStyleVar(ImGUI.StyleVar.FramePadding, new Vector2(5, 3));
			ImGUI.PushStyleColour(ImGUI.StyleColour.Button, new Colour(0, 0, 0, 0));

			Vector2 size = ImGUI.ContentRegionAvailable;
			size.y = ImGUI.TextLineHeight * 1.5f;
			ImGUI.BeginChild("ResourcesBreadcrumbs", size);
			string[] directories = m_CurrentDirectory.Replace(RootDirectory, "").TrimStart('/').Split('/');

			// Root directory
			if (ImGUI.Button("Assets"))
				OpenDirectory(RootDirectory);

			// Subdirectories
			for (int i = 0; i < directories.Length; i++)
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

			ImGUI.PopStyleVar();
			ImGUI.PopStyleColour();

			ImGUI.Separator();
		}

		private Texture ChooseImage(string extension) => null;

		private void DrawContents()
		{
			Vector2 size = ImGUI.ContentRegionAvailable;
			size.y -= 30;
			ImGUI.BeginChild("##ResourceContents", size);

			float cellsize = m_ThumbnailSize + m_Padding;
			float panelWidth = ImGUI.ContentRegionAvailable.x;

			if(m_ThumbnailSize > ThumbnailSizeRange.x)
				ImGUI.Columns((int)(panelWidth / cellsize), string.Empty, false);

			foreach (VFSFile file in m_Files)
			{
				Texture texture = file.IsDirectory ? Icons.Folder : ChooseImage(file.Extension);
				if (m_ThumbnailSize > ThumbnailSizeRange.x)
				{
					ImGUI.BeginGroup();
					ImGUI.ButtonImage(texture, new Vector2(m_ThumbnailSize, m_ThumbnailSize));
					ImGUI.Text(file.FileName);
					ImGUI.EndGroup();
				}
				else
				{
					float textureSize = ImGUI.TextLineHeight;
					ImGUI.Image(texture, new Vector2(textureSize, textureSize));
					ImGUI.SameLine();
					ImGUI.Selectable(file.FileName);
				}
				if (ImGUI.IsItemClicked())
				{
					if(file.IsDirectory)
						OpenDirectory($"{m_CurrentDirectory}/{file.FileName}");
					// else handle inspector for file type
				}
				ImGUI.NextColumn();
			}

			ImGUI.Columns(1);
			ImGUI.EndChild();
		}

		private void DrawSizeSlider()
		{
			ImGUI.PushStyleVar(ImGUI.StyleVar.FramePadding, new Vector2(0, 0));

			Vector2 size = ImGUI.WindowContentRegionMax;
			ImGUI.SetCursorPos(new Vector2(size.x - (size.x / 4) - 30, size.y - 20));
			ImGUI.Text("Size", Colour.Grey);
			ImGUI.SameLine();
			ImGUI.SetNextItemWidth(size.x / 4);
			ImGUI.Slider("##ThumbnailSize", ref m_ThumbnailSize, ThumbnailSizeRange.x, ThumbnailSizeRange.y, "");

			ImGUI.PopStyleVar();
		}

		private void OpenDirectory(string directory)
		{
			if (!VFS.Exists(directory))
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