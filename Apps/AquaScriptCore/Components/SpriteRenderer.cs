using System.Runtime.CompilerServices;

namespace AquaEngine
{
	public class SpriteRenderer : Component
	{
		/// <summary>
		/// Texture to draw
		/// </summary>
		public uint Sprite
		{
			get => _aqua_internal_GetSprite(World.ID, Entity.ID);
			set => _aqua_internal_SetSprite(World.ID, Entity.ID, value);
		}

		/// <summary>
		/// Tint to apply to drawn sprite
		/// </summary>
		public Colour Colour
		{
			get
			{
				_aqua_internal_GetColour(World.ID, Entity.ID, out Colour colour);
				return colour;
			}
			set => _aqua_internal_SetColour(World.ID, Entity.ID, ref value);
		}

		/// <summary>
		/// Shader program to draw sprite
		/// </summary>
		public uint Shader
		{
			get => _aqua_internal_GetShader(World.ID, Entity.ID);
			set => _aqua_internal_SetShader(World.ID, Entity.ID, value);
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _aqua_internal_GetSprite(uint worldID, uint entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_SetSprite(uint worldID, uint entityID, uint position);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_GetColour(uint worldID, uint entityID, out Colour colour);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_SetColour(uint worldID, uint entityID, ref Colour colour);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _aqua_internal_GetShader(uint worldID, uint entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _aqua_internal_SetShader(uint worldID, uint entityID, uint shader);
		#endregion
	}
}
