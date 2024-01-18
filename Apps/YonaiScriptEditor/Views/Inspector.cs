using System;
using Yonai;
using Yonai.IO;
using System.Reflection;
using YonaiEditor.Systems;
using System.Collections.Generic;

namespace YonaiEditor.Views
{
	public class InspectorView : View
	{
		private object m_Target = null;
		private CustomInspector m_CurrentInspector = null;

		/// <summary>
		/// Target object to show properties for
		/// </summary>
		public static object Target
		{
			get => MainInstance?.m_Target;
			set => MainInstance?.SetTarget(value);
		}

		private static CustomInspector s_DefaultInspector = new CustomInspector();
		private static Dictionary<Type, Type> s_Inspectors = new Dictionary<Type, Type>();
		private static Dictionary<string, Type> s_FileExtensions = new Dictionary<string, Type>();

		private static InspectorView MainInstance = null;
		private static List<InspectorView> s_Instances = new List<InspectorView>();

		[MenuItem("Window/Inspector")]
		private static void MenuCallback() => EditorUIService.Open<InspectorView>();

		public void SetTarget(object value)
		{
			if (m_Target == value) return; // No change

			Type previousType = m_Target?.GetType();
			Type newType = value?.GetType();

			// Assign target
			m_Target = value;

			// Check for inspector type changes
			if (previousType == newType && newType != typeof(VFSFile))
			{
				if (m_CurrentInspector != null)
				{
					m_CurrentInspector.Target = m_Target;
					m_CurrentInspector.OnTargetChanged();
				}
				return; // Nothing further needs to be done
			}

			// Inform existing inspector of closure
			m_CurrentInspector?.Closed();

			if (newType == typeof(VFSFile))
			{
				VFSFile vfsFile = (VFSFile)value;
				m_CurrentInspector = CreateInspector(vfsFile);
			}
			else
				m_CurrentInspector = CreateInspector(newType);

			if (m_CurrentInspector != null)
			{
				m_CurrentInspector.Target = m_Target;
				m_CurrentInspector.m_Inspector = this;
				m_CurrentInspector.Opened();
				m_CurrentInspector.OnTargetChanged();
			}
		}

		protected override void Opened()
		{
			// This is the main instance, insert at beginning
			MainInstance = this;
			EditorService.StateChanged += OnEditorStateChanged;
		}

		protected override void Closed()
		{
			MainInstance = null;
			EditorService.StateChanged -= OnEditorStateChanged;
		}

		/// <summary>
		/// Creates a new inspector window, showing properties of <paramref name="target"/>
		/// </summary>
		/// <returns>
		/// Instance of inspector window, can be used to change
		/// the target (<see cref="SetTarget(object)"/>) or close
		/// the window (<see cref="Close(InspectorView)"/>)
		/// </returns>
		public static InspectorView Open(object target)
		{
			InspectorView inspector = new InspectorView();
			s_Instances.Add(inspector);
			inspector.SetTarget(target);
			return inspector;
		}

		public static void Close(InspectorView inspector)
		{
			if(s_Instances.Contains(inspector))
				s_Instances.Remove(inspector);
		}

		private void OnEditorStateChanged(EditorState oldState, EditorState newState)
		{
			if (oldState != EditorState.Edit && newState != EditorState.Edit)
				// Not entering or exiting play mode, don't need to deselect anything as world isn't changed
				return;

			// If targeting an entity in the world, clear target
			if (Target is Entity)
			{
				// Check active scenes for entity with matching ID, retarget if found
				UUID targetID = ((Entity)Target).ID;
				Target = null; // Clear target, incase not found

				foreach(World world in SceneManager.GetActiveScenes())
				{
					if (!world.HasEntity(targetID))
						continue;
					Target = world.GetEntity(targetID);
					break;
				}
			}

			if(this == MainInstance)
				foreach(InspectorView instance in s_Instances)
					instance.OnEditorStateChanged(oldState, newState);
		}

		protected override void Draw()
		{
			bool isOpen = true;
			int index = s_Instances.IndexOf(this);
			if (ImGUI.Begin($"Inspector##{index}", ref isOpen))
			{
				if (m_Target != null && m_CurrentInspector != null)
					m_CurrentInspector.DrawInspector();
				else if (m_Target != null)
					ImGUI.Text("No inspector found", Colour.Grey);
			}

			ImGUI.End();

			// Check if window requested to be closed
			if (!isOpen)
			{
				if (this == MainInstance)
					EditorUIService.Close<InspectorView>();
				else
					Close(this);
			}

			// If this is the main inspector instance, call Draw() on the other inspectors
			if(this == MainInstance)
				for(int i = s_Instances.Count - 1; i >= 0; i--)
					s_Instances[i].Draw();
		}

		internal static void GetCustomInspectors()
		{
			s_Inspectors.Clear();

			// Get all methods with the MenuItem attribute, in all assemblies
			Assembly[] assemblies = AppDomain.CurrentDomain.GetAssemblies();
			foreach (Assembly assembly in assemblies)
			{
				if (assembly.Location.Contains("Mono"))
					continue; // Don't check mono, system libs, and additional dependencies
				try
				{
					Type[] types = assembly.GetTypes();
					foreach (Type type in types)
					{
						CustomInspectorAttribute attribute = type.GetCustomAttribute<CustomInspectorAttribute>();
						if (attribute != null)
							AddCustomInspector(attribute, type);
					}
				}
				catch { }
			}
		}

		private static void AddCustomInspector(CustomInspectorAttribute attribute, Type inspectorType)
		{
			if (attribute.TargetType != null)
				s_Inspectors.Add(attribute.TargetType, inspectorType);

			if (attribute.FileExtensions == null)
				return;

			foreach(string extension in attribute.FileExtensions)
			{
				if(string.IsNullOrEmpty(extension)) continue;
				string finalExtension = extension.ToLower();
				if (finalExtension[0] != '.')
					finalExtension = $".{finalExtension}";
				s_FileExtensions.Add(finalExtension, inspectorType);
			}
		}

		/// <summary>
		/// Checks for valid inspector in <see cref="s_Inspectors"/> by type.
		/// Searches recursively down inheritance tree, from topmost down to base type
		/// </summary>
		internal static CustomInspector CreateInspector(Type targetType)
		{
			if(targetType == null) return s_DefaultInspector;
			if(s_Inspectors.TryGetValue(targetType, out Type inspectorType))
				return Activator.CreateInstance(inspectorType) as CustomInspector;
			return CreateInspector(targetType.BaseType);
		}

		internal static CustomInspector CreateInspector(VFSFile file) =>
			string.IsNullOrEmpty(file.Extension) || !s_FileExtensions.ContainsKey(file.Extension) ?
			null : Activator.CreateInstance(s_FileExtensions[file.Extension.ToLower()]) as CustomInspector;
	}
}