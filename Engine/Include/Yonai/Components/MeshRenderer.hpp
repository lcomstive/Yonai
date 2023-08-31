#pragma once
#include <vector>
#include <Yonai/ResourceID.hpp>
#include <Yonai/Components/Component.hpp>

namespace Yonai::Components
{
	struct MeshRenderer : public Component
	{
		ResourceID Mesh;
		ResourceID Material;
	};
}