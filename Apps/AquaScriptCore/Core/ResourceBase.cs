using Newtonsoft.Json;
using System;
using System.IO;

namespace AquaEngine
{
	public abstract class ResourceBase
	{
		public UUID ResourceID { get; internal set; } = UUID.Invalid;
		public string ResourcePath { get; internal set; } = string.Empty;

		public IImportSettings ImportSettings { get; internal set; }

		/// <summary>
		/// When the resource is created
		/// </summary>
		protected virtual void OnLoad() { }

		/// <summary>
		/// When the resource is destroyed
		/// </summary>
		protected virtual void OnUnload() { }

		/// <summary>
		/// When <see cref="ImportSettings"/> get updated
		/// </summary>
		protected virtual void OnImported() { }

		/// <summary>
		/// This instance was created as a duplicate from <paramref name="original"/>.
		/// Called before <see cref="OnLoad"/>.
		/// </summary>
		protected virtual void OnCloned(ResourceBase original) { }

		internal ResourceBase Clone(UUID newID, string newPath)
		{
			ResourceBase clone = (ResourceBase)MemberwiseClone();

			clone.ResourceID = newID;
			clone.ResourcePath = newPath;
			clone.ImportSettings = ImportSettings;
			clone.OnCloned(this);

			return clone;
		}

		internal void _Load() => OnLoad();
		internal void _Unload() => OnUnload();

		public void Import(IImportSettings settings)
		{
			ImportSettings = settings;
			OnImported();
		}

		public void LoadFromDisk()
		{

		}

		/// <summary>
		/// Tries casting <see cref="ImportSettings"/> to <typeparamref name="T"/>.
		/// Returns default value if cannot cast, or if ImportSettings are null.
		/// </summary>
		/// <returns>True if successfully cast</returns>
		protected bool TryGetImportSettings<T>(out T value) where T : struct, IImportSettings
		{
			value = default;
			if (ImportSettings == null ||
				!ImportSettings.GetType().Equals(typeof(T)))
				return false;
			value = (T)ImportSettings;
			return true;
		}

		public static implicit operator bool(ResourceBase resourceBase) => resourceBase != null;
		public static implicit operator UUID(ResourceBase resourceBase) => resourceBase?.ResourceID ?? UUID.Invalid;
	}

	public abstract class NativeResourceBase : ResourceBase
	{
		protected IntPtr Handle { get; set; }

		internal void SetHandle(IntPtr handle) => Handle = handle;

		/// <summary>
		/// Called instead of <see cref="ResourceBase.OnLoad"/> when loaded from an existing native resource
		/// </summary>
		protected virtual void OnNativeLoad() { }

		protected override void OnCloned(ResourceBase original) =>
			Handle = ((NativeResourceBase)original).Handle;

		internal void _OnNativeLoad() => OnNativeLoad();
	}
}