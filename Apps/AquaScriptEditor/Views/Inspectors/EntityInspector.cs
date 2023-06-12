using System;
using AquaEngine;
using AquaEditor.Views;
using System.Reflection;
using AquaEngine.Graphics;
using System.Linq;

namespace AquaEditor.Inspectors
{
	[CustomInspector(typeof(Entity))]
	public class EntityInspector : CustomInspector
	{
		private Entity m_Target;

		public override void Opened() => SceneManager.WorldChanged += OnWorldChanged;
		public override void Closed() => SceneManager.WorldChanged -= OnWorldChanged;

		public override void OnTargetChanged() => m_Target = Target as Entity;

		private void OnWorldChanged(World world, bool added)
		{
			Entity target = InspectorView.Target as Entity;
			if (target && target.World.Equals(world))
				InspectorView.Target = null;
		}

		public override void DrawInspector()
		{
			// Make sure entity still exists in world, hasn't been deleted
			if(!m_Target.World.HasEntity(m_Target.ID))
			{
				InspectorView.Target = null;
				return;
			}

			NameComponent nameComponent;
			if(!m_Target.TryGetComponent(out nameComponent))
			{
				nameComponent = m_Target.AddComponent<NameComponent>();
				nameComponent.Name = "Entity";
			}

			string name = nameComponent.Name;
			if (ImGUI.Input("##nameComponent", ref name))
				nameComponent.Name = name;

			Component[] components = m_Target.GetComponents();
			foreach(Component component in components)
				DrawComponentInspector(component);
		}

		private void DrawComponentInspector(Component component)
		{
			if (component is NameComponent)
				return; // Skip name component

			Type type = component.GetType();
			CustomInspector inspector = InspectorView.FindInspector(type);
			if (inspector == null)
				return; // Nothing to draw

			if (!ImGUI.Foldout(type.Name, true))
				return;
			ImGUI.Indent();

			inspector.Target = component;
			inspector.OnTargetChanged();
			inspector.DrawInspector();

			ImGUI.Unindent();
		}
	}
}