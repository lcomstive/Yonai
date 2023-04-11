using AquaEngine.Graphics;
using System;
using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public class SpriteRenderer : Component
	{
		/// <summary>
		/// Texture to draw
		/// </summary>
		public Texture Sprite
		{
			get => Resource.Get<Texture>(_GetSprite(Handle));
			set => _SetSprite(Handle, value.ResourceID);
		}

		/// <summary>
		/// Tint to apply to drawn sprite
		/// </summary>
		public Colour Colour
		{
			get
			{
				_GetColour(Handle, out Colour colour);
				return colour;
			}
			set => _SetColour(Handle, ref value);
		}

		/// <summary>
		/// Shader program to draw sprite
		/// </summary>
		public Shader Shader
		{
			get => Resource.Get<Shader>(_GetShader(Handle));
			set => _SetShader(Handle, value.ResourceID);
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong _GetSprite(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void  _SetSprite(IntPtr handle, ulong position);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void  _GetColour(IntPtr handle, out Colour colour);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void  _SetColour(IntPtr handle, ref Colour colour);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong _GetShader(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void  _SetShader(IntPtr handle, ulong shader);
		#endregion
	}
}
