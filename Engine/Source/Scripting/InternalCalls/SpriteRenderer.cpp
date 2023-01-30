#include <mono/jit/jit.h>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Components/SpriteRenderer.hpp>

using namespace AquaEngine;
using namespace AquaEngine::Components;

SpriteRenderer* GetRenderer(unsigned int worldID, unsigned int entityID)
{
	World* world = World::GetWorld(worldID);
	return world ? world->GetEntity(entityID).GetComponent<SpriteRenderer>() : nullptr;
}

unsigned int GetSprite(unsigned int worldID, unsigned int entityID)
{
	SpriteRenderer* renderer = GetRenderer(worldID, entityID);
	return renderer ? renderer->Sprite : InvalidResourceID;
}

void SetSprite(unsigned int worldID, unsigned int entityID, ResourceID sprite)
{
	SpriteRenderer* renderer = GetRenderer(worldID, entityID);
	if (renderer)
		renderer->Sprite = sprite;
}

unsigned int GetShader(unsigned int worldID, unsigned int entityID)
{
	SpriteRenderer* renderer = GetRenderer(worldID, entityID);
	return renderer ? renderer->Shader : InvalidResourceID;
}

void SetShader(unsigned int worldID, unsigned int entityID, ResourceID shader)
{
	SpriteRenderer* renderer = GetRenderer(worldID, entityID);
	if (renderer)
		renderer->Shader = shader;
}

void GetColour(unsigned int worldID, unsigned int entityID, glm::vec4* colour)
{
	SpriteRenderer* renderer = GetRenderer(worldID, entityID);
	*colour = renderer ? renderer->Colour : glm::vec4();
}

void SetColour(unsigned int worldID, unsigned int entityID, glm::vec4* colour)
{
	SpriteRenderer* renderer = GetRenderer(worldID, entityID);
	if (renderer)
		renderer->Colour = *colour;
}

#define ADD_SPRITE_RENDERER_INTERNAL_CALL(name) mono_add_internal_call("AquaEngine.SpriteRenderer::_aqua_internal_"#name, (const void*)name);

void AquaEngine::Scripting::Assembly::AddSpriteRendererInternalCalls()
{
	ADD_SPRITE_RENDERER_INTERNAL_CALL(GetSprite)
	ADD_SPRITE_RENDERER_INTERNAL_CALL(SetSprite)
	ADD_SPRITE_RENDERER_INTERNAL_CALL(GetShader)
	ADD_SPRITE_RENDERER_INTERNAL_CALL(SetShader)
	ADD_SPRITE_RENDERER_INTERNAL_CALL(SetColour)
	ADD_SPRITE_RENDERER_INTERNAL_CALL(GetColour)
}