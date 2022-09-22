#pragma once
#include <vector>
#include <AquaEngine/ResourceID.hpp>
#include <AquaEngine/Components/Component.hpp>

namespace AquaEngine::Components
{
	struct MeshRenderer : public Component
	{
		ResourceID Mesh;
		ResourceID Material;
	};
}