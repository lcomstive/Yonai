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
		[Serialize(false)]
		public Texture Sprite
		{
			get => Resource.Get<Texture>(SpriteID);
			set => SpriteID = value?.ResourceID ?? UUID.Invalid;
		}

		/// <summary>
		/// ID of texture to draw.
		/// Sets <see cref="Sprite"/>.
		/// </summary>
		public UUID SpriteID { get; set; }

		/// <summary>
		/// Tint to apply to drawn sprite
		/// </summary>
		public Colour Colour { get; set; }

		/// <summary>
		/// Shader used to draw sprite.
		/// Sets <see cref="ShaderID"/>.
		/// </summary>
		[Serialize(false)]
		public Shader Shader
		{
			get => Resource.Get<Shader>(ShaderID);
			set => ShaderID = value?.ResourceID ?? UUID.Invalid;
		}

		/// <summary>
		/// ID of shader used to draw sprite.
		/// Sets <see cref="Shader"/>.
		/// </summary>
		public UUID ShaderID { get; private set; }
	}
}
