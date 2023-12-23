using System;
using Yonai.IO;

namespace Yonai
{
	public abstract class ResourceBase
	{
		public UUID ResourceID { get; internal set; } = UUID.Invalid;
		public VFSFile ResourcePath { get; internal set; }

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

		internal void _Load() => OnLoad();
		internal void _Unload() => OnUnload();

		public void Import(IImportSettings settings)
		{
			ImportSettings = settings;
			OnImported();
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

		internal IntPtr GetHandle() => Handle;
		internal void SetHandle(IntPtr handle) => Handle = handle;

		/// <summary>
		/// Called instead of <see cref="ResourceBase.OnLoad"/> when loaded from an existing native resource
		/// </summary>
		protected virtual void OnNativeLoad() { }

		internal void _OnNativeLoad() => OnNativeLoad();
	}
}