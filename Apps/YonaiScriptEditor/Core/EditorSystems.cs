using System.Runtime.CompilerServices;
using System;
using Yonai;

namespace YonaiEditor
{
	public static class EditorSystems
	{
		public static bool Has<T>() where T : YonaiSystem => _Has(typeof(T));
		public static T Add<T>() where T : YonaiSystem => (T)_Add(typeof(T));
		public static T Get<T>() where T : YonaiSystem => (T)_Get(typeof(T));
		public static bool Remove<T>() where T : YonaiSystem => _Remove(typeof(T));

		public static void Enable<T>(bool enable = true) where T : YonaiSystem =>
			_Enable(typeof(T), enable);

		public static YonaiSystem[] GetAll()
		{
			object[] objs = _GetAll();
			YonaiSystem[] systems = new YonaiSystem[objs.Length];
			for (int i = 0; i < objs.Length; i++)
				systems[i] = objs[i] as YonaiSystem;
			return systems;
		}

		#region Internal calls
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern bool _Has(Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern object _Get(Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern object[] _GetAll();
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern object _Add(Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern bool _Remove(Type type);
		[MethodImpl(MethodImplOptions.InternalCall)] internal static extern bool _Enable(Type type, bool enable);
		#endregion
	}
}