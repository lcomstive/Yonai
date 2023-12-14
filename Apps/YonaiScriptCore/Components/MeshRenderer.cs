using Yonai.Graphics;

namespace Yonai
{
	public class MeshRenderer : Component
	{
		public UUID MeshID { get; set; } = UUID.Invalid;

		[Serialize(false)]
		public Mesh Mesh
		{
			get => Resource.Get<Mesh>(MeshID, false);
			set => MeshID = value?.ResourceID ?? UUID.Invalid;
		}

		public UUID MaterialID { get; set; } = UUID.Invalid;

		[Serialize(false)]
		public Material Material
		{
			get => Resource.Get<Material>(MaterialID, false);
			set => MaterialID = value?.ResourceID ?? UUID.Invalid;
		}
	}
}

