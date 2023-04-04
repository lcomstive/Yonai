#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Components/Component.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>
#include <AquaEngine/Components/MeshRenderer.hpp>

using namespace AquaEngine;
using namespace AquaEngine::Scripting;
using namespace AquaEngine::Components;

ADD_MANAGED_GET_SET(MeshRenderer, Mesh, uint64_t)
ADD_MANAGED_GET_SET(MeshRenderer, Material, uint64_t)
