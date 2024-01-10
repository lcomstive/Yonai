using Yonai.Graphics;

namespace Yonai
{
	public class MeshRenderer : Component
	{
		[Serialize(false)]
		public UUID MeshID { get; set; }

		[Serialize(false)]
		public UUID MaterialID { get; set; }

		public Mesh Mesh
		{
			get => Resource.Get<Mesh>(MeshID);
			set => MeshID = value?.ResourceID ?? UUID.Invalid;
		}

		public Material Material
		{
			get => Resource.Get<Material>(MaterialID);
			set => MaterialID = value?.ResourceID ?? UUID.Invalid;
		}
	}
}

