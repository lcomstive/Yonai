using System;
using Yonai.Graphics;
using System.Runtime.CompilerServices;

namespace Yonai
{
	public class SpriteRenderer : Component
	{
		/// <summary>
		/// Texture to draw.
		/// Sets <see cref="SpriteID"/>.
		/// </summary>
		public Texture Sprite
		{
			get => Resource.Get<Texture>(SpriteID);
			set => SpriteID = value?.ResourceID ?? UUID.Invalid;
		}

		/// <summary>
		/// ID of texture to draw.
		/// Sets <see cref="Sprite"/>.
		/// </summary>
		[Serialize(false)]
		public UUID SpriteID
		{
			get => _GetSprite(Handle);
			set => _SetSprite(Handle, value);
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
		/// Shader used to draw sprite.
		/// Sets <see cref="ShaderID"/>.
		/// </summary>
		public Shader Shader
		{
			get => Resource.Get<Shader>(ShaderID);
			set => ShaderID = value?.ResourceID ?? UUID.Invalid;
		}

		/// <summary>
		/// ID of shader used to draw sprite.
		/// Sets <see cref="Shader"/>.
		/// </summary>
		[Serialize(false)]
		public UUID ShaderID
		{
			get => _GetShader(Handle);
			set => _SetShader(Handle, value);
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
