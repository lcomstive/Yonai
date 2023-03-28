#include <imgui.h>
#include <glm/glm.hpp>
#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Graphics/Texture.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

using namespace AquaEngine;
using namespace AquaEngine::Graphics;

/// Window Begin / End ///
ADD_MANAGED_METHOD(ImGUI, _Begin, void, (MonoString* nameRaw, int flags), AquaEditor)
{
	char* name = mono_string_to_utf8(nameRaw);
	ImGui::Begin(name, nullptr, flags);
	mono_free(name);
}

ADD_MANAGED_METHOD(ImGUI, _BeginClosable, bool, (MonoString* nameRaw, bool* open, int flags), AquaEditor)
{
	char* name = mono_string_to_utf8(nameRaw);
	bool value = ImGui::Begin(name, open, flags);
	mono_free(name);
	return value;
}

ADD_MANAGED_METHOD(ImGUI, End, void, (), AquaEditor) { ImGui::End(); }

ADD_MANAGED_METHOD(ImGUI, _BeginChild, bool, (MonoString* idRaw, glm::vec2* sizeRaw, bool border, int flags), AquaEditor)
{
	char* id = mono_string_to_utf8(idRaw);
	ImVec2 size(sizeRaw->x, sizeRaw->y);

	bool value = ImGui::BeginChild(id, size, border, flags);

	mono_free(id);
	return value;
}

ADD_MANAGED_METHOD(ImGUI, EndChild, void, (), AquaEditor)
{
	ImGui::EndChild();
}

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
{
	return ImGui::BeginDisabled();
}

ADD_MANAGED_METHOD(ImGUI, EndDisabled, void, (), AquaEditor)
{
	return ImGui::EndDisabled();
}

/// CONTROLS ///
ADD_MANAGED_METHOD(ImGUI, _Text, void, (MonoString* labelRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	ImGui::Text("%s", label);
	mono_free(label);
}

ADD_MANAGED_METHOD(ImGUI, _TextColoured, void, (MonoString* labelRaw, glm::vec4* colourRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	ImVec4 colour(colourRaw->r, colourRaw->g, colourRaw->b, colourRaw->a);
	ImGui::TextColored(colour, "%s", label);
	mono_free(label);
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

char* GetInput(MonoString* valueRaw, int maxCharacters)
{
	char* value = new char[maxCharacters];

	// Fill with \0's
	memset(value, '\0', sizeof(char) * maxCharacters);

	// Copy input into buffer
	mono_unichar2* unichars = mono_string_chars(valueRaw);
	for (int i = 0; i < std::min(mono_string_length(valueRaw), maxCharacters); i++)
		value[i] = (char)unichars[i];

	return value;
}

// Input
ADD_MANAGED_METHOD(ImGUI, _InputText, bool, (MonoString* labelRaw, MonoString** valueRaw, int maxCharacters, int flags), AquaEditor)
{
	char* value = GetInput(*valueRaw, maxCharacters);
	char* label = mono_string_to_utf8(labelRaw);

	bool change = ImGui::InputText(label, value, maxCharacters, flags);

	// If text is edited, update valueRaw
	if (change)
		*valueRaw = mono_string_new(mono_domain_get(), value);

	delete[] value;
	mono_free(label);
	return change;
}

ADD_MANAGED_METHOD(ImGUI, _InputTextMultiline, bool, (MonoString* labelRaw, MonoString** valueRaw, int maxCharacters, int flags), AquaEditor)
{
	char* value = GetInput(*valueRaw, maxCharacters);
	char* label = mono_string_to_utf8(labelRaw);

	bool change = ImGui::InputTextMultiline(label, value, maxCharacters, ImVec2(0, 0), flags);

	// If text is edited, update valueRaw
	if (change)
		*valueRaw = mono_string_new(mono_domain_get(), value);

	delete[] value;
	mono_free(label);
	return change;
}

ADD_MANAGED_METHOD(ImGUI, _InputFloat, bool, (MonoString* labelRaw, float* value, float step, float stepFast, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::InputFloat(label, value, step, stepFast, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _InputFloat2, bool, (MonoString* labelRaw, glm::vec2* value, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::InputFloat2(label, &value->x, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _InputFloat3, bool, (MonoString* labelRaw, glm::vec3* value, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::InputFloat3(label, &value->x, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _InputFloat4, bool, (MonoString* labelRaw, glm::vec4* value, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::InputFloat4(label, &value->x, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _InputQuat, bool, (MonoString* labelRaw, glm::vec4* value, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::InputFloat4(label, &value->x, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _InputInt, bool, (MonoString* labelRaw, int* value, int step, int stepFast), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);

	bool output = ImGui::InputInt(label, value, step, stepFast);

	mono_free(label);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _InputInt2, bool, (MonoString* labelRaw, glm::ivec2* value), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	bool output = ImGui::InputInt2(label, &value->x);
	mono_free(label);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _InputInt3, bool, (MonoString* labelRaw, glm::ivec3* value), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	bool output = ImGui::InputInt3(label, &value->x);
	mono_free(label);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _InputInt4, bool, (MonoString* labelRaw, glm::ivec4* value), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	bool output = ImGui::InputInt4(label, &value->x);
	mono_free(label);
	return output;
}

// Drag 
ADD_MANAGED_METHOD(ImGUI, _DragFloat, bool, (MonoString* labelRaw, float* value, float speed, float min, float max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::DragFloat(label, value, speed, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _DragFloat2, bool, (MonoString* labelRaw, glm::vec2* value, float speed, float min, float max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::DragFloat2(label, &value->x, speed, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _DragFloat3, bool, (MonoString* labelRaw, glm::vec3* value, float speed, float min, float max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::DragFloat3(label, &value->x, speed, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _DragInt, bool, (MonoString* labelRaw, int* value, float speed, int min, int max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::DragInt(label, value, speed, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _DragInt2, bool, (MonoString* labelRaw, glm::ivec2* value, float speed, int min, int max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::DragInt2(label, &value->x, speed, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _DragInt3, bool, (MonoString* labelRaw, glm::ivec3* value, float speed, int min, int max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::DragInt3(label, &value->x, speed, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

// Slider
ADD_MANAGED_METHOD(ImGUI, SliderAngle, bool, (MonoString* labelRaw, float* value, float min, float max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::SliderAngle(label, value, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _SliderFloat, bool, (MonoString* labelRaw, float* value, float min, float max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::SliderFloat(label, value, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _SliderFloat2, bool, (MonoString* labelRaw, glm::vec2* value, float min, float max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::SliderFloat2(label, &value->x, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _SliderFloat3, bool, (MonoString* labelRaw, glm::vec3* value, float min, float max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::SliderFloat3(label, &value->x, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _SliderInt, bool, (MonoString* labelRaw, int* value, int min, int max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::SliderInt(label, value, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _SliderInt2, bool, (MonoString* labelRaw, glm::ivec2* value, int min, int max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::SliderInt2(label, &value->x, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _SliderInt3, bool, (MonoString* labelRaw, glm::ivec3* value, int min, int max, MonoString* formatRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::SliderInt3(label, &value->x, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

// Other 
ADD_MANAGED_METHOD(ImGUI, SameLine, void, (), AquaEditor)
{
	return ImGui::SameLine();
}

ADD_MANAGED_METHOD(ImGUI, Space, void, (), AquaEditor)
{
	return ImGui::Spacing();
}

ADD_MANAGED_METHOD(ImGUI, Button, bool, (MonoString* labelRaw), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	bool output = ImGui::Button(label);
	mono_free(label);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, Checkbox, bool, (MonoString* labelRaw, bool* checkState), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	bool output = ImGui::Checkbox(label, checkState);
	mono_free(label);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, ColourEdit3, bool, (MonoString* labelRaw, float* colour), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	bool output = ImGui::ColorEdit3(label, colour);
	mono_free(label);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, ColourEdit4, bool, (MonoString* labelRaw, float* colour), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	bool output = ImGui::ColorEdit4(label, colour);
	mono_free(label);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, SetTooltip, void, (MonoString* textRaw), AquaEditor)
{
	char* text = mono_string_to_utf8(textRaw);
	ImGui::SetTooltip("%s", text);
	mono_free(text);
}

ADD_MANAGED_METHOD(ImGUI, BeginTooltip, bool, (), AquaEditor) { return ImGui::BeginTooltip(); }
ADD_MANAGED_METHOD(ImGUI, EndTooltip, void, (), AquaEditor) { ImGui::EndTooltip(); }

ADD_MANAGED_METHOD(ImGUI, _PlotLines, void, (MonoString* labelRaw, MonoArray* pointsRaw, MonoString* overlayText, glm::vec2* graphSize), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* overlay = mono_string_to_utf8(overlayText);
	ImVec2 size(graphSize->x, graphSize->y);

	int pointsLength = (int)mono_array_length(pointsRaw);
	float* points = new float[pointsLength];

	for (int i = 0; i < pointsLength; i++)
		points[i] = mono_array_get(pointsRaw, float, i);

	ImGui::PlotLines(label, (const float*)points, pointsLength, 0, overlay, .01f, 1000.0f, size);

	// Free memory
	delete[] points;
	mono_free(label);
	mono_free(overlay);
}

ADD_MANAGED_METHOD(ImGUI, _DockSpace, void, (MonoString* idRaw, glm::vec2* size), AquaEditor)
{
	char* id = mono_string_to_utf8(idRaw);
	ImGui::DockSpace(ImGui::GetID(id), ImVec2(size->x, size->y));
	mono_free(id);
}

// VIEWPORT //
ADD_MANAGED_METHOD(ImGUI, _GetViewport, void,
	(unsigned int ID,
		glm::vec2* outPosition,
		glm::vec2* outSize,
		glm::vec2* outWorkPosition,
		glm::vec2* outWorkSize,
		int* outParentViewportID
		), AquaEditor)
{
	ImGuiViewport* viewport = ImGui::FindViewportByID(ID);
	if (!viewport)
		return;
	*outPosition = glm::vec2(viewport->Pos.x, viewport->Pos.y);
	*outSize = glm::vec2(viewport->Size.x, viewport->Size.y);
	*outWorkPosition = glm::vec2(viewport->WorkPos.x, viewport->WorkPos.y);
	*outWorkSize = glm::vec2(viewport->WorkSize.x, viewport->WorkSize.y);
	*outParentViewportID = viewport->ParentViewportId;
}

ADD_MANAGED_METHOD(ImGUI, GetMainViewportID, unsigned int, (), AquaEditor)
{ return ImGui::GetMainViewport()->ID; }

ADD_MANAGED_METHOD(ImGUI, GetWindowViewportID, unsigned int, (), AquaEditor)
{ return ImGui::GetWindowViewport()->ID; }

ADD_MANAGED_METHOD(ImGUI, _SetNextWindowPos, void, (glm::vec2* position), AquaEditor)
{ ImGui::SetNextWindowPos(ImVec2(position->x, position->y)); }

ADD_MANAGED_METHOD(ImGUI, _SetNextWindowSize, void, (glm::vec2* position), AquaEditor)
{ ImGui::SetNextWindowSize(ImVec2(position->x, position->y)); }

ADD_MANAGED_METHOD(ImGUI, SetNextWindowViewport, void, (unsigned int viewportID), AquaEditor)
{ ImGui::SetNextWindowViewport(viewportID); }

// STYLE //
ADD_MANAGED_METHOD(ImGUI, _PushStyleVar, void, (int flag, float value), AquaEditor)
{ ImGui::PushStyleVar(flag, value); }

ADD_MANAGED_METHOD(ImGUI, _PushStyleVarV, void, (int flag, glm::vec2* value), AquaEditor)
{ ImGui::PushStyleVar(flag, ImVec2(value->x, value->y)); }

ADD_MANAGED_METHOD(ImGUI, PopStyleVar, void, (int amount), AquaEditor)
{ ImGui::PopStyleVar(amount); }

// MENU //
ADD_MANAGED_METHOD(ImGUI, BeginMenuBar, bool, (), AquaEditor)
{ return ImGui::BeginMenuBar(); }

ADD_MANAGED_METHOD(ImGUI, EndMenuBar, void, (), AquaEditor)
{ ImGui::EndMenuBar(); }

ADD_MANAGED_METHOD(ImGUI, BeginMenu, bool, (MonoString* labelRaw, bool enabled), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	bool output = ImGui::BeginMenu(label, enabled);
	mono_free(label);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, EndMenu, void, (), AquaEditor)
{ ImGui::EndMenu(); }

ADD_MANAGED_METHOD(ImGUI, MenuItem, bool, (MonoString* labelRaw, MonoString* shortcutRaw, bool enabled), AquaEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* shortcut = mono_string_to_utf8(shortcutRaw);
	bool output = ImGui::MenuItem(label, shortcut, false, enabled);
	mono_free(label);
	return output;
}