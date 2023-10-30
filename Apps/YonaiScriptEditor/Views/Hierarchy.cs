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
		private Colour m_SelectedColour = new Colour(1, 1, 1, 0.25f);

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

			for(int i = 0; i < worlds.Length; i++)
			{
				World world = worlds[i];
				if (!ImGUI.Foldout(world.Name, true))
					continue;

				Entity[] entities = world.Entities;

				Vector2 size = i < worlds.Length - 1 ? new Vector2(0, entities.Length * 21) : Vector2.Zero;
				ImGUI.BeginChild($"{world.Name} [{world.ID}]", size);

				foreach (Entity entity in entities)
				{
					Transform transform = entity.GetComponent<Transform>();
					if (!transform || transform.Parent == null)
						DrawEntity(entity, transform);
				}

				ImGUI.EndChild();

				// If selected empty space and entity is currently selected, deselect it
				if (ImGUI.IsItemClicked() && InspectorView.Target is Entity)
					InspectorView.Target = null;

				HandleDragDrop(world);
				DrawContextMenu(world);
			}

			EndDrawing(isOpen);
		}

		private const float IndentWidth = 10;
		private void DrawEntity(Entity entity, Transform transform)
		{
			bool isSelected = InspectorView.Target?.Equals(entity) ?? false;
			ImGUI.Indent(IndentWidth);

			if (isSelected)
				ImGUI.PushStyleColour(ImGUI.StyleColour.ChildBg, m_SelectedColour);

			ImGUI.BeginGroup();

			NameComponent nameComponent;
			if (!entity.TryGetComponent(out nameComponent))
			{
				nameComponent = entity.AddComponent<NameComponent>();
				nameComponent.Name = $"Entity {entity.ID}";
			}

			Transform[] children = transform?.GetChildren() ?? new Transform[0];
			if (children.Length == 0)
			{
				ImGUI.HorizontalSpace(17.5f);
				ImGUI.Selectable(nameComponent.Name);
			}
			else if(ImGUI.Foldout(nameComponent.Name))
			{
				foreach (Transform child in children)
					DrawEntity(child.Entity, child);
			}

			if (ImGUI.IsItemHovered() && ImGUI.IsAnyMouseClicked())
				InspectorView.Target = entity;

			if (isSelected)
				ImGUI.PopStyleColour();

			ImGUI.EndGroup();
			ImGUI.Unindent(IndentWidth);

			DrawContextMenu(entity);
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

			if(SceneManager.MultipleScenesActive)
			{
				ImGUI.Separator();
				if (ImGUI.Selectable("Unload World"))
					SceneManager.Unload(world);
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

		#region Drag & Drop
		private static readonly Dictionary<Type, Action<World, UUID>> DragDropFunctions = new Dictionary<Type, Action<World, UUID>>()
		{
			{ typeof(World), (world, resourceID) => SceneManager.Load(Resource.Get<World>(resourceID), SceneAddType.Additive) },
			{ typeof(Texture), HandleDropTexture }
		};

		private void HandleDragDrop(World targetWorld)
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
		}
		#endregion
	}
}