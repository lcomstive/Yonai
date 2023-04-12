using System;
using AquaEngine;
using System.Linq;
using AquaEngine.Graphics;
using System.Collections.Generic;

namespace AquaEditor.Views
{
	public class ResourcesView : View
	{
		private float m_Padding = 12;
		private float m_ThumbnailSize = ThumbnailSizeRange.y;

		private VFSFile[] m_Files;
		private string m_SelectedPath;
		private string m_CurrentDirectory = RootDirectory;

		// New name popup
		private const string NewFilePopupName = "ResourcesNewFilePopup";
		private string m_NewFileName = string.Empty;
		private Action<string> m_NewNamePopupFinishAction = null;
		private bool m_NewNameModalOpen = false;

		private const string RootDirectory = "project://Assets";
		private static readonly IVector2 ThumbnailSizeRange = new IVector2(32, 96);

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
			
			NewFilePopup();

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

		private List<string> ValidTextureExtensions = new List<string>() { ".png", ".jpg", ".jpeg", ".dds" };

		private Texture ChooseImage(VFSFile file)
		{
			if (ValidTextureExtensions.Contains(file.Extension.ToLower()))
			{
				if (!Resource.Exists(file.FullPath))
					return Resource.Load<Texture>(file.FullPath);
				return Resource.Get<Texture>(file.FullPath);
			}

			return null;
		}

		private void DrawContents()
		{
			Vector2 size = ImGUI.ContentRegionAvailable;
			size.y -= 30;
			ImGUI.BeginChild("##ResourceContents", size);

			ImGUI.PushStyleColour(ImGUI.StyleColour.Button, Colour.None);

			float cellsize = m_ThumbnailSize + m_Padding;
			float panelWidth = ImGUI.ContentRegionAvailable.x;

			if (m_ThumbnailSize > ThumbnailSizeRange.x)
				ImGUI.Columns(Math.Max((int)(panelWidth / cellsize), 1), string.Empty, false);

			foreach (VFSFile file in m_Files)
			{
				bool selected = file.FullPath.Equals(m_SelectedPath);
				Texture texture = file.IsDirectory ? Icons.Folder : ChooseImage(file);
				if (m_ThumbnailSize > ThumbnailSizeRange.x)
				{
					if (selected) ImGUI.PopStyleColour();

					ImGUI.BeginGroup();
					ImGUI.ButtonImage(texture, new Vector2(m_ThumbnailSize, m_ThumbnailSize) * 0.8f);
					ImGUI.Text(file.FileName);
					ImGUI.EndGroup();

					if (selected) ImGUI.PushStyleColour(ImGUI.StyleColour.Button, Colour.None);
				}
				else
				{
					ImGUI.Image(texture, new Vector2(ImGUI.TextLineHeight, ImGUI.TextLineHeight));
					ImGUI.SameLine();
					ImGUI.Selectable(file.FileName, selected);
				}
				
				DrawContextMenu(file);
			
				if (ImGUI.IsItemClicked())
				{
					m_SelectedPath = file.FullPath;
					InspectorView.Target = file;

					if(file.IsDirectory && ImGUI.IsMouseDoubleClicked(MouseButton.Left))
						OpenDirectory($"{m_CurrentDirectory}/{file.FileName}");
					// else handle inspector for file type
				}
				ImGUI.NextColumn();
			}

			ImGUI.PopStyleColour();
			ImGUI.Columns(1);
			ImGUI.EndChild();

			if (ImGUI.IsItemClicked())
			{
				m_SelectedPath = string.Empty;

				// Check if inspector target is resource from this view
				if(InspectorView.Target != null && InspectorView.Target is VFSFile)
					InspectorView.Target = null; // Clear
			}
			if (ImGUI.IsItemHovered() &&
				ImGUI.IsMouseClicked(MouseButton.Right) &&
				!ImGUI.IsPopupOpen("ResourcesEdit"))
				ImGUI.OpenPopup("ResourcesNew");
			DrawContextMenu();
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
		}

		private void DrawContextMenu()
		{
			if (!ImGUI.BeginPopup($"ResourcesNew"))
				return;

			if (ImGUI.Selectable("Refresh"))
				OpenDirectory(m_CurrentDirectory);
			ImGUI.Separator();
			if (ImGUI.BeginMenu("New"))
			{
				if (ImGUI.Selectable("Directory"))
				{
					m_NewNamePopupFinishAction = (directoryName) => VFS.CreateDirectory($"{m_CurrentDirectory}/{directoryName}");
					m_NewNameModalOpen = true;
				}
				ImGUI.Separator();
				if (ImGUI.Selectable("Material"))
				{
					m_NewNamePopupFinishAction = (materialName) => Resource.Load<Material>($"{m_CurrentDirectory}/{materialName}.material", new MaterialImportSettings());
					m_NewNameModalOpen = true;
				}
				ImGUI.EndMenu();
			}

			ImGUI.EndPopup();
		}

		private void DrawContextMenu(VFSFile file)
		{
			if (!ImGUI.BeginPopupContextItem($"ResourcesEdit", ImGUI.PopupFlags.MouseButtonRight))
				return;

			if (ImGUI.Selectable("Delete"))
			{ }

			ImGUI.EndPopup();
		}
		
		private void NewFilePopup()
		{
			if (m_NewNameModalOpen)
				ImGUI.OpenPopup(NewFilePopupName);

			ImGUI.SetNextItemWidth(200);
			if (!ImGUI.BeginPopupModal(NewFilePopupName, ref m_NewNameModalOpen))
				return;

			ImGUI.Input("##ResourcesNewFileInput", ref m_NewFileName, 64);
			ImGUI.SameLine();
			if(ImGUI.Button("Submit"))
			{
				if(!string.IsNullOrEmpty(m_NewFileName)) 
					m_NewNamePopupFinishAction?.Invoke(m_NewFileName);
				m_NewFileName = string.Empty;
				m_NewNameModalOpen = false;
				m_NewNamePopupFinishAction = null;
				OpenDirectory(m_CurrentDirectory); // Refresh file list
				ImGUI.CloseCurrentPopup();
			}
			ImGUI.EndPopup();
		}
	}
}