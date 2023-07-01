using System;
using System.Reflection;
using System.Collections.Generic;

namespace AquaEngine.Systems
{
	public class BehaviourSystem : AquaSystem
	{
		private List<Type> m_BehaviourTypes = new List<Type>();

		protected override void Enabled()
		{
			// Gather all classes that implement IBehaviour
			Assembly[] assemblies = AppDomain.CurrentDomain.GetAssemblies();
			Type behaviourType = typeof(IBehaviour);
			foreach (Assembly assembly in assemblies)
			{
				if (assembly.FullName.StartsWith("System."))
					continue; // Skip system and internal assemblies

				foreach (Type type in assembly.GetTypes())
					if(behaviourType.IsAssignableFrom(type) && // Inherits from IBehaviour
						!type.IsInterface && !type.IsAbstract) // No interfaces or abstract classes
						m_BehaviourTypes.Add(type);
			}
		}

		protected override void Update()
		{
			World[] worlds = SceneManager.GetActiveScenes();
			foreach (World world in worlds)
			{
				foreach (Type type in m_BehaviourTypes)
				{
					Component[] components = world.GetComponents(type);
					foreach (Component component in components)
					{
						IBehaviour behaviour = (IBehaviour)component;
						try
						{
							if(behaviour.Enabled)
								behaviour.Update();
						}
						catch (Exception e)
						{
							Log.Exception(e);
							behaviour.Enabled = false;
						}
					}
				}
			}
		}
	}
}