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
		/// <summary>
		/// Sets the parent and handles changes in
		/// previous and new parent's children
		/// </summary>
		YonaiAPI void SetParent(Transform* parent);

		YonaiAPI Transform* GetParent();

		YonaiAPI void AddChild(Transform* child);
		YonaiAPI void RemoveChild(Transform* child);

		YonaiAPI std::vector<Transform*> GetChildren();

		YonaiAPI void SetPosition(glm::vec3 position);
		/// <param name="euler">Euler rotation, in degrees</param>
		YonaiAPI void SetRotation(glm::vec3 euler);
		YonaiAPI void SetRotation(glm::quat rotation);
		YonaiAPI void SetScale(glm::vec3 scale);

		YonaiAPI void SetGlobalPosition(glm::vec3 position);
		YonaiAPI void SetGlobalRotation(glm::quat rotation);
		YonaiAPI void SetGlobalScale(glm::vec3 scale);

		/// <param name="euler">Euler rotation, in degrees</param>
		YonaiAPI void SetGlobalRotation(glm::vec3 euler, bool degrees = true);

		/// <summary>
		/// Updates local and global model matrices to 
		/// reflect changes in local and global position, rotation and scale
		/// </summary>
		/// <returns></returns>
		YonaiAPI void UpdateModelMatrices(bool force = false);

		YonaiAPI glm::mat4 GetModelMatrix(bool global = true);
		YonaiAPI void SetModelMatrix(glm::mat4& matrix, bool global);

		/// <summary>
		/// Converts a world coordinate to local space, relative to this transform
		/// </summary>
		YonaiAPI glm::vec3 InverseTransformPoint(const glm::vec3& point);

		/// <summary>
		/// Converts a direction from world space to local space, relative to this transform
		/// </summary>
		YonaiAPI glm::vec3 InverseTransformDirection(const glm::vec3& direction);

		#pragma region Getters
		YonaiAPI glm::vec3 GetScale();
		YonaiAPI glm::vec3 GetPosition();
		YonaiAPI glm::quat GetRotation();

		/// <returns>Euler angles, in degrees</returns>
		YonaiAPI glm::vec3 GetEulerRotation();

		YonaiAPI glm::vec3 GetGlobalScale();
		YonaiAPI glm::vec3 GetGlobalPosition();
		YonaiAPI glm::quat GetGlobalRotation();

		/// <returns>Euler angles, in degrees</returns>
		YonaiAPI glm::vec3 GetGlobalEulerRotation();

		YonaiAPI glm::vec3 Up();
		YonaiAPI glm::vec3 Right();
		YonaiAPI glm::vec3 Forward();

		YonaiAPI glm::vec3 GlobalUp();
		YonaiAPI glm::vec3 GlobalRight();
		YonaiAPI glm::vec3 GlobalForward();

		template<typename T>
		T* GetComponentInChildren()
		{
			for (auto pair : m_Children)
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
			for (auto pair : m_Children)
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

	private:
		// When true, local and/or global matrices need updating
		bool m_IsDirty = true;

		Transform* m_Parent = nullptr;
		std::unordered_map<UUID, Transform*> m_Children = {};

		glm::mat4 ModelMatrix = glm::mat4(1.0f);
		glm::mat4 GlobalModelMatrix = glm::mat4(1.0f);

		glm::vec3 Position = { 0, 0, 0 };
		glm::quat Rotation = glm::identity<glm::quat>();
		glm::vec3 Scale = { 1, 1, 1 };
	};
}
