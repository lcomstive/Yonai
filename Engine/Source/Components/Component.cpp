#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Components/Component.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

using namespace AquaEngine;
using namespace AquaEngine::Scripting;
using namespace AquaEngine::Components;

ADD_MANAGED_METHOD(Component, SetEnabled, void, (void* handle, bool enabled))
{ ((Component*)handle)->Enabled = enabled; }