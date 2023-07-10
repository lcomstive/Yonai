#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <Yonai/World.hpp>
#include <Yonai/Entity.hpp>
#include <glm/gtc/quaternion.hpp>
#include <Yonai/Graphics/Shader.hpp>
#include <Yonai/Components/Component.hpp>

namespace Yonai::Components
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
		YonaiAPI void SetParent(Transform* parent);

		YonaiAPI void AddChild(Transform* child);
		YonaiAPI void RemoveChild(Transform* child);

#pragma region Getters
		YonaiAPI glm::mat4 GetModelMatrix(bool global = true);
		YonaiAPI glm::vec3 GetGlobalScale();
		YonaiAPI glm::vec3 GetGlobalPosition();
		YonaiAPI glm::quat GetGlobalRotation();

		YonaiAPI glm::vec3 Up();
		YonaiAPI glm::vec3 Right();
		YonaiAPI glm::vec3 Forward();

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
