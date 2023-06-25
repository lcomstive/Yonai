#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <AquaEngine/World.hpp>
#include <AquaEngine/Entity.hpp>
#include <glm/gtc/quaternion.hpp>
#include <AquaEngine/Graphics/Shader.hpp>
#include <AquaEngine/Components/Component.hpp>

namespace AquaEngine::Components
{
	struct Transform : public Component
	{
		Transform* Parent = nullptr;
		std::unordered_map<UUID, Transform*> Children = {};
		
		glm::vec3 Position = { 0, 0, 0 };
		glm::quat Rotation = glm::identity<glm::quat>();
		glm::vec3 Scale = { 1, 1, 1 };

		/// <summary>
		/// Sets the parent and handles changes in
		/// previous and new parent's children
		/// </summary>
		AquaAPI void SetParent(Transform* parent);

		AquaAPI void AddChild(Transform* child);
		AquaAPI void RemoveChild(Transform* child);

#pragma region Getters
		AquaAPI glm::mat4 GetModelMatrix(bool global = true);
		AquaAPI glm::vec3 GetGlobalScale();
		AquaAPI glm::vec3 GetGlobalPosition();
		AquaAPI glm::quat GetGlobalRotation();

		AquaAPI glm::vec3 Up();
		AquaAPI glm::vec3 Right();
		AquaAPI glm::vec3 Forward();

		template<typename T>
		T* GetComponentInChildren()
		{
			for (auto pair : Children)
			{
				T* component = pair.second->Entity.GetComponent<T>();
				if (!component)
					component = pair.second->GetComponentInChildren<T>();
				if (component)
					return component;
			}
			return nullptr;
		}

		template<typename T>
		void GetComponentsInChildren(std::vector<T*>& output)
		{
			for (auto pair : Children)
			{
				T* component = pair.second->Entity.GetComponent<T>();
				if (component)
					output.emplace_back(component);
				pair.second->GetComponentsInChildren<T>(output);
			}
		}

		template<typename T>
		std::vector<T*> GetComponentsInChildren()
		{
			std::vector<T*> components;
			GetComponentsInChildren<T>(components);
			return components;
		}
#pragma endregion
	};
}
