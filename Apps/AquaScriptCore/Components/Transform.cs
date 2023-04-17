using System;
using System.Linq;
using Newtonsoft.Json.Linq;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public class Transform : Component
	{
		[Serialize(Label = "Position")]
		public Vector3 LocalPosition
		{
			get
			{
				_GetPosition(Handle, out Vector3 position);
				return position;
			}
			set => _SetPosition(Handle, ref value);
		}

		[Serialize(false), HideInInspector]
		public Vector3 Position
		{
			get
			{
				_GetGlobalPosition(Handle, out Vector3 position);
				return position;
			}
			set
			{
				if(Parent == null)
				{
					_SetPosition(Handle, ref value);
					return;
				}

				// TODO: Test this
				Vector3 delta = value - Parent.Position;
				_SetPosition(Handle, ref delta);
			}
		}

		[Serialize(Label = "Rotation")]
		public Quaternion LocalRotation
		{
			get
			{
				_GetRotation(Handle, out Quaternion rotation);
				return rotation;
			}
			set => _SetRotation(Handle, ref value);
		}

		[Serialize(false), HideInInspector]
		public Quaternion Rotation
		{
			get
			{
				_GetGlobalRotation(Handle, out Quaternion rotation);
				return rotation;
			}
			// set => _SetRotation(Handle, ref value);
		}

		[Serialize(Label = "Scale")]
		public Vector3 LocalScale
		{
			get
			{
				_GetScale(Handle, out Vector3 scale);
				return scale;
			}
			set => _SetScale(Handle, ref value);
		}

		[Serialize(false), HideInInspector]
		public Vector3 Scale
		{
			get
			{
				_GetGlobalScale(Handle, out Vector3 scale);
				return scale;
			}
			// set => _SetScale(Handle, ref value);
		}

		[Serialize(false), HideInInspector]
		public Vector3 Right
		{
			get
			{
				_GetRight(Handle, out Vector3 value);
				return value;
			}
		}

		[Serialize(false), HideInInspector]
		public Vector3 Up
		{
			get
			{
				_GetUp(Handle, out Vector3 value);
				return value;
			}
		}

		[Serialize(false), HideInInspector]
		public Vector3 Forward
		{
			get
			{
				_GetForward(Handle, out Vector3 value);
				return value;
			}
		}

		[Serialize(false), HideInInspector]
		public Transform Parent
		{
			get => _GetParent(Handle) as Transform;
			set => _SetParent(Handle, value ? value.Handle : IntPtr.Zero);
		}

		public Transform[] GetChildren() => _GetChildren(Handle).Cast<Transform>().ToArray();

		public void AddChild(Transform child) => _AddChild(Handle, child.Handle);
		public void RemoveChild(Transform child) => _RemoveChild(Handle, child.Handle);

		public override JObject OnSerialize()
		{
			JObject json = base.OnSerialize();
			if(Parent != null)
				json["Parent"] = Parent.Entity.ID.ToString();
			return json;
		}

		public override void OnDeserialize(JObject json)
		{
			base.OnDeserialize(json);
			if (json["Parent"] == null)
				return;

			string parentIDStr = json["Parent"].Value<string>();
			if (!string.IsNullOrEmpty(parentIDStr))
			{
				UUID parentID = ulong.Parse(parentIDStr);
				Log.Warning($"Setting entity parent to {parentID}...");
				Parent = World.GetEntity(parentID)?.GetComponent<Transform>();
				if(Parent == null)
					Log.Warning("Parent transform not found");
			}
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetPosition(IntPtr handle, ref Vector3 value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetPosition(IntPtr handle, out Vector3 value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetGlobalPosition(IntPtr handle, out Vector3 value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetRotation(IntPtr handle, ref Quaternion value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetRotation(IntPtr handle, out Quaternion value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetGlobalRotation(IntPtr handle, out Quaternion value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetScale(IntPtr handle, ref Vector3 value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetScale(IntPtr handle, out Vector3 value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetGlobalScale(IntPtr handle, out Vector3 value);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetUp(IntPtr handle, out Vector3 value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetRight(IntPtr handle, out Vector3 value);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetForward(IntPtr handle, out Vector3 value);
	
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern object _GetParent(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetParent(IntPtr handle, IntPtr parentHandle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern object[] _GetChildren(IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _AddChild(IntPtr handle, IntPtr childHandle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _RemoveChild(IntPtr handle, IntPtr childHandle);
		#endregion
	}
}
