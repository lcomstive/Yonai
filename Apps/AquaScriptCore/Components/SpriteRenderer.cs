using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public class SpriteRenderer : Component
	{
		/// <summary>
		/// Texture to draw
		/// </summary>
		public UUID Sprite
		{
			get => _GetSprite(World.ID, Entity.ID);
			set => _SetSprite(World.ID, Entity.ID, value);
		}

		/// <summary>
		/// Tint to apply to drawn sprite
		/// </summary>
		public Colour Colour
		{
			get
			{
				_GetColour(World.ID, Entity.ID, out Colour colour);
				return colour;
			}
			set => _SetColour(World.ID, Entity.ID, ref value);
		}

		/// <summary>
		/// Shader program to draw sprite
		/// </summary>
		public UUID Shader
		{
			get => _GetShader(World.ID, Entity.ID);
			set => _SetShader(World.ID, Entity.ID, value);
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong _GetSprite(ulong worldID, ulong entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetSprite(ulong worldID, ulong entityID, ulong position);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetColour(ulong worldID, ulong entityID, out Colour colour);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetColour(ulong worldID, ulong entityID, ref Colour colour);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern ulong _GetShader(ulong worldID, ulong entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetShader(ulong worldID, ulong entityID, ulong shader);
		#endregion
	}
}
