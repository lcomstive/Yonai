using Yonai;
using System;
using Yonai.IO;
using System.Linq;
using Yonai.Graphics;
using System.Diagnostics;
using YonaiEditor.Systems;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace YonaiEditor.Views
{
	public class ResourcesView : View
	{
		private float m_Padding = 12;
		private Texture m_IconFolder;
		private float m_ThumbnailSize = 0;

		private VFSFile[] m_Files;
		private string m_SelectedPath;
		private string m_CurrentDirectory = RootDirectory;

		/// <summary>
		/// Files with these extensions are not shown in the resource view
		/// </summary>
		private List<string> IgnoreFileExtensions = new List<string>() { ".cache", ".DS_Store" };

		/// <summary>
		/// These extensions will load as a texture and be previewed instead of an icon
		/// </summary>
		private List<string> ValidTextureExtensions = new List<string>() { ".png", ".jpg", ".jpeg", ".dds" };

		// New name popup
		private const string NewFilePopupName = "ResourcesNewFilePopup";
		private string m_NewFileName = string.Empty;
		private Action<string> m_NewNamePopupFinishAction = null;
		private bool m_NewNameModalOpen = false;

		private const string RootDirectory = "project://Assets";
		private static readonly IVector2 ThumbnailSizeRange = new IVector2(32, 96);

		private const string LocalSettingsLastDirectoryName = "ResourcesLastDirectory";
		private const string LocalSettingsThumbnailSizeName = "ResourcesThumbnailSize";

		[MenuItem("Window/Resources")]
		private static void MenuCallback() => EditorUIService.Open<ResourcesView>();

		protected override void Opened()
		{
			m_IconFolder = Resource.Get<Texture>("assets://Textures/Icons/Folder.png");
			m_ThumbnailSize = LocalProjectSettings.Get(LocalSettingsThumbnailSizeName, 0.0f);

			if (LocalProjectSettings.TryGet(LocalSettingsLastDirectoryName, out string lastDirectory) &&
				lastDirectory.StartsWith(RootDirectory) && VFS.Exists(lastDirectory))
				OpenDirectory(lastDirectory);
			else
				OpenDirectory(RootDirectory);
		}

		protected override void Closed()
		{
			LocalProjectSettings.Set(LocalSettingsLastDirectoryName, m_CurrentDirectory);
			LocalProjectSettings.Set(LocalSettingsThumbnailSizeName, m_ThumbnailSize);
		}

		/// <summary>
		/// Scans current directory for files and refreshes any changes
		/// </summary>
		public void Refresh() => OpenDirectory(m_CurrentDirectory);

		public void HighlightPath(string path)
		{
			if (!VFS.Exists(path) || !path.StartsWith(RootDirectory))
				return; // Cannot navigate to path

			OpenDirectory(path.Substring(0, path.LastIndexOf('/')));
			m_SelectedPath = path;
		}

		protected override void Draw()
		{
			bool isOpen = true;
			if (ImGUI.Begin("Resources", ref isOpen))
			{
				DrawBreadcrumbs();
				DrawContents();
				DrawBottomBar();
			}
			ImGUI.End();

			NewFilePopup();

			// Check if window requested to be closed
			if (!isOpen)
				EditorUIService.Close<ResourcesView>();
		}

		private string GetDirectoryPath(string[] directories, int index) =>
			RootDirectory + "/" + string.Join("/", directories.Take(index + 1).ToArray());

		/// <summary>
		/// Handle moving files up directories via breadcrumbs
		/// </summary>
		/// <param name="directory"></param>
		private void BreadcrumbDragDropTarget(string directory)
		{
			if (!ImGUI.BeginDragDropTarget())
				return;

			object payload = ImGUI.AcceptDragDropPayload("ResourcePath", ImGUI.DragDropFlags.AcceptPeekOnly);
			if (payload == null)
				return;
			VFSFile payloadFile = (VFSFile)payload;
			if (payloadFile.ParentDirectory.Trim('/').CompareTo(directory) == 0)
				return; // Don't drop into same directory
			ImGUI.AcceptDragDropPayload("ResourcePath");

			if (ImGUI.DragDropPayloadIsDelivery())
			{
				VFS.Move(payloadFile.FullPath, $"{directory}/{payloadFile.FileName}");
				// Move cache file as well
				if (VFS.Exists($"{payloadFile.FullPath}.cache"))
					VFS.Move($"{payloadFile.FullPath}.cache", $"{directory}/{payloadFile.FileName}.cache");
				Refresh();
			}
			ImGUI.EndDragDropTarget();
		}

		/// <summary>
		/// Draw current directory structure
		/// e.g. Assets / Models / Backpack / Textures
		/// </summary>
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
			BreadcrumbDragDropTarget(RootDirectory);

			// Subdirectories
			for (int i = 0; i < directories.Length; i++)
			{
				if (string.IsNullOrEmpty(directories[i]))
					continue;
				ImGUI.SameLine(0, 0);
				ImGUI.Text("/", Colour.Grey);
				ImGUI.SameLine(0, 0);
				if (ImGUI.Button(directories[i]) && i != directories.Length - 1)
					OpenDirectory(GetDirectoryPath(directories, i));

				BreadcrumbDragDropTarget(GetDirectoryPath(directories, i));
			}

			ImGUI.PopStyleVar();
			ImGUI.PopStyleColour();

			ImGUI.EndChild();
			ImGUI.Separator();
		}

		/// <summary>
		/// Maps file extensions to functions that return the icon or image to display to the user
		/// </summary>
		private static Dictionary<string, Func<VFSFile, Texture>> ImageExtensionImages = new Dictionary<string, Func<VFSFile, Texture>>()
		{
			{ ".png",       GetTexturePreview           },
			{ ".jpg",       GetTexturePreview           },
			{ ".jpeg",      GetTexturePreview           },
			{ ".dds",       GetTexturePreview           },
			{ ".mp3",       GetAudioPreview             },
			{ ".ogg",       GetAudioPreview             },
			{ ".wav",       GetAudioPreview             },
			{ ".mixer",     (_) => Icons.Get("Mixer")   },
			{ ".shader",    (_) => Icons.Get("Shader")  },
			{ ".world",     (_) => Icons.Get("World")   },
			{ ".material",  (_) => Icons.Get("Material")},
		};

		private Texture ChooseImage(VFSFile file)
		{
			if (ImageExtensionImages.TryGetValue(file.Extension, out Func<VFSFile, Texture> imageFunc))
				return imageFunc(file);
			return Icons.Get("File");
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
				if (IgnoreFileExtensions.Contains(file.Extension))
					continue; // Ignore this file

				bool selected = file.FullPath.Equals(m_SelectedPath);
				Texture texture = file.IsDirectory ? m_IconFolder : ChooseImage(file);
				ImGUI.PushID(file.FullPath);
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
					if (file.IsDirectory)
					{
						ImGUI.Image(texture, new Vector2(ImGUI.TextLineHeight, ImGUI.TextLineHeight));
						ImGUI.SameLine();
					}
					else
						ImGUI.HorizontalSpace(ImGUI.TextLineHeight);
					ImGUI.Selectable(file.FileName, selected);
				}

				DrawContextMenu(file);

				if (ImGUI.BeginDragDropSource(ImGUI.DragDropFlags.SourceAllowNullID))
				{
					// Drag drop preview
					ImGUI.Text(file.FileName);

					// Set contents
					ImGUI.SetDragDropPayload("ResourcePath", file);
					ImGUI.EndDragDropSource();
				}

				if (file.IsDirectory && ImGUI.BeginDragDropTarget())
				{
					object payload = ImGUI.AcceptDragDropPayload("ResourcePath");
					if (payload != null && ImGUI.DragDropPayloadIsDelivery())
					{
						VFSFile payloadFile = (VFSFile)payload;
						VFS.Move(payloadFile.FullPath, $"{file.FullPath}/{payloadFile.FileName}");
						// Move cache file as well
						if (VFS.Exists($"{payloadFile.FullPath}.cache"))
							VFS.Move($"{payloadFile.FullPath}.cache", $"{file.FullPath}/{payloadFile.FileName}.cache");
						Refresh();
					}
					ImGUI.EndDragDropTarget();
				}

				if (ImGUI.IsItemClicked())
				{
					m_SelectedPath = file.FullPath;
					if (ImGUI.IsMouseDoubleClicked(MouseButton.Left))
					{
						if (file.IsDirectory && ImGUI.IsMouseDoubleClicked(MouseButton.Left))
							OpenDirectory($"{m_CurrentDirectory}/{file.FileName}");
						else
							// Handle inspector for file type
							InspectorView.Target = file;
					}
				}
				ImGUI.PopID();
				ImGUI.NextColumn();
			}

			ImGUI.PopStyleColour();
			ImGUI.Columns(1);
			ImGUI.EndChild();

			if (ImGUI.IsItemClicked())
			{
				m_SelectedPath = string.Empty;
				if (ImGUI.IsMouseDoubleClicked(MouseButton.Left))
					InspectorView.Target = null;
			}

			if (ImGUI.IsItemHovered() &&
				ImGUI.IsMouseClicked(MouseButton.Right) &&
				!ImGUI.IsPopupOpen("ResourcesEdit"))
				ImGUI.OpenPopup("ResourcesNew");
			DrawContextMenu();
		}

		private void DrawBottomBar()
		{
			Vector2 size = ImGUI.WindowContentRegionMax;
			ImGUI.PushStyleVar(ImGUI.StyleVar.FramePadding, new Vector2(0, 0));

			// Search button //
			ImGUI.PushStyleColour(ImGUI.StyleColour.Button, Colour.None);
			if (ImGUI.ButtonImage(Icons.Get("Search"), new Vector2(20, 20)))
			{
				VFSFile[] files = VFS.GetFiles(RootDirectory, true);
				List<string> searchPaths = new List<string>();
				foreach (VFSFile file in files)
				{
					if (file.Extension.Equals(".cache"))
						continue; // Ignore cache files

					string filepath = file.FullPath.Replace(RootDirectory + "/", "");
					if (file.IsDirectory)
						filepath += '/';
					searchPaths.Add(filepath);
				}

				SearchView.Search(searchPaths.ToArray(), (selectedFile) =>
				{
					if (!string.IsNullOrEmpty(selectedFile))
						HighlightPath($"{RootDirectory}/{selectedFile}");
				});
			}
			ImGUI.PopStyleColour();

			// Thumbnail size slider //
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

		private void ShowInExplorer(string directory)
		{
			// Get absolute directory
			directory = VFS.GetMapping<VFSFileMapping>(directory)?.ExpandPath(directory) ?? directory;

			if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
			{
				directory = directory.Replace("/", "\\");
				Process.Start(new ProcessStartInfo("explorer", directory));
			}
			else if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
				Process.Start("open", directory);
		}

		private void DrawContextMenu()
		{
			if (!ImGUI.BeginPopup("ResourcesNew"))
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
				if (ImGUI.Selectable("Shader"))
				{
					m_NewNamePopupFinishAction = (shaderName) => Resource.Load<Shader>($"{m_CurrentDirectory}/{shaderName}.shader", new ShaderImportSettings());
					m_NewNameModalOpen = true;
				}
				ImGUI.EndMenu();
			}
			ImGUI.Separator();
			if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows) && ImGUI.Selectable("Show in Explorer"))
				ShowInExplorer(m_CurrentDirectory);
			else if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX) && ImGUI.Selectable("Show in Finder"))
				ShowInExplorer(m_CurrentDirectory);

			ImGUI.EndPopup();
		}

		private void DrawContextMenu(VFSFile file)
		{
			if (!ImGUI.BeginPopupContextItem($"ResourcesEdit", ImGUI.PopupFlags.MouseButtonRight))
				return;

			if (ImGUI.Selectable("Copy Path"))
				Clipboard.SetText(file.FullPath);

			if (ImGUI.Selectable("Delete"))
			{
				VFS.Remove(file.FullPath);
				Refresh();
			}

			ImGUI.Separator();
			if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows) && ImGUI.Selectable("Show in Explorer"))
				ShowInExplorer(file.IsDirectory ? file.FullPath : file.ParentDirectory);
			else if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX) && ImGUI.Selectable("Show in Finder"))
				ShowInExplorer(file.IsDirectory ? file.FullPath : file.ParentDirectory);

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
			if (ImGUI.Button("Submit"))
			{
				if (!string.IsNullOrEmpty(m_NewFileName))
					m_NewNamePopupFinishAction?.Invoke(m_NewFileName);
				m_NewFileName = string.Empty;
				m_NewNameModalOpen = false;
				m_NewNamePopupFinishAction = null;
				OpenDirectory(m_CurrentDirectory); // Refresh file list
				ImGUI.CloseCurrentPopup();
			}
			ImGUI.EndPopup();
		}

		#region File Images
		private static Texture GetTexturePreview(VFSFile file)
		{
			if (!Resource.Exists(file.FullPath))
				return Resource.Load<Texture>(file.FullPath);
			return Resource.Get<Texture>(file.FullPath);
		}

		private static Texture GetAudioPreview(VFSFile _) => Icons.Get("Audio");
		#endregion
	}
}