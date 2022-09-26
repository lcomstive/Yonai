#pragma once
#include "View.hpp"
#include <AquaEngine/Components/Camera.hpp>
#include <AquaEngine/Components/FPSCamera.hpp>
#include <AquaEngine/Systems/CameraControlSystem.hpp>
#include <AquaEngine/Systems/Global/RenderSystem.hpp>

namespace AquaEditor
{
	struct ViewportView : public View
	{
		ViewportView();
		~ViewportView();

		void Update() override;
		void Draw() override;

	private:
		// Editor non-playmode specific world & components
		AquaEngine::World* m_EditorWorld = nullptr;		
		AquaEngine::Components::Camera* m_EditorCamera = nullptr;
		AquaEngine::Components::FPSCamera* m_EditorCameraControls = nullptr;
		AquaEngine::Systems::RenderSystem* m_RenderSystem = nullptr;
		AquaEngine::Systems::CameraControlSystem* m_CameraControlSystem = nullptr;
	};
}