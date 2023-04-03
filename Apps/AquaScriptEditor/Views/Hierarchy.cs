using AquaEngine;
using System.Collections.Generic;

namespace AquaEditor.Views
{
	public class HierarchyView : View
	{
		private World[] m_Worlds = null;

		[MenuItem("Window/Hierarchy")]
		private static void MenuCallback() => EditorUIService.Open<HierarchyView>();

		protected override void Opened()
		{
			// Get all current worlds
			m_Worlds = SceneManager.GetActiveScenes();

			// Listen to scene add and remove event
			SceneManager.WorldChanged += OnWorldChanged;
		}

		protected override void Closed() => SceneManager.WorldChanged -= OnWorldChanged;

		private void OnWorldChanged(World world, bool added) =>
			m_Worlds = SceneManager.GetActiveScenes();

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
						new Vector2(0, entities.Length * (ImGUI.TextLineHeight + 7.5f)));

					foreach (Entity entity in entities)
					{
						ImGUI.Text($"[{entity.ID}]", Colour.Grey);

						if (!entity.TryGetComponent(out NameComponent nameComponent) ||
							string.IsNullOrEmpty(nameComponent.Name))
							continue;

						ImGUI.SameLine();
						if (ImGUI.Button(nameComponent.Name))
						{
							try { InspectorView.Target = entity; }
							catch(System.Exception e) { Log.Exception(e); }
						}
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