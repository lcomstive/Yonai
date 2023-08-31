using System;
using Yonai;
using YonaiEditor.Views;
using System.Reflection;
using Yonai.Graphics;
using System.Linq;
using System.Collections.Generic;
using Yonai.IO;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace YonaiEditor.Inspectors
{
	[CustomInspector(typeof(Entity))]
	public class EntityInspector : CustomInspector
	{
		private Entity m_Target;

		private static List<Type> s_ComponentTypes = new List<Type>();

		public override void Opened()
		{
			SceneManager.WorldChanged += OnWorldChanged;
			if(s_ComponentTypes.Count == 0)
				CacheComponentTypes();
		}

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

			float width = ImGUI.ContentRegionAvailable.x;
			string name = nameComponent.Name;
			ImGUI.SetNextItemWidth(width);
			if (ImGUI.Input("##nameComponent", ref name))
				nameComponent.Name = name;

			Component[] components = m_Target.GetComponents();
			foreach(Component component in components)
				DrawComponentInspector(component);

			if (ImGUI.Button("Add Component", new Vector2(width, 0)))
				ImGUI.OpenPopup("ComponentMenu");

			DrawAddComponentMenu();
		}

		private void DrawAddComponentMenu()
		{
			ImGUI.SetNextWindowPos(ImGUI.GetCursorScreenPos());
			ImGUI.SetNextWindowSize(new Vector2(ImGUI.ContentRegionAvailable.x, 250));
			if (!ImGUI.BeginPopup("ComponentMenu"))
				return;

			foreach (Type type in s_ComponentTypes)
				if (ImGUI.Selectable(type.Name))
					m_Target.AddComponent(type);

			ImGUI.EndPopup();
		}

		private void DrawComponentInspector(Component component)
		{
			if (component is NameComponent)
				return; // Skip name component

			Type type = component.GetType();
			CustomInspector inspector = InspectorView.FindInspector(type);
			if (inspector == null)
				return; // Nothing to draw

			bool foldout = ImGUI.Foldout(type.Name, true);
			DrawComponentContextMenu(type, component);
    
			if (!foldout)
	           return;

			ImGUI.Indent();

			inspector.Target = component;
			inspector.OnTargetChanged();
			inspector.DrawInspector();

			ImGUI.Unindent();
		}

		private void DrawComponentContextMenu(Type type, Component component)
		{
			if (ImGUI.BeginPopupContextItem($"Inspector:{m_Target.ID}:{type.FullName}", ImGUI.PopupFlags.MouseButtonRight))
			{
				if (ImGUI.Selectable("Copy"))
					Clipboard.SetText(JsonConvert.SerializeObject(m_Target.GetComponent(type).OnSerialize()));
				if (ImGUI.Selectable("Paste"))
					m_Target.GetComponent(type).OnDeserialize(JsonConvert.DeserializeObject<JObject>(Clipboard.GetText()));
				ImGUI.Separator();
				if (ImGUI.Selectable("Remove"))
				{
					m_Target.RemoveComponent(type);
					return;
				}
				if (ImGUI.Selectable("Reset"))
				{
					m_Target.RemoveComponent(type);
					m_Target.AddComponent(type);
				}
				ImGUI.EndPopup();
			}
		}

		/// <summary>
		/// Searches assembly for all types that inherit from <see cref="Component"/>
		/// </summary>
		private void CacheComponentTypes()
		{
			Log.Debug("Caching component types for entity inspector");
			Type componentType = typeof(Component);
			Assembly[] assemblies = AppDomain.CurrentDomain.GetAssemblies();
			foreach (Assembly assembly in assemblies)
			{
				try
				{
					// Get all types in assembly that derive from Component
					foreach (Type type in assembly.GetTypes().Where(x => x.IsSubclassOf(componentType) && x.IsClass && !x.IsAbstract))
						s_ComponentTypes.Add(type);
				} catch { }
			}
			Log.Debug($"Found {s_ComponentTypes.Count} components");
		}
	}
}