using AquaEngine;

namespace AquaEditor.Views
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
				foreach (World world in worlds)
				{
					if (!ImGUI.Foldout(world.Name, true))
						continue;
					Entity[] entities = world.GetEntities();

					ImGUI.BeginChild($"{world.Name} [{world.ID}]",
						new Vector2(0, entities.Length * 21));

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

						ImGUI.Text(nameComponent.Name);

						if(isSelected)
							ImGUI.PopStyleColour();

						ImGUI.EndChild();

						if(ImGUI.IsItemClicked())
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