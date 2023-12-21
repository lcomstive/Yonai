using Yonai;
using System;
using Yonai.Graphics;
using Yonai.IO;
using YonaiEditor.Systems;
using System.Collections.Generic;

namespace YonaiEditor.Views
{
	public class HierarchyView : View
	{
		private bool m_IsArrowKeyDown = false;

		[MenuItem("Window/Hierarchy")]
		private static void MenuCallback() => EditorUIService.Open<HierarchyView>();

		protected override void Draw()
		{
			World[] worlds = SceneManager.GetActiveScenes();
			bool isOpen = true;
			if (!ImGUI.Begin("Hierarchy", ref isOpen))
			{
				EndDrawing(isOpen);
				return;
			}

			m_IsArrowKeyDown = ImGUI.IsKeyDown(Key.ArrowDown, Key.ArrowLeft, Key.ArrowUp, Key.ArrowRight);

			for(int i = 0; i < worlds.Length; i++)
			{
				World world = worlds[i];

				ImGUI.BeginGroup();
				ImGUI.SetNextItemOpen(true, ImGUI.Condition.Once);
				bool nodeOpen = ImGUI.TreeNode(world.Name, ImGUI.TreeNodeFlags.OpenOnDoubleClick | ImGUI.TreeNodeFlags.OpenOnArrow);

				// Inspect world
				if (ImGUI.IsItemClicked())
					InspectorView.Target = world;

				DrawContextMenu(world);

				if (!nodeOpen)
					continue;

				Entity[] entities = world.Entities;

				foreach (Entity entity in entities)
				{
					Transform transform = entity.GetComponent<Transform>();
					if (!transform || transform.Parent == null)
						DrawEntity(entity, transform);
				}

				ImGUI.TreePop();
				ImGUI.EndGroup();

				HandleDragDrop(world);
			}

			Vector2 remainingSpace = ImGUI.ContentRegionAvailable;
			remainingSpace.y = Math.Max(remainingSpace.y, 30);
			ImGUI.Dummy(remainingSpace);

			// If selected empty space, deselect inspector
			if (ImGUI.IsItemClicked() && ImGUI.IsWindowFocused())
				InspectorView.Target = null;
			HandleDragDrop(worlds[worlds.Length - 1]);

			EndDrawing(isOpen);
		}

		private void DrawEntity(Entity entity, Transform transform)
		{
			NameComponent nameComponent;
			if (!entity.TryGetComponent(out nameComponent))
			{
				nameComponent = entity.AddComponent<NameComponent>();
				nameComponent.Name = $"Entity {entity.ID}";
			}

			Transform[] children = transform?.GetChildren() ?? new Transform[0];

			bool isSelected = InspectorView.Target?.Equals(entity) ?? false;
			ImGUI.TreeNodeFlags flags = ImGUI.TreeNodeFlags.OpenOnDoubleClick;
			flags |= ImGUI.TreeNodeFlags.OpenOnArrow | ImGUI.TreeNodeFlags.SpanFullWidth;
			if (isSelected)
				flags |= ImGUI.TreeNodeFlags.Selected;
			flags |= ImGUI.TreeNodeFlags.NavLeftJumpsBackHere;
			flags |= ImGUI.TreeNodeFlags.OpenOnArrow;
			if (children.Length == 0)
				flags |= ImGUI.TreeNodeFlags.Leaf;

			if(ImGUI.TreeNode($"{nameComponent.Name}##{entity.World.ID}:{entity.ID}", flags))
			{
				if (ImGUI.IsItemHovered() &&
					(m_IsArrowKeyDown || ImGUI.IsMouseClicked(MouseButton.Left))) // Mouse click
					InspectorView.Target = entity;

				DrawContextMenu(entity);

				foreach (Transform child in children)
					DrawEntity(child.Entity, child);

				ImGUI.TreePop();
			}
		}

		private void EndDrawing(bool isOpen)
		{
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

			ImGUI.Separator();

			DrawContextMenuContents(entity.World);

			ImGUI.EndPopup();
		}

		private void DrawContextMenu(World world)
		{
			if (!ImGUI.BeginPopupContextItem($"Hierarchy:{world.ID}", ImGUI.PopupFlags.MouseButtonRight))
				return;

			DrawContextMenuContents(world);

			ImGUI.EndPopup();
		}

		private void DrawContextMenuContents(World world)
		{
			if (ImGUI.BeginMenu("New"))
			{
				if (ImGUI.Selectable("Entity"))
					world.CreateEntity().AddComponent<NameComponent>().Name = "Entity";

				if (ImGUI.Selectable("Camera"))
					CreateEntity(world, "Camera").AddComponent<Camera>();

				if (ImGUI.Selectable("Sound Source"))
					CreateEntity(world, "Sound Source").AddComponent<AudioSource>();

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

			if (SceneManager.MultipleScenesActive)
			{
				ImGUI.Separator();
				if (ImGUI.Selectable("Unload World"))
					SceneManager.Unload(world);
			}
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

		#region Drag & Drop
		private static readonly Dictionary<Type, Action<World, UUID>> DragDropFunctions = new Dictionary<Type, Action<World, UUID>>()
		{
			{ typeof(World), (world, resourceID) => SceneManager.Load(Resource.Get<World>(resourceID), SceneAddType.Additive) },
			{ typeof(Texture), HandleDropTexture },
			{ typeof(Model), HandleDropModel }
		};

		internal static void HandleDragDrop(World targetWorld)
		{
			if (!ImGUI.BeginDragDropTarget())
				return;

			object payload = ImGUI.AcceptDragDropPayload("ResourcePath", ImGUI.DragDropFlags.AcceptPeekOnly);
			if (payload == null) return;

			VFSFile payloadFile = (VFSFile)payload;
			if (!Resource.Exists(payloadFile.FullPath)) return; // Invalid path

			UUID resourceID = Resource.GetID(payloadFile.FullPath);
			Type payloadType = Resource.GetType(resourceID);

			if(DragDropFunctions.ContainsKey(payloadType))
			{
				ImGUI.AcceptDragDropPayload("ResourcePath");
				if (ImGUI.DragDropPayloadIsDelivery())
					DragDropFunctions[payloadType].Invoke(targetWorld, resourceID);
			}

			ImGUI.EndDragDropTarget();
		}

		private static void HandleDropTexture(World world, UUID resourceID)
		{
			Entity entity = world.CreateEntity();
			entity.AddComponent<NameComponent>().Name = "Sprite";
			entity.AddComponent<Transform>();

			SpriteRenderer renderer = entity.AddComponent<SpriteRenderer>();
			renderer.SpriteID = resourceID;
			renderer.Shader = Resource.Get<Shader>("assets://Shaders/Sprite.shader");
		}

		private static void AttachMeshToEntity(Entity e, Model.MeshData meshData)
		{
			MeshRenderer renderer = e.AddComponent<MeshRenderer>();
			renderer.Mesh = meshData.Mesh;
			renderer.Material = meshData.Material;
		}

		private static void HandleDropModel(World world, UUID resourceID)
		{
			Model model = Resource.Get<Model>(resourceID);

			Entity entity = world.CreateEntity();
			Transform parent = entity.AddComponent<Transform>();

			if (model.Meshes.Length > 1)
			{
				foreach (Model.MeshData meshData in model.Meshes)
				{
					Entity meshEntity = world.CreateEntity();
					meshEntity.AddComponent<Transform>().Parent = parent;

					AttachMeshToEntity(meshEntity, meshData);
				}
			}
			else if(model.Meshes.Length == 1)
				AttachMeshToEntity(entity, model.Meshes[0]);
		}
		#endregion
	}
}