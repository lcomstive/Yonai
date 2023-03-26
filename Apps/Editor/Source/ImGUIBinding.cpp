#include <imgui.h>
#include <glm/glm.hpp>
#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Graphics/Texture.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

using namespace AquaEngine;
using namespace AquaEngine::Graphics;

/// Window Begin / End ///
ADD_MANAGED_METHOD(ImGUI, _Begin, void, (MonoString* nameRaw), AquaEditor)
{
	char* name = mono_string_to_utf8(nameRaw);
	ImGui::Begin(name);
}

ADD_MANAGED_METHOD(ImGUI, _BeginClosable, bool, (MonoString* nameRaw, bool* open), AquaEditor)
{
	char* name = mono_string_to_utf8(nameRaw);
	return ImGui::Begin(name, open);
}

ADD_MANAGED_METHOD(ImGUI, End, void, (), AquaEditor) { ImGui::End(); }

/// STATE ///
ADD_MANAGED_METHOD(ImGUI, IsItemHovered, bool, (), AquaEditor)
{
	return ImGui::IsItemHovered();
}

ADD_MANAGED_METHOD(ImGUI, IsItemClicked, bool, (), AquaEditor)
{
	return ImGui::IsItemClicked();
}

ADD_MANAGED_METHOD(ImGUI, IsItemEdited, bool, (), AquaEditor)
{
	return ImGui::IsItemEdited();
}

ADD_MANAGED_METHOD(ImGUI, BeginDisabled, void, (), AquaEditor)
{ return ImGui::BeginDisabled(); }

ADD_MANAGED_METHOD(ImGUI, EndDisabled, void, (), AquaEditor)
{ return ImGui::EndDisabled(); }

/// CONTROLS ///
ADD_MANAGED_METHOD(ImGUI, Text, void, (MonoString* labelRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	ImGui::Text(label);
}

ADD_MANAGED_METHOD(ImGUI, _Image, void, (unsigned int textureID, glm::vec2* size, glm::vec4* tint, glm::vec4* borderCol), AquaEditor)
{
	ImVec2 imSize(size->x, size->y);
	ImVec4 imTint(tint->x, tint->g, tint->b, tint->a);
	ImVec4 imBorder(borderCol->x, borderCol->g, borderCol->b, borderCol->a);

	Texture* texture = Resource::Get<Texture>(textureID);
	ImGui::Image(
		(ImTextureID)(texture ? texture->GetID() : 0),
		imSize,
		ImVec2(0, 1), // UV0
		ImVec2(1, 0), // UV1
		imTint,
		imBorder
	);
}

ADD_MANAGED_METHOD(ImGUI, _ImageButton, bool, (unsigned int textureID, glm::vec2* size, int framePadding, glm::vec4* tint, glm::vec4* bgCol), AquaEditor)
{
	ImVec2 imSize(size->x, size->y);
	ImVec4 imTint(tint->x, tint->g, tint->b, tint->a);
	ImVec4 imBackground(bgCol->x, bgCol->g, bgCol->b, bgCol->a);

	Texture* texture = Resource::Get<Texture>(textureID);
	return ImGui::ImageButton(
		(ImTextureID)(texture ? texture->GetID() : 0),
		imSize,
		ImVec2(0, 1), // UV0
		ImVec2(1, 0), // UV1
		framePadding,
		imBackground,
		imTint
	);
}

// Drag 
ADD_MANAGED_METHOD(ImGUI, _DragFloat, bool, (MonoString* labelRaw, float* value, float speed, float min, float max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);
	return ImGui::DragFloat(label, value, speed, min, max, format);
}

ADD_MANAGED_METHOD(ImGUI, _DragFloat2, bool, (MonoString* labelRaw, glm::vec2* value, float speed, float min, float max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);
	return ImGui::DragFloat2(label, &value->x, speed, min, max, format);
}

ADD_MANAGED_METHOD(ImGUI, _DragFloat3, bool, (MonoString* labelRaw, glm::vec3* value, float speed, float min, float max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);
	return ImGui::DragFloat3(label, &value->x, speed, min, max, format);
}

ADD_MANAGED_METHOD(ImGUI, _DragInt, bool, (MonoString* labelRaw, int* value, float speed, int min, int max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);
	return ImGui::DragInt(label, value, speed, min, max, format);
}

ADD_MANAGED_METHOD(ImGUI, _DragInt2, bool, (MonoString* labelRaw, glm::ivec2* value, float speed, int min, int max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);
	return ImGui::DragInt2(label, &value->x, speed, min, max, format);
}

ADD_MANAGED_METHOD(ImGUI, _DragInt3, bool, (MonoString* labelRaw, glm::ivec3* value, float speed, int min, int max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);
	return ImGui::DragInt3(label, &value->x, speed, min, max, format);
}

// Slider
ADD_MANAGED_METHOD(ImGUI, SliderAngle, bool, (MonoString* labelRaw, float* value, float min, float max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);
	return ImGui::SliderAngle(label, value, min, max, format);
}

ADD_MANAGED_METHOD(ImGUI, _SliderFloat, bool, (MonoString* labelRaw, float* value, float min, float max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);
	return ImGui::SliderFloat(label, value, min, max, format);
}

ADD_MANAGED_METHOD(ImGUI, _SliderFloat2, bool, (MonoString* labelRaw, glm::vec2* value, float min, float max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);
	return ImGui::SliderFloat2(label, &value->x, min, max, format);
}

ADD_MANAGED_METHOD(ImGUI, _SliderFloat3, bool, (MonoString* labelRaw, glm::vec3* value, float min, float max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);
	return ImGui::SliderFloat3(label, &value->x, min, max, format);
}

ADD_MANAGED_METHOD(ImGUI, _SliderInt, bool, (MonoString* labelRaw, int* value, int min, int max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);
	return ImGui::SliderInt(label, value, min, max, format);
}

ADD_MANAGED_METHOD(ImGUI, _SliderInt2, bool, (MonoString* labelRaw, glm::ivec2* value, int min, int max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);
	return ImGui::SliderInt2(label, &value->x, min, max, format);
}

ADD_MANAGED_METHOD(ImGUI, _SliderInt3, bool, (MonoString* labelRaw, glm::ivec3* value, int min, int max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);
	return ImGui::SliderInt3(label, &value->x, min, max, format);
}

// Other 
ADD_MANAGED_METHOD(ImGUI, SameLine, void, (), AquaEditor)
{ return ImGui::SameLine(); }

ADD_MANAGED_METHOD(ImGUI, Space, void, (), AquaEditor)
{ return ImGui::Spacing(); }

ADD_MANAGED_METHOD(ImGUI, Button, bool, (MonoString* labelRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	return ImGui::Button(label);
}

ADD_MANAGED_METHOD(ImGUI, Checkbox, bool, (MonoString* labelRaw, bool* checkState), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	return ImGui::Checkbox(label, checkState);
}

ADD_MANAGED_METHOD(ImGUI, ColourEdit3, bool, (MonoString* labelRaw, float* colour), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	return ImGui::ColorEdit3(label, colour);
}

ADD_MANAGED_METHOD(ImGUI, ColourEdit4, bool, (MonoString* labelRaw, float* colour), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	return ImGui::ColorEdit4(label, colour);
}