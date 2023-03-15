#include <mono/jit/jit.h>
#include <AquaEngine/Scripting/InternalCalls.hpp>
#include <AquaEngine/Components/SpriteRenderer.hpp>

using namespace AquaEngine;
using namespace AquaEngine::Components;

SpriteRenderer* GetRenderer(unsigned int worldID, unsigned int entityID)
{
	World* world = World::GetWorld(worldID);
	return world ? world->GetEntity(entityID).GetComponent<SpriteRenderer>() : nullptr;
}

ADD_MANAGED_METHOD(SpriteRenderer, GetSprite, unsigned int, (unsigned int worldID, unsigned int entityID))
{
	SpriteRenderer* renderer = GetRenderer(worldID, entityID);
	return renderer ? renderer->Sprite : InvalidResourceID;
}

ADD_MANAGED_METHOD(SpriteRenderer, SetSprite, void, (unsigned int worldID, unsigned int entityID, unsigned int sprite))
{
	SpriteRenderer* renderer = GetRenderer(worldID, entityID);
	if (renderer)
		renderer->Sprite = sprite;
}

ADD_MANAGED_METHOD(SpriteRenderer, GetShader, unsigned int, (unsigned int worldID, unsigned int entityID))
{
	SpriteRenderer* renderer = GetRenderer(worldID, entityID);
	return renderer ? renderer->Shader : InvalidResourceID;
}

ADD_MANAGED_METHOD(SpriteRenderer, SetShader, void, (unsigned int worldID, unsigned int entityID, unsigned int shader))
{
	SpriteRenderer* renderer = GetRenderer(worldID, entityID);
	if (renderer)
		renderer->Shader = shader;
}

ADD_MANAGED_METHOD(SpriteRenderer, GetColour, void, (unsigned int worldID, unsigned int entityID, glm::vec4* colour))
{
	SpriteRenderer* renderer = GetRenderer(worldID, entityID);
	*colour = renderer ? renderer->Colour : glm::vec4();
}

ADD_MANAGED_METHOD(SpriteRenderer, SetColour, void, (unsigned int worldID, unsigned int entityID, glm::vec4* colour))
{
	SpriteRenderer* renderer = GetRenderer(worldID, entityID);
	if (renderer)
		renderer->Colour = *colour;
}
