using System;
using System.Linq;
using System.Reflection;
using AquaEngine;

namespace AquaEditor.Views
{
	public interface ICustomInspector
	{
		void DrawInspector(object target);
	}

	public class InspectorView : View
	{
		private static object m_Target = null;

		/// <summary>
		/// Callback for displaying target properties.
		/// Defaults to <see cref="DrawDefaultInspector"/> if no <see cref="ICustomInspector"/> interface is found.
		/// </summary>
		private static Action<object> m_DrawFn;

		/// <summary>
		/// Target object to show properties for
		/// </summary>
		public static object Target
		{
			get => m_Target;
			set
			{
				if(m_Target == value) return; // No change

				m_Target = value;

				// Check for custom inspector
				ICustomInspector inspector = m_Target as ICustomInspector;
				if (inspector != null)
					m_DrawFn = inspector.DrawInspector;
				else // No ICustomInspector interface, use default draw method
					m_DrawFn = DrawDefaultInspector;
			}
		}

		[MenuItem("Window/Inspector")]
		private static void MenuCallback() => EditorUIService.Open<InspectorView>();

		protected override void Draw()
		{
			bool isOpen = true;
			if (ImGUI.Begin("Inspector", ref isOpen) && m_Target != null)
				m_DrawFn(m_Target);
			ImGUI.End();

			// Check if window requested to be closed
			if (!isOpen)
				EditorUIService.Close<InspectorView>();
		}

		private static void DrawDefaultInspector(object target)
		{
			Type type = target.GetType();
			const BindingFlags flags = BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance;
			MemberInfo[] members = type.GetMembers(flags);

			foreach (MemberInfo member in members)
			{
				ImGUI.Text(member.Name);
			}
		}
	}
}