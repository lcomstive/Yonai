#include <mono/jit/jit.h>
#include <AquaEngine/Scripting/InternalCalls.hpp>
#include <AquaEngine/Components/SpriteRenderer.hpp>

using namespace AquaEngine;
using namespace AquaEngine::Components;

SpriteRenderer* GetRenderer(uint64_t worldID, uint64_t entityID)
{
	World* world = World::GetWorld(worldID);
	return world ? world->GetEntity(entityID).GetComponent<SpriteRenderer>() : nullptr;
}

ADD_MANAGED_METHOD(SpriteRenderer, GetSprite, uint64_t, (uint64_t worldID, uint64_t entityID))
{
	SpriteRenderer* renderer = GetRenderer(worldID, entityID);
	return renderer ? renderer->Sprite : InvalidResourceID;
}

ADD_MANAGED_METHOD(SpriteRenderer, SetSprite, void, (uint64_t worldID, uint64_t entityID, uint64_t sprite))
{
	SpriteRenderer* renderer = GetRenderer(worldID, entityID);
	if (renderer)
		renderer->Sprite = sprite;
}

ADD_MANAGED_METHOD(SpriteRenderer, GetShader, uint64_t, (uint64_t worldID, uint64_t entityID))
{
	SpriteRenderer* renderer = GetRenderer(worldID, entityID);
	return renderer ? renderer->Shader : InvalidResourceID;
}

ADD_MANAGED_METHOD(SpriteRenderer, SetShader, void, (uint64_t worldID, uint64_t entityID, uint64_t shader))
{
	SpriteRenderer* renderer = GetRenderer(worldID, entityID);
	if (renderer)
		renderer->Shader = shader;
}

ADD_MANAGED_METHOD(SpriteRenderer, GetColour, void, (uint64_t worldID, uint64_t entityID, glm::vec4* colour))
{
	SpriteRenderer* renderer = GetRenderer(worldID, entityID);
	*colour = renderer ? renderer->Colour : glm::vec4();
}

ADD_MANAGED_METHOD(SpriteRenderer, SetColour, void, (uint64_t worldID, uint64_t entityID, glm::vec4* colour))
{
	SpriteRenderer* renderer = GetRenderer(worldID, entityID);
	if (renderer)
		renderer->Colour = *colour;
}
