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
		public uint Shader
		{
			get => _GetShader(World.ID, Entity.ID);
			set => _SetShader(World.ID, Entity.ID, value);
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _GetSprite(uint worldID, uint entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetSprite(uint worldID, uint entityID, uint position);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _GetColour(uint worldID, uint entityID, out Colour colour);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetColour(uint worldID, uint entityID, ref Colour colour);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern uint _GetShader(uint worldID, uint entityID);
		[MethodImpl(MethodImplOptions.InternalCall)] private static extern void _SetShader(uint worldID, uint entityID, uint shader);
		#endregion
	}
}
