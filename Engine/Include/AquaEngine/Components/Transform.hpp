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
		void SetParent(Transform* parent);

		void AddChild(Transform* child);
		void RemoveChild(Transform* child);

#pragma region Getters
		glm::mat4 GetModelMatrix(bool global = true);
		glm::vec3 GetGlobalScale();
		glm::vec3 GetGlobalPosition();
		glm::quat GetGlobalRotation();

		glm::vec3 Up();
		glm::vec3 Right();
		glm::vec3 Forward();

		template<typename T>
		T* GetComponentInChildren()
		{
			for (Transform* child : Children)
			{
				T* component = child->Entity.GetComponent<T>();
				if (!component)
					component = child->GetComponentInChildren<T>();
				if (component)
					return component;
			}
			return nullptr;
		}

		template<typename T>
		void GetComponentsInChildren(std::vector<T*>& output)
		{
			for (Transform* child : Children)
			{
				T* component = child->Entity.GetComponent<T>();
				if (component)
					output.emplace_back(component);
				child->GetComponentsInChildren<T>(output);
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
