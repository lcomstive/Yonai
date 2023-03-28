#pragma once
#include <string>
#include <typeinfo>
#include <typeindex>
#include <filesystem>
#include <unordered_map>
#include <Views/View.hpp>
#include <ProjectInfo.hpp>
#include <AquaEngine/World.hpp>
#include <AquaEngine/Application.hpp>
#include <AquaEngine/Components/Camera.hpp>
#include <AquaEngine/Graphics/RenderTexture.hpp>
#include <AquaEngine/Scripting/ScriptEngine.hpp>
#include <AquaEngine/Systems/Global/RenderSystem.hpp>

namespace AquaEditor
{
	class EditorApp : public AquaEngine::Application
	{
		ProjectInfo m_ProjectInfo = {};
		std::filesystem::path m_ProjectPath;
		AquaEngine::Systems::RenderSystem* m_RenderSystem = nullptr;

		AquaEngine::World* m_CurrentScene = nullptr;

		std::unordered_map<std::type_index, View*> m_Views = {};

		void Draw();
		void DrawUI();

		void LoadScene();
		void InitialiseScripting();

		/// <summary>
		/// Reads project information from '/Project/project.json'.
		/// Stores data in m_ProjectInfo.
		/// </summary>
		void LoadProject();

		/// <summary>
		/// Loads editor script and loads the initial editor service
		/// </summary>
		void LaunchEditorService();

	protected:
		void Setup() override;
		void OnUpdate() override;
		void Cleanup() override;

	public:
		template<typename T>
		T* Get()
		{
			auto it = m_Views.find(typeid(T));
			return it == m_Views.end() ? (T*)nullptr : (T*)it->second;
		}

		template<typename T>
		T* Add()
		{
			if(!std::is_base_of<View, T>())
				return nullptr;
			if (Has<T>())
				return Get<T>();
			
			T* view = new T();
			m_Views.emplace(typeid(T), view);
			return (T*)view;
		}

		template<typename T>
		bool Has() { return m_Views.find(typeid(T)) != m_Views.end(); }

		template<typename T>
		void Remove()
		{
			if(!Has<T>())
				return;

			std::type_index type = typeid(T);
			delete m_Views[type];
			m_Views.erase(type);
		}
	};
}