#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <mono/jit/jit.h>

using namespace std; 

typedef pair<const char*, const void*> MethodType; 

void _managed_internal_ImGUI_Begin(MonoString* nameRaw);
bool _managed_internal_ImGUI_BeginClosable(MonoString* nameRaw, bool* open);
void _managed_internal_ImGUIEnd();
bool _managed_internal_ImGUIIsItemHovered();
bool _managed_internal_ImGUIIsItemClicked();
bool _managed_internal_ImGUIIsItemEdited();
void _managed_internal_ImGUIBeginDisabled();
void _managed_internal_ImGUIEndDisabled();
void _managed_internal_ImGUIText(MonoString* labelRaw);
void _managed_internal_ImGUI_Image(unsigned int textureID, glm::vec2* size, glm::vec4* tint, glm::vec4* borderCol);
bool _managed_internal_ImGUI_ImageButton(unsigned int textureID, glm::vec2* size, int framePadding, glm::vec4* tint, glm::vec4* bgCol);
bool _managed_internal_ImGUI_DragFloat(MonoString* labelRaw, float* value, float speed, float min, float max, MonoString* formatRaw);
bool _managed_internal_ImGUI_DragFloat2(MonoString* labelRaw, glm::vec2* value, float speed, float min, float max, MonoString* formatRaw);
bool _managed_internal_ImGUI_DragFloat3(MonoString* labelRaw, glm::vec3* value, float speed, float min, float max, MonoString* formatRaw);
bool _managed_internal_ImGUI_DragInt(MonoString* labelRaw, int* value, float speed, int min, int max, MonoString* formatRaw);
bool _managed_internal_ImGUI_DragInt2(MonoString* labelRaw, glm::ivec2* value, float speed, int min, int max, MonoString* formatRaw);
bool _managed_internal_ImGUI_DragInt3(MonoString* labelRaw, glm::ivec3* value, float speed, int min, int max, MonoString* formatRaw);
bool _managed_internal_ImGUISliderAngle(MonoString* labelRaw, float* value, float min, float max, MonoString* formatRaw);
bool _managed_internal_ImGUI_SliderFloat(MonoString* labelRaw, float* value, float min, float max, MonoString* formatRaw);
bool _managed_internal_ImGUI_SliderFloat2(MonoString* labelRaw, glm::vec2* value, float min, float max, MonoString* formatRaw);
bool _managed_internal_ImGUI_SliderFloat3(MonoString* labelRaw, glm::vec3* value, float min, float max, MonoString* formatRaw);
bool _managed_internal_ImGUI_SliderInt(MonoString* labelRaw, int* value, int min, int max, MonoString* formatRaw);
bool _managed_internal_ImGUI_SliderInt2(MonoString* labelRaw, glm::ivec2* value, int min, int max, MonoString* formatRaw);
bool _managed_internal_ImGUI_SliderInt3(MonoString* labelRaw, glm::ivec3* value, int min, int max, MonoString* formatRaw);
void _managed_internal_ImGUISameLine();
void _managed_internal_ImGUISpace();
bool _managed_internal_ImGUIButton(MonoString* labelRaw);
bool _managed_internal_ImGUICheckbox(MonoString* labelRaw, bool* checkState);
bool _managed_internal_ImGUIColourEdit3(MonoString* labelRaw, float* colour);
bool _managed_internal_ImGUIColourEdit4(MonoString* labelRaw, float* colour);


const vector<pair<const char*, const void*>> _InternalMethods = {
	MethodType("AquaEditor.ImGUI::_Begin", (const void*)_managed_internal_ImGUI_Begin),
	MethodType("AquaEditor.ImGUI::_BeginClosable", (const void*)_managed_internal_ImGUI_BeginClosable),
	MethodType("AquaEditor.ImGUI::End", (const void*)_managed_internal_ImGUIEnd),
	MethodType("AquaEditor.ImGUI::IsItemHovered", (const void*)_managed_internal_ImGUIIsItemHovered),
	MethodType("AquaEditor.ImGUI::IsItemClicked", (const void*)_managed_internal_ImGUIIsItemClicked),
	MethodType("AquaEditor.ImGUI::IsItemEdited", (const void*)_managed_internal_ImGUIIsItemEdited),
	MethodType("AquaEditor.ImGUI::BeginDisabled", (const void*)_managed_internal_ImGUIBeginDisabled),
	MethodType("AquaEditor.ImGUI::EndDisabled", (const void*)_managed_internal_ImGUIEndDisabled),
	MethodType("AquaEditor.ImGUI::Text", (const void*)_managed_internal_ImGUIText),
	MethodType("AquaEditor.ImGUI::_Image", (const void*)_managed_internal_ImGUI_Image),
	MethodType("AquaEditor.ImGUI::_ImageButton", (const void*)_managed_internal_ImGUI_ImageButton),
	MethodType("AquaEditor.ImGUI::_DragFloat", (const void*)_managed_internal_ImGUI_DragFloat),
	MethodType("AquaEditor.ImGUI::_DragFloat2", (const void*)_managed_internal_ImGUI_DragFloat2),
	MethodType("AquaEditor.ImGUI::_DragFloat3", (const void*)_managed_internal_ImGUI_DragFloat3),
	MethodType("AquaEditor.ImGUI::_DragInt", (const void*)_managed_internal_ImGUI_DragInt),
	MethodType("AquaEditor.ImGUI::_DragInt2", (const void*)_managed_internal_ImGUI_DragInt2),
	MethodType("AquaEditor.ImGUI::_DragInt3", (const void*)_managed_internal_ImGUI_DragInt3),
	MethodType("AquaEditor.ImGUI::SliderAngle", (const void*)_managed_internal_ImGUISliderAngle),
	MethodType("AquaEditor.ImGUI::_SliderFloat", (const void*)_managed_internal_ImGUI_SliderFloat),
	MethodType("AquaEditor.ImGUI::_SliderFloat2", (const void*)_managed_internal_ImGUI_SliderFloat2),
	MethodType("AquaEditor.ImGUI::_SliderFloat3", (const void*)_managed_internal_ImGUI_SliderFloat3),
	MethodType("AquaEditor.ImGUI::_SliderInt", (const void*)_managed_internal_ImGUI_SliderInt),
	MethodType("AquaEditor.ImGUI::_SliderInt2", (const void*)_managed_internal_ImGUI_SliderInt2),
	MethodType("AquaEditor.ImGUI::_SliderInt3", (const void*)_managed_internal_ImGUI_SliderInt3),
	MethodType("AquaEditor.ImGUI::SameLine", (const void*)_managed_internal_ImGUISameLine),
	MethodType("AquaEditor.ImGUI::Space", (const void*)_managed_internal_ImGUISpace),
	MethodType("AquaEditor.ImGUI::Button", (const void*)_managed_internal_ImGUIButton),
	MethodType("AquaEditor.ImGUI::Checkbox", (const void*)_managed_internal_ImGUICheckbox),
	MethodType("AquaEditor.ImGUI::ColourEdit3", (const void*)_managed_internal_ImGUIColourEdit3),
	MethodType("AquaEditor.ImGUI::ColourEdit4", (const void*)_managed_internal_ImGUIColourEdit4)
};