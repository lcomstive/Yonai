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
					// Check that entity still exists in world, may have been removed
					// (e.g. when destroying a transform's parent entity)
					if(!world.HasEntity(entity.ID)) continue;

					Transform transform = entity.GetComponent<Transform>();
					if (!transform || transform.Parent == null)
						DrawEntity(entity, transform);
				}

				ImGUI.TreePop();
				ImGUI.EndGroup();

				HandleResourceDragDrop(world);
			}

			Vector2 remainingSpace = ImGUI.ContentRegionAvailable;
			remainingSpace.y = Math.Max(remainingSpace.y, 30);
			ImGUI.Dummy(remainingSpace);

			// If selected empty space, deselect inspector
			if (ImGUI.IsItemClicked() && ImGUI.IsWindowFocused())
				InspectorView.Target = null;
			HandleResourceDragDrop(worlds[worlds.Length - 1]);
			HandleEntityDrop(worlds[worlds.Length - 1], null);

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
			flags |= ImGUI.TreeNodeFlags.OpenOnArrow | ImGUI.TreeNodeFlags.SpanFullWidth |
						ImGUI.TreeNodeFlags.NavLeftJumpsBackHere;
			if (isSelected) flags |= ImGUI.TreeNodeFlags.Selected;
			if (children.Length == 0) flags |= ImGUI.TreeNodeFlags.Leaf;

			bool nodeOpen = ImGUI.TreeNode($"{nameComponent.Name}##{entity.World.ID}:{entity.ID}", flags);

			if (ImGUI.IsItemHovered() &&
				(m_IsArrowKeyDown || ImGUI.IsMouseClicked(MouseButton.Left))) // Mouse click
			{
				// Get entity directly from world instead of passing `entity`,
				//	as it becomes invalid once outside of this function's scope
				InspectorView.Target = entity.World.GetEntity(entity.ID);
			}

			bool entityModified = DrawContextMenu(entity);
			HandleEntityDragDrop(entity, transform, nameComponent);

			if (!nodeOpen || entityModified) return;

			foreach (Transform child in children)
				DrawEntity(child.Entity, child);

			ImGUI.TreePop();
		}

		private void EndDrawing(bool isOpen)
		{
			ImGUI.End();

			// Check if window requested to be closed
			if (!isOpen)
				EditorUIService.Close<HierarchyView>();
		}

		/// <returns>True if entity or it's transform has been modified</returns>
		private bool DrawContextMenu(Entity entity)
		{
			bool modified = false;
			if (!ImGUI.BeginPopupContextItem($"Hierarchy:{entity.World.ID}:{entity.ID}", ImGUI.PopupFlags.MouseButtonRight))
				return modified;
			// InspectorView.Target = entity;

			if (ImGUI.Selectable("Delete"))
			{
				try { entity.Destroy(); }
				catch(Exception e) { Log.Exception(e); }
				modified = true;
			}

			if (ImGUI.Selectable("Inspect"))
				InspectorView.Open(entity.World.GetEntity(entity.ID)); // Pass copy of entity as target

			ImGUI.Separator();

			DrawContextMenuContents(entity.World, true);

			ImGUI.EndPopup();

			return modified;
		}

		private void DrawContextMenu(World world)
		{
			if (!ImGUI.BeginPopupContextItem($"Hierarchy:{world.ID}", ImGUI.PopupFlags.MouseButtonRight))
				return;

			DrawContextMenuContents(world);

			ImGUI.EndPopup();
		}

		/// <param name="entityMenu">When true, this menu has been created as a part of right-clicking an entity</param>
		private void DrawContextMenuContents(World world, bool entityMenu = false)
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

			if (!entityMenu && ImGUI.Selectable("Inspect"))
				InspectorView.Open(world);

			if (!entityMenu && SceneManager.MultipleScenesActive)
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
			meshRenderer.MeshID = meshID;
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

		internal static void HandleResourceDragDrop(World targetWorld)
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

		private const string DragDropEntityName = "HierarchyEntity";
		private static void HandleEntityDragDrop(Entity entity, Transform transform, NameComponent nameComponent)
		{
			ImGUI.DragDropFlags dragFlags = ImGUI.DragDropFlags.SourceNoDisableHover;
			if (ImGUI.BeginDragDropSource(dragFlags))
			{
				ImGUI.Text(nameComponent.Name);
				ImGUI.SetDragDropPayload(DragDropEntityName, entity);
				ImGUI.EndDragDropSource();
			}

			HandleEntityDrop(entity.World, transform);
		}

		private static void HandleEntityDrop(World world, Transform transform)
		{
			if (!ImGUI.BeginDragDropTarget())
				return;

			Entity droppedEntity = ImGUI.AcceptDragDropPayload(DragDropEntityName, ImGUI.DragDropFlags.AcceptPeekOnly) as Entity;
			if (!droppedEntity) return; // Invalid entity
			if (droppedEntity.World != world) return; // Restrict drops to being in the same world

			Transform droppedTransform = droppedEntity.GetComponent<Transform>();
			if (!droppedTransform) return; // Cannot parent entity without transform

			ImGUI.AcceptDragDropPayload(DragDropEntityName);
			if (ImGUI.DragDropPayloadIsDelivery())
				droppedTransform.Parent = transform;

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
			renderer.MeshID = meshData.Mesh;
			renderer.MaterialID = meshData.Material;
		}

		private static void HandleDropModel(World world, UUID resourceID)
		{
			Model model = Resource.Get<Model>(resourceID);

			Entity entity = world.CreateEntity();
			entity.AddComponent<NameComponent>().Name = model.ResourcePath.FileNameWithoutExtension;
			Transform parent = entity.AddComponent<Transform>();

			if (model.Meshes.Length > 1)
			{
				foreach (Model.MeshData meshData in model.Meshes)
				{
					Entity meshEntity = world.CreateEntity();
					meshEntity.AddComponent<NameComponent>().Name = meshData.Mesh.ResourcePath.FileNameWithoutExtension;
					meshEntity.AddComponent<Transform>().Parent = parent;

					AttachMeshToEntity(meshEntity, meshData);
				}
			}
			else if (model.Meshes.Length == 1)
				AttachMeshToEntity(entity, model.Meshes[0]);
		}
		#endregion
	}
}