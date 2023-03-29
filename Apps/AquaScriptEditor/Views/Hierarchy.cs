using System;
using AquaEngine;
using System.Linq;
using System.Collections.Generic;

namespace AquaEditor.Views
{
	public class HierarchyView : View
	{
		private List<World> m_Worlds = new List<World>();

		[MenuItem("Window/Hierarchy")]
		private static void MenuCallback() => EditorUIService.Open<HierarchyView>();

		protected override void Opened()
		{
			// Get all current worlds
			World[] worlds = World.GetAll();

			foreach (World world in worlds)
				m_Worlds.Add(world);

			// Listen to scene add and remove event
			SceneManager.WorldChanged += OnWorldChanged;
		}

		protected override void Closed() => SceneManager.WorldChanged -= OnWorldChanged;

		private void OnWorldChanged(World world, bool added)
		{
			if (added)
				m_Worlds.Add(world);
			else
				m_Worlds.Remove(world);
		}

		protected override void Draw()
		{
			bool isOpen = true;
			if (ImGUI.Begin("Hierarchy", ref isOpen))
			{
				foreach (World world in m_Worlds)
				{
					if (!ImGUI.Foldout(world.Name, true))
						continue;
					Entity[] entities = world.GetEntities();

					ImGUI.BeginChild($"{world.Name} [{world.ID}]",
						new Vector2(0, entities.Length * (ImGUI.TextLineHeight + 5)));

					foreach (Entity entity in entities)
					{
						ImGUI.Text($"[{entity.ID}]", Colour.Grey);

						if (!entity.TryGetComponent(out NameComponent nameComponent) ||
							string.IsNullOrEmpty(nameComponent.Name))
							continue;

						ImGUI.SameLine();
						ImGUI.Text(nameComponent.Name);
					}

					ImGUI.EndChild();
				}
			}
			ImGUI.End();

			// Check if window requested to be closed
			if (!isOpen)
				EditorUIService.Close<HierarchyView>();
		}
	}
}