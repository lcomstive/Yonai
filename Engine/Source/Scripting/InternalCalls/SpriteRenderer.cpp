#include <mono/jit/jit.h>
#include <Yonai/Scripting/InternalCalls.hpp>
#include <Yonai/Components/SpriteRenderer.hpp>

using namespace Yonai;
using namespace Yonai::Components;

ADD_MANAGED_GET_SET(SpriteRenderer, Sprite, uint64_t)
ADD_MANAGED_GET_SET(SpriteRenderer, Shader, uint64_t)

ADD_MANAGED_METHOD(SpriteRenderer, GetColour, void, (void* handle, glm::vec4* colour))
{ *colour = ((SpriteRenderer*)handle)->Colour; }

ADD_MANAGED_METHOD(SpriteRenderer, SetColour, void, (void* handle, glm::vec4* colour))
{ ((SpriteRenderer*)handle)->Colour = *colour; }
