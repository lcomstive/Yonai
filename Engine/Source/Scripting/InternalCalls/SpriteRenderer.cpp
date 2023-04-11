#include <mono/jit/jit.h>
#include <AquaEngine/Scripting/InternalCalls.hpp>
#include <AquaEngine/Components/SpriteRenderer.hpp>

using namespace AquaEngine;
using namespace AquaEngine::Components;

ADD_MANAGED_GET_SET(SpriteRenderer, Sprite, uint64_t)
ADD_MANAGED_GET_SET(SpriteRenderer, Shader, uint64_t)

ADD_MANAGED_METHOD(SpriteRenderer, GetColour, void, (void* handle, glm::vec4* colour))
{ *colour = ((SpriteRenderer*)handle)->Colour; }

ADD_MANAGED_METHOD(SpriteRenderer, SetColour, void, (void* handle, glm::vec4* colour))
{ ((SpriteRenderer*)handle)->Colour = *colour; }
