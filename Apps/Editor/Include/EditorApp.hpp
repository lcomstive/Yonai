#pragma once
#include <string>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <Views/View.hpp>
#include <AquaEngine/World.hpp>
#include <AquaEngine/Application.hpp>
#include <AquaEngine/Components/Camera.hpp>
#include <AquaEngine/Systems/Global/RenderSystem.hpp>
#include <AquaEngine/Graphics/RenderTexture.hpp>

namespace AquaEditor
{
	class EditorApp : public AquaEngine::WindowedApplication
	{
		std::string m_ProjectPath = "";
		AquaEngine::Systems::RenderSystem* m_RenderSystem = nullptr;

		AquaEngine::World* m_CurrentScene = nullptr;

		std::unordered_map<std::type_index, View*> m_Views = {};

		void DrawUI();
		void LoadScene();

	protected:
		void Setup() override;
		void OnDraw() override;
		void OnUpdate() override;

	public:
		template<typename T>
		T* Get()
		{
			auto it = m_Views.find(typeid(T));
			return it == m_Views.end() ? (T*)nullptr : (T*)*it;
		}

		template<typename T>
		T* Add()
		{
			if(!std::is_base_of<View, T>())
				return nullptr;
			
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