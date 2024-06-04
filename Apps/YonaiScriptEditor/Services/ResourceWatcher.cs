using Yonai;
using Yonai.IO;
using System.IO;
using System.Linq;
using YonaiEditor.Views;
using System.Collections.Generic;

namespace YonaiEditor.Systems
{
	/// <summary>
	/// Watches for file changes in the project asset directory and reacts accordingly
	/// </summary>
	public class ResourceWatcherService : YonaiSystem
	{
		private Dictionary<string, FileSystemWatcher> m_Watchers = new Dictionary<string, FileSystemWatcher>();

		protected override void Enabled()
		{
			AddWatcher(VFS.ExpandPath("project://Assets", true));
			AddWatcher(VFS.ExpandPath("app://Assets", true));
		}

		protected override void Disabled()
		{
			FileSystemWatcher[] watchers = m_Watchers.Values.ToArray();
			for (int i = 0; i < watchers.Length; i++)
				watchers[i].Dispose();
			m_Watchers.Clear();
		}

		private void AddWatcher(string path)
		{
			if (m_Watchers.ContainsKey(path)) return;

			FileSystemWatcher watcher = new FileSystemWatcher(path);

			watcher.NotifyFilter = NotifyFilters.CreationTime |
									NotifyFilters.DirectoryName |
									NotifyFilters.LastWrite |
									NotifyFilters.FileName |
									NotifyFilters.Size;

			watcher.Changed += OnChange;
			watcher.Created += OnCreate;
			watcher.Deleted += OnDeleted;
			watcher.Renamed += OnRenamed;
			watcher.Error += OnError;

			watcher.IncludeSubdirectories = true;
			watcher.EnableRaisingEvents = true;

			m_Watchers.Add(path, watcher);
			Log.Trace($"Watching '{path}' for changes");
		}

		private void RemoveWatcher(string path)
		{
			if (!m_Watchers.TryGetValue(path, out FileSystemWatcher watcher))
				return;

			watcher.Dispose();
			m_Watchers.Remove(path);

			Log.Trace($"Stopped watching '{path}' for changes");
		}

		private void OnCreate(object sender, FileSystemEventArgs e)
		{
			VFSFile vfsFile = VFS.AbsoluteToVFSPath(e.Name);
			Log.Debug($"'{e.Name}' was created");
			FileChanged?.Invoke(vfsFile, WatcherChangeTypes.Created);
			EditorUIService.GetView<ResourcesView>().Refresh();
		}

		private void OnDeleted(object sender, FileSystemEventArgs e)
		{
			VFSFile vfsFile = VFS.AbsoluteToVFSPath(e.Name);
			Log.Debug($"'{e.Name}' was deleted");
			FileChanged?.Invoke(vfsFile, WatcherChangeTypes.Deleted);
			EditorUIService.GetView<ResourcesView>().Refresh();
		}

		private void OnChange(object sender, FileSystemEventArgs e)
		{
			try
			{
				Log.Debug($"Change in '{e.Name}' ({e.FullPath})");

				VFSFile vfsFile = VFS.AbsoluteToVFSPath(e.Name);
				Log.Debug($"'{vfsFile.FullPath}' was changed");
				FileChanged?.Invoke(vfsFile, WatcherChangeTypes.Changed);
			}
			catch(System.Exception ex) { Log.Exception(ex); }
		}

		private void OnRenamed(object sender, RenamedEventArgs e)
		{
			VFSFile vfsFileOld = VFS.AbsoluteToVFSPath(e.OldName);
			VFSFile vfsFileNew = VFS.AbsoluteToVFSPath(e.Name);

			Log.Debug($"'{vfsFileOld}' was renamed to '{vfsFileNew}'");

			UUID resourceID = Resource.GetID(vfsFileOld);
			if (resourceID != UUID.Invalid)
				Resource.Move(resourceID, vfsFileNew);

			FileChanged?.Invoke(vfsFileOld, WatcherChangeTypes.Renamed);
			EditorUIService.GetView<ResourcesView>().Refresh();
		}

		private void OnError(object sender, ErrorEventArgs e) =>
			Log.Exception(e.GetException(), "Error watching resources");

		public delegate void OnFileChange(VFSFile file, WatcherChangeTypes change);
		public static event OnFileChange FileChanged;
	}
}