#include <Yonai/Scripting/Assembly.hpp>
#include <Yonai/Components/Component.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>
#include <Yonai/Components/MeshRenderer.hpp>

using namespace Yonai;
using namespace Yonai::Scripting;
using namespace Yonai::Components;

ADD_MANAGED_GET_SET(MeshRenderer, Mesh, uint64_t)
ADD_MANAGED_GET_SET(MeshRenderer, Material, uint64_t)
