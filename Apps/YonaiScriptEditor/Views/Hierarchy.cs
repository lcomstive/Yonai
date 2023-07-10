using Yonai;
using Yonai.Graphics;

namespace YonaiEditor.Views
{
	public class HierarchyView : View
	{
		private Colour m_SelectedColour = new Colour(1, 1, 1, 0.25f);

		[MenuItem("Window/Hierarchy")]
		private static void MenuCallback() => EditorUIService.Open<HierarchyView>();

		protected override void Draw()
		{
			World[] worlds = SceneManager.GetActiveScenes();
			bool isOpen = true;
			if (ImGUI.Begin("Hierarchy", ref isOpen))
			{
				for(int i = 0; i < worlds.Length; i++)
				{
					World world = worlds[i];
					if (!ImGUI.Foldout(world.Name, true))
						continue;
					Entity[] entities = world.GetEntities();

					Vector2 size = i < worlds.Length - 1 ? new Vector2(0, entities.Length * 21) : Vector2.Zero;
					ImGUI.BeginChild($"{world.Name} [{world.ID}]", size);

					foreach (Entity entity in entities)
					{
						bool isSelected = InspectorView.Target?.Equals(entity) ?? false;

						if (isSelected)
							ImGUI.PushStyleColour(ImGUI.StyleColour.ChildBg, m_SelectedColour);

						ImGUI.BeginChild($"{world.ID}:{entity.ID}", new Vector2(0, 17.5f));

						NameComponent nameComponent;
						if (!entity.TryGetComponent(out nameComponent))
						{
							nameComponent = entity.AddComponent<NameComponent>();
							nameComponent.Name = $"Entity {entity.ID}";
						}

						ImGUI.Selectable(nameComponent.Name);

						if (ImGUI.IsItemHovered() && ImGUI.IsAnyMouseClicked())
							InspectorView.Target = entity;

						if (isSelected)
							ImGUI.PopStyleColour();

						ImGUI.EndChild();

						DrawContextMenu(entity);
					}

					ImGUI.EndChild();

					// If selected empty space and entity is currently selected, deselect it
					if (ImGUI.IsItemClicked() && InspectorView.Target is Entity)
						InspectorView.Target = null;

					DrawContextMenu(world);
				}
			}
			ImGUI.End();

			// Check if window requested to be closed
			if (!isOpen)
				EditorUIService.Close<HierarchyView>();
		}

		private void DrawContextMenu(Entity entity)
		{
			if (!ImGUI.BeginPopupContextItem($"Hierarchy:{entity.World.ID}:{entity.ID}", ImGUI.PopupFlags.MouseButtonRight))
				return;
			InspectorView.Target = entity;

			if (ImGUI.Selectable("Delete"))
				entity.Destroy();

			ImGUI.EndPopup();
		}

		private void DrawContextMenu(World world)
		{
			if (!ImGUI.BeginPopupContextItem($"Hierarchy:{world.ID}", ImGUI.PopupFlags.MouseButtonRight))
				return;

			if (ImGUI.BeginMenu("New"))
			{
				if (ImGUI.Selectable("Entity"))
					world.CreateEntity().AddComponent<NameComponent>().Name = "Entity";

				if (ImGUI.Selectable("Camera"))
					CreateEntity(world, "Camera").AddComponent<Camera>();

				if (ImGUI.Selectable("Sound Source"))
					CreateEntity(world, "Sound Source").AddComponent<SoundSource>();

				ImGUI.Separator();
				ImGUI.Text("3D Objects", Colour.Grey);

				if (ImGUI.Selectable("Cube"))
					CreatePrimitive(world, Mesh.Cube, "Cube");
				if (ImGUI.Selectable("Sphere"))
					CreatePrimitive(world, Mesh.Sphere, "Sphere");
				if (ImGUI.Selectable("Quad"))
					CreatePrimitive(world, Mesh.Quad, "Quad");

				ImGUI.Separator();
				ImGUI.Text("2D Objects", Colour.Grey);

				if (ImGUI.Selectable("Sprite"))
					CreateSprite(world);

				ImGUI.EndMenu();
			}

			ImGUI.EndPopup();
		}

		private Entity CreateEntity(World world, string name)
		{
			Entity entity = world.CreateEntity();
			entity.AddComponent<NameComponent>().Name = name;
			entity.AddComponent<Transform>();
			return entity;
		}

		private void CreatePrimitive(World world, UUID meshID, string name)
		{
			MeshRenderer meshRenderer = CreateEntity(world, name).AddComponent<MeshRenderer>();
			meshRenderer.Mesh = meshID;
			meshRenderer.Material = Resource.Get<Material>("Materials/Default3D");
		}

		private void CreateSprite(World world)
		{
			SpriteRenderer spriteRenderer = CreateEntity(world, "Sprite").AddComponent<SpriteRenderer>();
			spriteRenderer.Sprite = Resource.Get<Texture>("Textures/DefaultSprite");
			spriteRenderer.Shader = Resource.Get<Shader>("Shaders/2D/Default");
		}
	}
}