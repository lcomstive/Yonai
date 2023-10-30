#pragma warning(push)
#pragma warning(disable : 4244) // Conversion from 'T' to float, possible loss of data
#pragma warning(disable : 4312) // 'type cast': conversion from 'unsigned int' to 'ImTextureID' of greater size

#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/glm.hpp>
#include <Yonai/Utils.hpp>
#include <Yonai/Resource.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <Yonai/Graphics/Texture.hpp>
#include <Yonai/Components/Camera.hpp>
#include <Yonai/Graphics/RenderTexture.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>
#include <YonaiEditor/Systems/ImGUISystem.hpp>

using namespace std;
using namespace glm;
using namespace Yonai;
using namespace Yonai::Graphics;
using namespace YonaiEditor::Systems;

// Helper Functions //
ImU32 ToU32(glm::vec4* colour) { return ImGui::ColorConvertFloat4ToU32(ImVec4(colour->x, colour->y, colour->z, colour->w)); }
ImVec2 ToVec2(glm::vec2* value) { return ImVec2(value->x, value->y); }
ImVec2 ToVec2(glm::ivec2* value) { return ImVec2(value->x, value->y); }
ImVec4 ToVec4(glm::vec4* value) { return ImVec4(value->x, value->y, value->z, value->w); }
ImColor ToColor(glm::vec4* value) { return ImColor(value->x, value->y, value->z, value->w); }

ADD_MANAGED_METHOD(ImGUI, SetCurrentContext, void, (), YonaiEditor)
{ ImGui::SetCurrentContext(SystemManager::Global()->Get<ImGUISystem>()->GetContext()); }

ADD_MANAGED_METHOD(ImGUI, SetIniFilename, void, (MonoString* pathRaw), YonaiEditor)
{
	ImGUISystem* imguiSystem = SystemManager::Global()->Get<ImGUISystem>();
	if (!imguiSystem)
	{
		spdlog::warn("ImGUI IniFilename not set - ImGUISystem not found in global systems");
		return;
	}
	char* name = mono_string_to_utf8(pathRaw);
	imguiSystem->m_IniFilepath = string(name);
	spdlog::trace("ImGUI IniFilename set to '{}'", name);
	mono_free(name);
}

ADD_MANAGED_METHOD(ImGUI, SetDisplayFramebufferScale, void, (float scaleX, float scaleY), YonaiEditor)
{ ImGui::GetIO().DisplayFramebufferScale = { scaleX, scaleY }; }

ADD_MANAGED_METHOD(ImGUI, LoadIniSettingsFromDisk, void, (MonoString* pathRaw), YonaiEditor)
{
	char* name = mono_string_to_utf8(pathRaw);
	ImGui::LoadIniSettingsFromDisk(name);
	spdlog::trace("ImGUI IniFilename loaded from '{}'", name);
	mono_free(name);
}

ADD_MANAGED_METHOD(ImGUI, SaveIniSettingsToDisk, void, (MonoString* pathRaw), YonaiEditor)
{
	char* name = mono_string_to_utf8(pathRaw);
	ImGui::SaveIniSettingsToDisk(name);
	spdlog::trace("ImGUI IniFilename saved to '{}'", name);
	mono_free(name);
}

ADD_MANAGED_METHOD(ImGUI, AddFontFromFile, void, (MonoString* pathRaw, int fontSize), YonaiEditor)
{
	char* name = mono_string_to_utf8(pathRaw);
	ImGui::GetIO().Fonts->AddFontFromFileTTF(name, fontSize);
	spdlog::trace("Added font '{}' to ImGUI", name);
	mono_free(name);
}

ADD_MANAGED_METHOD(ImGUI, AddFont, void, (MonoArray* dataRaw, int fontSize), YonaiEditor)
{
	vector<unsigned char> data;
	data.resize(mono_array_length(dataRaw));
	for (size_t i = 0; i < data.size(); i++)
		data[i] = mono_array_get(dataRaw, unsigned char, i);

	data.resize(mono_array_length(dataRaw));
	ImGui::GetIO().Fonts->AddFontFromMemoryTTF(data.data(), (int)data.size(), fontSize);
}

ADD_MANAGED_METHOD(ImGUI, SetFontGlobalScale, void, (float scale), YonaiEditor)
{ ImGui::GetIO().FontGlobalScale = scale; }

/// Window Begin / End ///
ADD_MANAGED_METHOD(ImGUI, _Begin, void, (MonoString* nameRaw, int flags), YonaiEditor)
{
	char* name = mono_string_to_utf8(nameRaw);
	ImGui::Begin(name, nullptr, flags);
	mono_free(name);
}

ADD_MANAGED_METHOD(ImGUI, _BeginClosable, bool, (MonoString* nameRaw, bool* open, int flags), YonaiEditor)
{
	char* name = mono_string_to_utf8(nameRaw);
	bool value = ImGui::Begin(name, open, flags);
	mono_free(name);
	return value;
}

ADD_MANAGED_METHOD(ImGUI, End, void, (), YonaiEditor) { ImGui::End(); }

ADD_MANAGED_METHOD(ImGUI, _BeginChild, bool, (MonoString* idRaw, glm::vec2* sizeRaw, bool border, int flags), YonaiEditor)
{
	char* id = mono_string_to_utf8(idRaw);
	ImVec2 size(sizeRaw->x, sizeRaw->y);

	bool value = ImGui::BeginChild(id, size, border, flags);

	mono_free(id);
	return value;
}

ADD_MANAGED_METHOD(ImGUI, EndChild, void, (), YonaiEditor)
{
	ImGui::EndChild();
}

/// STATE ///
ADD_MANAGED_METHOD(ImGUI, _IsItemHovered, bool, (int flags), YonaiEditor) { return ImGui::IsItemHovered(flags); }
ADD_MANAGED_METHOD(ImGUI, IsItemClicked, bool, (), YonaiEditor) { return ImGui::IsItemClicked(); }
ADD_MANAGED_METHOD(ImGUI, IsItemActive, bool, (), YonaiEditor) { return ImGui::IsItemActive(); }
ADD_MANAGED_METHOD(ImGUI, IsItemActivated, bool, (), YonaiEditor) { return ImGui::IsItemActivated(); }
ADD_MANAGED_METHOD(ImGUI, IsItemDeactivatedAfterEdit, bool, (), YonaiEditor) { return ImGui::IsItemDeactivatedAfterEdit(); }
ADD_MANAGED_METHOD(ImGUI, _IsMouseClicked, bool, (int button), YonaiEditor) { return ImGui::IsMouseClicked(button); }
ADD_MANAGED_METHOD(ImGUI, _IsMouseDoubleClicked, bool, (int button), YonaiEditor) { return ImGui::IsMouseDoubleClicked(button); }
ADD_MANAGED_METHOD(ImGUI, _IsMouseDragging, bool, (int button), YonaiEditor) { return ImGui::IsMouseDragging(button); }
ADD_MANAGED_METHOD(ImGUI, _IsMouseReleased, bool, (int button), YonaiEditor) { return ImGui::IsMouseReleased(button); }
ADD_MANAGED_METHOD(ImGUI, IsItemEdited, bool, (), YonaiEditor) { return ImGui::IsItemEdited(); }
ADD_MANAGED_METHOD(ImGUI, BeginDisabled, void, (), YonaiEditor) { return ImGui::BeginDisabled(); }
ADD_MANAGED_METHOD(ImGUI, EndDisabled, void, (), YonaiEditor) { return ImGui::EndDisabled(); }
ADD_MANAGED_METHOD(ImGUI, SetItemDefaultFocus, void, (), YonaiEditor) { ImGui::SetItemDefaultFocus(); }

ADD_MANAGED_METHOD(ImGUI, _IsKeyDown, bool, (int key), YonaiEditor)
{ return ImGui::IsKeyDown((ImGuiKey)key); }

ADD_MANAGED_METHOD(ImGUI, _IsKeyPressed, bool, (int key), YonaiEditor)
{ return ImGui::IsKeyPressed((ImGuiKey)key, false); }

ADD_MANAGED_METHOD(ImGUI, _IsKeyReleased, bool, (int key), YonaiEditor)
{ return ImGui::IsKeyReleased((ImGuiKey)key); }

ADD_MANAGED_METHOD(ImGUI, PushID, void, (MonoString* idRaw), YonaiEditor)
{
	char* id = mono_string_to_utf8(idRaw);
	ImGui::PushID(id);
	mono_free(id);
}

ADD_MANAGED_METHOD(ImGUI, PopID, void, (), YonaiEditor) { ImGui::PopID(); }

ADD_MANAGED_METHOD(ImGUI, _CalculateTextWidth, void, (MonoString* textRaw, glm::vec2* output), YonaiEditor)
{
	char* text = mono_string_to_utf8(textRaw);
	ImVec2 size = ImGui::CalcTextSize(text, 0, true);
	*output = glm::vec2(size.x, size.y);
	mono_free(text);
}

ADD_MANAGED_METHOD(ImGUI, Foldout, bool, (MonoString* labelRaw, bool openByDefault), YonaiEditor)
{
	int flags = 0;
	if (openByDefault)
		flags |= ImGuiTreeNodeFlags_DefaultOpen;

	char* label = mono_string_to_utf8(labelRaw);
	bool output = ImGui::CollapsingHeader(label, flags);
	mono_free(label);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, Indent, void, (float width), YonaiEditor)
{ ImGui::Indent(width); }

ADD_MANAGED_METHOD(ImGUI, Unindent, void, (float width), YonaiEditor)
{ ImGui::Unindent(width); }

ADD_MANAGED_METHOD(ImGUI, GetCursorPosX, float, (), YonaiEditor) { return ImGui::GetCursorPosX(); }
ADD_MANAGED_METHOD(ImGUI, GetCursorPosY, float, (), YonaiEditor) { return ImGui::GetCursorPosY(); }
ADD_MANAGED_METHOD(ImGUI, _GetCursorPos, void, (glm::vec2* output), YonaiEditor)
{
	ImVec2 cursorPos = ImGui::GetCursorPos();
	*output = glm::vec2(cursorPos.x, cursorPos.y);
}

ADD_MANAGED_METHOD(ImGUI,  GetCursorScreenPosX, float, (), YonaiEditor) { return ImGui::GetCursorScreenPos().x; }
ADD_MANAGED_METHOD(ImGUI,  GetCursorScreenPosY, float, (), YonaiEditor) { return ImGui::GetCursorScreenPos().y; }
ADD_MANAGED_METHOD(ImGUI, _GetCursorScreenPos, void, (glm::vec2* output), YonaiEditor)
{
	ImVec2 cursorPos = ImGui::GetCursorScreenPos();
	*output = glm::vec2(cursorPos.x, cursorPos.y);
}

ADD_MANAGED_METHOD(ImGUI, _SetCursorPos, void, (glm::vec2* input), YonaiEditor) { ImGui::SetCursorPos(ImVec2(input->x, input->y)); }
ADD_MANAGED_METHOD(ImGUI, _SetCursorScreenPos, void, (glm::vec2* input), YonaiEditor) { ImGui::SetCursorScreenPos(ImVec2(input->x, input->y)); }
ADD_MANAGED_METHOD(ImGUI, SetCursorPosX, void, (float value), YonaiEditor) { ImGui::SetCursorPosX(value); }
ADD_MANAGED_METHOD(ImGUI, SetCursorPosY, void, (float value), YonaiEditor) { ImGui::SetCursorPosY(value); }

ADD_MANAGED_METHOD(ImGUI, _GetMouseCursor, int, (), YonaiEditor) { return (int)ImGui::GetMouseCursor(); }
ADD_MANAGED_METHOD(ImGUI, _SetMouseCursor, void, (int type), YonaiEditor) { ImGui::SetMouseCursor(type); }

ADD_MANAGED_METHOD(ImGUI, BeginGroup, void, (), YonaiEditor) { ImGui::BeginGroup(); }
ADD_MANAGED_METHOD(ImGUI, EndGroup, void, (), YonaiEditor) { ImGui::EndGroup(); }

// DRAG & DROP ///
ADD_MANAGED_METHOD(ImGUI, _BeginDragDropSource, bool, (int flags), YonaiEditor)
{ return ImGui::BeginDragDropSource(flags); }

ADD_MANAGED_METHOD(ImGUI, EndDragDropSource, void, (), YonaiEditor)
{ return ImGui::EndDragDropSource(); }

ADD_MANAGED_METHOD(ImGUI, BeginDragDropTarget, bool, (), YonaiEditor)
{ return ImGui::BeginDragDropTarget(); }

ADD_MANAGED_METHOD(ImGUI, EndDragDropTarget, void, (), YonaiEditor)
{ return ImGui::EndDragDropTarget(); }

ADD_MANAGED_METHOD(ImGUI, SetDragDropPayload, void, (MonoString* labelRaw, MonoObject* payload), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	ImGui::SetDragDropPayload(label, payload, mono_object_get_size(payload));
	mono_free(label);
}

ADD_MANAGED_METHOD(ImGUI, GetDragDropPayload, MonoObject*, (MonoString* labelRaw, int flags), YonaiEditor)
{
	const ImGuiPayload* payload = ImGui::GetDragDropPayload();
	return payload ? (MonoObject*)payload->Data : nullptr;
}

ADD_MANAGED_METHOD(ImGUI, GetDragDropPayloadType, MonoString*, (), YonaiEditor)
{
	const ImGuiPayload* payload = ImGui::GetDragDropPayload();
	return payload ? mono_string_new(mono_domain_get(), payload->DataType) : nullptr;
}

ADD_MANAGED_METHOD(ImGUI, IsDragDropPayloadType, bool, (MonoString* labelRaw), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	const ImGuiPayload* payload = ImGui::GetDragDropPayload();
	bool output = payload ? payload->IsDataType(label) : false;
	mono_free(label);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _AcceptDragDropPayload, MonoObject*, (MonoString* labelRaw, int flags), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(label, flags);
	mono_free(label);
	return payload ? (MonoObject*)payload->Data : nullptr;
}

ADD_MANAGED_METHOD(ImGUI, DragDropPayloadIsDelivery, bool, (), YonaiEditor)
{
	const ImGuiPayload* payload = ImGui::GetDragDropPayload();
	return payload ? payload->IsDelivery() : false;
}

/// CONTROLS ///
ADD_MANAGED_METHOD(ImGUI, _Text, void, (MonoString* labelRaw), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	ImGui::Text("%s", label);
	mono_free(label);
}

ADD_MANAGED_METHOD(ImGUI, _TextColoured, void, (MonoString* labelRaw, glm::vec4* colour), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	ImGui::TextColored(ToVec4(colour), "%s", label);
	mono_free(label);
}

ADD_MANAGED_METHOD(ImGUI, _Image, void, (uint64_t textureID, glm::vec2* size, glm::vec4* tint, glm::vec4* borderCol), YonaiEditor)
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

ADD_MANAGED_METHOD(ImGUI, _ImageRenderTexture, void, (void* handle, glm::vec2* size, glm::vec4* tint, glm::vec4* borderCol), YonaiEditor)
{
	ImVec2 imSize(size->x, size->y);
	ImVec4 imTint(tint->x, tint->g, tint->b, tint->a);
	ImVec4 imBorder(borderCol->x, borderCol->g, borderCol->b, borderCol->a);

	RenderTexture* texture = (RenderTexture*)handle;
	ImGui::Image(
		(ImTextureID)(texture ? texture->GetID() : 0),
		imSize,
		ImVec2(0, 1), // UV0
		ImVec2(1, 0), // UV1
		imTint,
		imBorder
	);
}

ADD_MANAGED_METHOD(ImGUI, _ImageButton, bool, (uint64_t textureID, glm::vec2* size, int framePadding, glm::vec4* tint, glm::vec4* bgCol), YonaiEditor)
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
ADD_MANAGED_METHOD(ImGUI, _InputText, bool, (MonoString* labelRaw, MonoString** valueRaw, int maxCharacters, int flags), YonaiEditor)
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

ADD_MANAGED_METHOD(ImGUI, _InputTextMultiline, bool, (MonoString* labelRaw, MonoString** valueRaw, int maxCharacters, glm::vec2* size, int flags), YonaiEditor)
{
	char* value = GetInput(*valueRaw, maxCharacters);
	char* label = mono_string_to_utf8(labelRaw);

	bool change = ImGui::InputTextMultiline(label, value, maxCharacters, ToVec2(size), flags);

	// If text is edited, update valueRaw
	if (change)
		*valueRaw = mono_string_new(mono_domain_get(), value);

	delete[] value;
	mono_free(label);
	return change;
}

ADD_MANAGED_METHOD(ImGUI, _InputFloat, bool, (MonoString* labelRaw, float* value, float step, float stepFast, MonoString* formatRaw), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::InputFloat(label, value, step, stepFast, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _InputFloat2, bool, (MonoString* labelRaw, glm::vec2* value, MonoString* formatRaw), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::InputFloat2(label, &value->x, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _InputFloat3, bool, (MonoString* labelRaw, glm::vec3* value, MonoString* formatRaw), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::InputFloat3(label, &value->x, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _InputFloat4, bool, (MonoString* labelRaw, glm::vec4* value, MonoString* formatRaw), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::InputFloat4(label, &value->x, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _InputQuat, bool, (MonoString* labelRaw, glm::vec4* value, MonoString* formatRaw), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::InputFloat4(label, &value->x, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _InputInt, bool, (MonoString* labelRaw, int* value, int step, int stepFast), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);

	bool output = ImGui::InputInt(label, value, step, stepFast);

	mono_free(label);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _InputInt2, bool, (MonoString* labelRaw, glm::ivec2* value), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	bool output = ImGui::InputInt2(label, &value->x);
	mono_free(label);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _InputInt3, bool, (MonoString* labelRaw, glm::ivec3* value), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	bool output = ImGui::InputInt3(label, &value->x);
	mono_free(label);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _InputInt4, bool, (MonoString* labelRaw, glm::ivec4* value), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	bool output = ImGui::InputInt4(label, &value->x);
	mono_free(label);
	return output;
}

// Drag 
ADD_MANAGED_METHOD(ImGUI, _DragFloat, bool, (MonoString* labelRaw, float* value, float speed, float min, float max, MonoString* formatRaw), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::DragFloat(label, value, speed, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _DragFloat2, bool, (MonoString* labelRaw, glm::vec2* value, float speed, float min, float max, MonoString* formatRaw), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::DragFloat2(label, &value->x, speed, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _DragFloat3, bool, (MonoString* labelRaw, glm::vec3* value, float speed, float min, float max, MonoString* formatRaw), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::DragFloat3(label, &value->x, speed, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}


ADD_MANAGED_METHOD(ImGUI, _DragFloat4, bool, (MonoString* labelRaw, glm::vec4* value, float speed, float min, float max, MonoString* formatRaw), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::DragFloat4(label, &value->x, speed, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _DragInt, bool, (MonoString* labelRaw, int* value, float speed, int min, int max, MonoString* formatRaw), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::DragInt(label, value, speed, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _DragInt2, bool, (MonoString* labelRaw, glm::ivec2* value, float speed, int min, int max, MonoString* formatRaw), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::DragInt2(label, &value->x, speed, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _DragInt3, bool, (MonoString* labelRaw, glm::ivec3* value, float speed, int min, int max, MonoString* formatRaw), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::DragInt3(label, &value->x, speed, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

// Slider
ADD_MANAGED_METHOD(ImGUI, SliderAngle, bool, (MonoString* labelRaw, float* value, float min, float max, MonoString* formatRaw), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::SliderAngle(label, value, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _SliderFloat, bool, (MonoString* labelRaw, float* value, float min, float max, MonoString* formatRaw), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::SliderFloat(label, value, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _SliderFloat2, bool, (MonoString* labelRaw, glm::vec2* value, float min, float max, MonoString* formatRaw), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::SliderFloat2(label, &value->x, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _SliderFloat3, bool, (MonoString* labelRaw, glm::vec3* value, float min, float max, MonoString* formatRaw), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::SliderFloat3(label, &value->x, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _SliderInt, bool, (MonoString* labelRaw, int* value, int min, int max, MonoString* formatRaw), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::SliderInt(label, value, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _SliderInt2, bool, (MonoString* labelRaw, glm::ivec2* value, int min, int max, MonoString* formatRaw), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::SliderInt2(label, &value->x, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _SliderInt3, bool, (MonoString* labelRaw, glm::ivec3* value, int min, int max, MonoString* formatRaw), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* format = mono_string_to_utf8(formatRaw);

	bool output = ImGui::SliderInt3(label, &value->x, min, max, format);

	mono_free(label);
	mono_free(format);
	return output;
}

// Other 
ADD_MANAGED_METHOD(ImGUI, Space, void, (), YonaiEditor) { return ImGui::Spacing(); }
ADD_MANAGED_METHOD(ImGUI, _Dummy, void, (glm::vec2* size), YonaiEditor) { ImGui::Dummy(ToVec2(size)); }
ADD_MANAGED_METHOD(ImGUI, _GetTextLineHeight, float, (), YonaiEditor) { return ImGui::GetTextLineHeight(); }
ADD_MANAGED_METHOD(ImGUI, SameLine, void, (float offset, float spacing), YonaiEditor) { return ImGui::SameLine(offset, spacing); }

ADD_MANAGED_METHOD(ImGUI, _Button, bool, (MonoString* labelRaw, glm::vec2* sizeRaw), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	ImVec2 size(sizeRaw->x, sizeRaw->y);
	bool output = ImGui::Button(label, size);
	mono_free(label);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _InvisibleButton, bool, (MonoString* labelRaw, glm::vec2* size), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	bool output = ImGui::InvisibleButton(label, ImVec2(size->x, size->y));
	mono_free(label);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _Selectable, bool, (MonoString* labelRaw, bool selected, int flags, glm::vec2* size), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	ImVec2 imSize(size->x, size->y);
	bool output = ImGui::Selectable(label, selected, flags, imSize);
	mono_free(label);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, Checkbox, bool, (MonoString* labelRaw, bool* checkState), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	bool output = ImGui::Checkbox(label, checkState);
	mono_free(label);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, ColourEdit3, bool, (MonoString* labelRaw, float* colour), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	bool output = ImGui::ColorEdit3(label, colour);
	mono_free(label);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, ColourEdit4, bool, (MonoString* labelRaw, float* colour), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	bool output = ImGui::ColorEdit4(label, colour);
	mono_free(label);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, SetTooltip, void, (MonoString* textRaw), YonaiEditor)
{
	char* text = mono_string_to_utf8(textRaw);
	ImGui::SetTooltip("%s", text);
	mono_free(text);
}

ADD_MANAGED_METHOD(ImGUI, BeginTooltip, bool, (), YonaiEditor) { return ImGui::BeginTooltip(); }
ADD_MANAGED_METHOD(ImGUI, EndTooltip, void, (), YonaiEditor) { ImGui::EndTooltip(); }

ADD_MANAGED_METHOD(ImGUI, _BeginPopupContextItem, bool, (MonoString* idRaw, int flags), YonaiEditor)
{
	char* id = mono_string_to_utf8(idRaw);
	bool output = ImGui::BeginPopupContextItem(id, flags);
	mono_free(id);
	return output;
}
ADD_MANAGED_METHOD(ImGUI, _BeginPopup, bool, (MonoString* idRaw, int flags), YonaiEditor)
{
	char* id = mono_string_to_utf8(idRaw);
	bool output = ImGui::BeginPopup(id, flags);
	mono_free(id);
	return output;
}
ADD_MANAGED_METHOD(ImGUI, EndPopup, void, (), YonaiEditor) { ImGui::EndPopup(); }

ADD_MANAGED_METHOD(ImGUI, IsPopupOpen, bool, (MonoString* idRaw), YonaiEditor)
{
	char* id = mono_string_to_utf8(idRaw);
	bool output = ImGui::IsPopupOpen(id);
	mono_free(id);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _BeginPopupModalOpen, bool, (MonoString* idRaw, bool* isOpen, int flags), YonaiEditor)
{
	char* id = mono_string_to_utf8(idRaw);
	bool output = ImGui::BeginPopupModal(id, isOpen, flags);
	mono_free(id);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _BeginPopupModal, bool, (MonoString* idRaw, int flags), YonaiEditor)
{
	char* id = mono_string_to_utf8(idRaw);
	bool output = ImGui::BeginPopupModal(id, nullptr, flags);
	mono_free(id);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, _OpenPopup, void, (MonoString* idRaw, int flags), YonaiEditor)
{
	char* id = mono_string_to_utf8(idRaw);
	ImGui::OpenPopup(id, flags);
	mono_free(id);
}

ADD_MANAGED_METHOD(ImGUI, CloseCurrentPopup, void, (), YonaiEditor)
{ ImGui::CloseCurrentPopup(); }

ADD_MANAGED_METHOD(ImGUI, Separator, void, (), YonaiEditor) { ImGui::Separator(); }

ADD_MANAGED_METHOD(ImGUI, PushItemWidth, void, (float width), YonaiEditor)
{ ImGui::PushItemWidth(width); }

ADD_MANAGED_METHOD(ImGUI, PopItemWidth, void, (), YonaiEditor)
{ ImGui::PopItemWidth(); }

ADD_MANAGED_METHOD(ImGUI, SetNextItemWidth, void, (float width), YonaiEditor)
{ ImGui::SetNextItemWidth(width); }

ADD_MANAGED_METHOD(ImGUI, _PlotLines, void, (MonoString* labelRaw, MonoArray* pointsRaw, MonoString* overlayText, glm::vec2* graphSize), YonaiEditor)
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

ADD_MANAGED_METHOD(ImGUI, _DockSpace, void, (MonoString* idRaw, glm::vec2* size), YonaiEditor)
{
	char* id = mono_string_to_utf8(idRaw);
	ImGui::DockSpace(ImGui::GetID(id), ImVec2(size->x, size->y));
	mono_free(id);
}

ADD_MANAGED_METHOD(ImGUI, _GetMouseDelta, void, (glm::vec2* output), YonaiEditor)
{
	ImVec2 delta = ImGui::GetIO().MouseDelta;
	*output = glm::vec2(delta.x, delta.y);
}

ADD_MANAGED_METHOD(ImGUI, GetMouseDeltaX, float, (), YonaiEditor) { return ImGui::GetIO().MouseDelta.x; }
ADD_MANAGED_METHOD(ImGUI, GetMouseDeltaY, float, (), YonaiEditor) { return ImGui::GetIO().MouseDelta.y; }

// VIEWPORT //
ADD_MANAGED_METHOD(ImGUI, _GetViewport, void,
	(unsigned int ID,
		glm::vec2* outPosition,
		glm::vec2* outSize,
		glm::vec2* outWorkPosition,
		glm::vec2* outWorkSize,
		int* outParentViewportID
		), YonaiEditor)
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

ADD_MANAGED_METHOD(ImGUI, GetMainViewportID, unsigned int, (), YonaiEditor)
{ return ImGui::GetMainViewport()->ID; }

ADD_MANAGED_METHOD(ImGUI, GetWindowViewportID, unsigned int, (), YonaiEditor)
{ return ImGui::GetWindowViewport()->ID; }

ADD_MANAGED_METHOD(ImGUI, _SetNextWindowPos, void, (glm::vec2* position), YonaiEditor)
{ ImGui::SetNextWindowPos(ImVec2(position->x, position->y)); }

ADD_MANAGED_METHOD(ImGUI, _SetNextWindowSize, void, (glm::vec2* position), YonaiEditor)
{ ImGui::SetNextWindowSize(ImVec2(position->x, position->y)); }

ADD_MANAGED_METHOD(ImGUI, SetNextWindowViewport, void, (unsigned int viewportID), YonaiEditor)
{ ImGui::SetNextWindowViewport(viewportID); }

ADD_MANAGED_METHOD(ImGUI, SetNextWindowFocus, void, (), YonaiEditor)
{ ImGui::SetNextWindowFocus(); }

ADD_MANAGED_METHOD(ImGUI, _SetWindowFocus, void, (), YonaiEditor)
{ ImGui::SetWindowFocus(); }

ADD_MANAGED_METHOD(ImGUI, _SetWindowFocusNamed, void, (MonoString* titleRaw), YonaiEditor)
{
	char* title = mono_string_to_utf8(titleRaw);
	ImGui::SetWindowFocus(title);
	mono_free(title);
}

ADD_MANAGED_METHOD(ImGUI, _GetWindowContentRegionMin, void, (glm::vec2* outRegion), YonaiEditor)
{
	ImVec2 region = ImGui::GetWindowContentRegionMin();
	*outRegion = glm::ivec2(region.x, region.y);
}

ADD_MANAGED_METHOD(ImGUI, _GetWindowContentRegionMax, void, (glm::vec2* outRegion), YonaiEditor)
{
	ImVec2 region = ImGui::GetWindowContentRegionMax();
	*outRegion = glm::ivec2(region.x, region.y);
}

ADD_MANAGED_METHOD(ImGUI, _GetContentRegionAvail, void, (glm::vec2* outRegion), YonaiEditor)
{
	ImVec2 region = ImGui::GetContentRegionAvail();
	*outRegion = glm::ivec2(region.x, region.y);
}

ADD_MANAGED_METHOD(ImGUI, _GetWindowPosition, void, (glm::vec2* outPos), YonaiEditor)
{
	ImVec2 pos = ImGui::GetWindowPos();
	*outPos= glm::ivec2(pos.x, pos.y);
}

ADD_MANAGED_METHOD(ImGUI, _GetWindowSize, void, (glm::vec2* outPos), YonaiEditor)
{
	ImVec2 pos = ImGui::GetWindowSize();
	*outPos= glm::ivec2(pos.x, pos.y);
}

ADD_MANAGED_METHOD(ImGUI, _IsWindowFocused, bool, (int flags), YonaiEditor)
{ return ImGui::IsWindowFocused(flags); }

ADD_MANAGED_METHOD(ImGUI, _IsWindowHovered, bool, (int flags), YonaiEditor)
{ return ImGui::IsWindowHovered(flags); }

ADD_MANAGED_METHOD(ImGUI, _IsWindowCollapsed, bool, (), YonaiEditor)
{ return ImGui::IsWindowCollapsed(); }

// STYLE //
ADD_MANAGED_METHOD(ImGUI, _PushStyleVar, void, (int flag, float value), YonaiEditor)
{ ImGui::PushStyleVar(flag, value); }

ADD_MANAGED_METHOD(ImGUI, _PushStyleVarV, void, (int flag, glm::vec2* value), YonaiEditor)
{ ImGui::PushStyleVar(flag, ImVec2(value->x, value->y)); }

ADD_MANAGED_METHOD(ImGUI, PopStyleVar, void, (int amount), YonaiEditor)
{ ImGui::PopStyleVar(amount); }

ADD_MANAGED_METHOD(ImGUI, _PushStyleColour, void, (int var, glm::vec4* value), YonaiEditor)
{ ImGui::PushStyleColor(var, ToVec4(value)); }

ADD_MANAGED_METHOD(ImGUI, PopStyleColour, void, (int amount), YonaiEditor)
{ ImGui::PopStyleColor(amount); }

// MENU //
ADD_MANAGED_METHOD(ImGUI, BeginMenuBar, bool, (), YonaiEditor)
{ return ImGui::BeginMenuBar(); }

ADD_MANAGED_METHOD(ImGUI, EndMenuBar, void, (), YonaiEditor)
{ ImGui::EndMenuBar(); }

ADD_MANAGED_METHOD(ImGUI, BeginMenu, bool, (MonoString* labelRaw, bool enabled), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	bool output = ImGui::BeginMenu(label, enabled);
	mono_free(label);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, EndMenu, void, (), YonaiEditor)
{ ImGui::EndMenu(); }

ADD_MANAGED_METHOD(ImGUI, MenuItem, bool, (MonoString* labelRaw, MonoString* shortcutRaw, bool enabled), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* shortcut = mono_string_to_utf8(shortcutRaw);
	bool output = ImGui::MenuItem(label, shortcut, false, enabled);
	mono_free(label);
	return output;
}

// TABLE //
ADD_MANAGED_METHOD(ImGUI, _BeginTable, bool, (MonoString* labelRaw, unsigned int columns, int flags, glm::vec2* outerSize, float innerWidth), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	ImVec2 size(outerSize->x, outerSize->y);
	bool output = ImGui::BeginTable(label, columns, flags, size, innerWidth);
	mono_free(label);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, EndTable, void, (), YonaiEditor)
{ ImGui::EndTable(); }

ADD_MANAGED_METHOD(ImGUI, TableNextRow, void, (float minHeight, bool isHeader), YonaiEditor)
{ ImGui::TableNextRow(isHeader ? ImGuiTableRowFlags_Headers : 0, minHeight); }

ADD_MANAGED_METHOD(ImGUI, TableSetColumnIndex, void, (int column), YonaiEditor)
{ ImGui::TableSetColumnIndex(column); }

ADD_MANAGED_METHOD(ImGUI, TableNextColumn, void, (), YonaiEditor)
{ ImGui::TableNextColumn(); }

ADD_MANAGED_METHOD(ImGUI, TableHeadersRow, void, (), YonaiEditor)
{ ImGui::TableHeadersRow(); }

ADD_MANAGED_METHOD(ImGUI, TableHeader, void, (MonoString* labelRaw), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	ImGui::TableHeader(label);
	mono_free(label);
}

ADD_MANAGED_METHOD(ImGUI, TableSetColumnEnabled, void, (int column, bool enabled), YonaiEditor)
{ ImGui::TableSetColumnEnabled(column, enabled); }

ADD_MANAGED_METHOD(ImGUI, _TableGetColumnFlags, int, (int column), YonaiEditor)
{ return ImGui::TableGetColumnFlags(column); }

ADD_MANAGED_METHOD(ImGUI, TableSetupScrollFreeze, void, (int column, int row), YonaiEditor)
{ return ImGui::TableSetupScrollFreeze(column, row); }

ADD_MANAGED_METHOD(ImGUI, _TableSetupColumn, void, (MonoString* labelRaw, int flags, float size), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	ImGui::TableSetupColumn(label, flags, size);
	mono_free(label);
}

// Column API //
ADD_MANAGED_METHOD(ImGUI, Columns, void, (int count, MonoString* idRaw, bool border), YonaiEditor)
{
	char* id = mono_string_to_utf8(idRaw);
	ImGui::Columns(count, id, border);
	mono_free(id);
}

ADD_MANAGED_METHOD(ImGUI, NextColumn, void, (), YonaiEditor) { ImGui::NextColumn(); }
ADD_MANAGED_METHOD(ImGUI, SetColumnWidth, void, (int index, float value), YonaiEditor) { ImGui::SetColumnWidth(index, value); }
ADD_MANAGED_METHOD(ImGUI, SetColumnOffset, void, (int index, float value), YonaiEditor) { ImGui::SetColumnOffset(index, value); }

// COMBO //
ADD_MANAGED_METHOD(ImGUI, _BeginCombo, bool, (MonoString* labelRaw, MonoString* previewRaw, int flags), YonaiEditor)
{
	char* label = mono_string_to_utf8(labelRaw);
	char* preview = mono_string_to_utf8(previewRaw);
	bool output = ImGui::BeginCombo(label, preview, flags);
	mono_free(label);
	return output;
}

ADD_MANAGED_METHOD(ImGUI, EndCombo, void, (), YonaiEditor) { ImGui::EndCombo(); }

// DRAW LIST //
ImDrawList* GetDrawList() { return ImGui::GetWindowDrawList(); }

ADD_MANAGED_METHOD(ImGUI, _AddLine, void, (glm::vec2* p1, glm::vec2* p2, glm::vec4* colour, float thickness), YonaiEditor)
{ GetDrawList()->AddLine(ToVec2(p1), ToVec2(p2), ToU32(colour), thickness); }

ADD_MANAGED_METHOD(ImGUI, _AddRect, void, (glm::vec2* p1, glm::vec2* p2, glm::vec4* colour, float rounding, int flags, float thickness), YonaiEditor)
{ GetDrawList()->AddRect(ToVec2(p1), ToVec2(p2), ToU32(colour), rounding, flags, thickness); }

ADD_MANAGED_METHOD(ImGUI, _AddRectFilled, void, (glm::vec2* p1, glm::vec2* p2, glm::vec4* colour, float rounding, int flags), YonaiEditor)
{ GetDrawList()->AddRectFilled(ToVec2(p1), ToVec2(p2), ToU32(colour), rounding, flags); }

ADD_MANAGED_METHOD(ImGUI, _AddQuad, void, (glm::vec2* p1, glm::vec2* p2, glm::vec2* p3, glm::vec2* p4, glm::vec4* colour, float thickness), YonaiEditor)
{ GetDrawList()->AddQuad(ToVec2(p1), ToVec2(p2), ToVec2(p3), ToVec2(p4), ToU32(colour), thickness); }

ADD_MANAGED_METHOD(ImGUI, _AddQuadFilled, void, (glm::vec2* p1, glm::vec2* p2, glm::vec2* p3, glm::vec2* p4, glm::vec4* colour), YonaiEditor)
{ GetDrawList()->AddQuadFilled(ToVec2(p1), ToVec2(p2), ToVec2(p3), ToVec2(p4), ToU32(colour)); }

ADD_MANAGED_METHOD(ImGUI, _AddTriangle, void, (glm::vec2* p1, glm::vec2* p2, glm::vec2* p3, glm::vec4* colour, float thickness), YonaiEditor)
{ GetDrawList()->AddTriangle(ToVec2(p1), ToVec2(p2), ToVec2(p3), ToU32(colour), thickness); }

ADD_MANAGED_METHOD(ImGUI, _AddTriangleFilled, void, (glm::vec2* p1, glm::vec2* p2, glm::vec2* p3, glm::vec4* colour), YonaiEditor)
{ GetDrawList()->AddTriangleFilled(ToVec2(p1), ToVec2(p2), ToVec2(p3), ToU32(colour)); }

ADD_MANAGED_METHOD(ImGUI, _AddCircle, void, (glm::vec2* center, float radius, glm::vec4* colour, int numSegments, float thickness), YonaiEditor)
{ GetDrawList()->AddCircle(ToVec2(center), radius, ToU32(colour), numSegments, thickness); }

ADD_MANAGED_METHOD(ImGUI, _AddCircleFilled, void, (glm::vec2* center, float radius, glm::vec4* colour, int numSegments), YonaiEditor)
{ GetDrawList()->AddCircleFilled(ToVec2(center), radius, ToU32(colour), numSegments); }

ADD_MANAGED_METHOD(ImGUI, _AddNgon, void, (glm::vec2* center, float radius, glm::vec4* colour, int numSegments, float thickness), YonaiEditor)
{ GetDrawList()->AddNgon(ToVec2(center), radius, ToU32(colour), numSegments, thickness); }

ADD_MANAGED_METHOD(ImGUI, _AddNgonFilled, void, (glm::vec2* center, float radius, glm::vec4* colour, int numSegments), YonaiEditor)
{ GetDrawList()->AddNgonFilled(ToVec2(center), radius, ToU32(colour), numSegments); }

ADD_MANAGED_METHOD(ImGUI, _AddText, void, (glm::vec2* position, MonoString* textRaw, glm::vec4* colour, float fontSize), YonaiEditor)
{
	char* text = mono_string_to_utf8(textRaw);
	GetDrawList()->AddText(nullptr, fontSize, ToVec2(position), ToU32(colour), text);
	mono_free(text);
}

ADD_MANAGED_METHOD(ImGUI, _AddBezierCubic, void, (glm::vec2* p1, glm::vec2* p2, glm::vec2* p3, glm::vec2* p4, glm::vec4* colour, float thickness, int segments), YonaiEditor)
{ GetDrawList()->AddBezierCubic(ToVec2(p1), ToVec2(p2), ToVec2(p3), ToVec2(p4), ToU32(colour), thickness, segments); }

ADD_MANAGED_METHOD(ImGUI, _AddBezierCurve, void, (glm::vec2* p1, glm::vec2* p2, glm::vec2* p3, glm::vec2* p4, glm::vec4* colour, float thickness, int segments), YonaiEditor)
{ /* GetDrawList()->AddBe(ToVec2(p1), ToVec2(p2), ToVec2(p3), ToVec2(p4), ToU32(colour), thickness, segments); */ }

ADD_MANAGED_METHOD(ImGUI, _AddBezierQuadratic, void, (glm::vec2* p1, glm::vec2* p2, glm::vec2* p3, glm::vec4* colour, float thickness, int segments), YonaiEditor)
{ GetDrawList()->AddBezierQuadratic(ToVec2(p1), ToVec2(p2), ToVec2(p3), ToU32(colour), thickness, segments); }

ADD_MANAGED_METHOD(ImGUI, _AddImage, void, (uint64_t textureID, glm::vec2* min, glm::vec2* max, glm::vec4* colour), YonaiEditor)
{
	Texture* texture = Resource::Get<Texture>(textureID);
	GetDrawList()->AddImage((ImTextureID)(texture ? texture->GetID() : 0), ToVec2(min), ToVec2(max), ImVec2(), ImVec2(1, 1), ToU32(colour));
}

ADD_MANAGED_METHOD(ImGUI, _AddImageRounded, void, (uint64_t textureID, glm::vec2* min, glm::vec2* max, glm::vec4* colour, float rounding, int flags), YonaiEditor)
{
	Texture* texture = Resource::Get<Texture>(textureID);
	GetDrawList()->AddImageRounded((ImTextureID)(texture ? texture->GetID() : 0), ToVec2(min), ToVec2(max), ImVec2(), ImVec2(1, 1), ToU32(colour), rounding, flags);
}

ADD_MANAGED_METHOD(ImGUI, _PushClipRect, void, (glm::vec2* p1, glm::vec2* p2, bool intersectWithCurrentClipRect), YonaiEditor)
{ ImGui::PushClipRect(ToVec2(p1), ToVec2(p2), intersectWithCurrentClipRect); }

ADD_MANAGED_METHOD(ImGUI, PopClipRect, void, (), YonaiEditor)
{ ImGui::PopClipRect(); }

ADD_MANAGED_METHOD(ImGUI, _DrawListPushClipRect, void, (glm::vec2* p1, glm::vec2* p2, bool intersectWithCurrentClipRect), YonaiEditor)
{ GetDrawList()->PushClipRect(ToVec2(p1), ToVec2(p2), intersectWithCurrentClipRect); }

ADD_MANAGED_METHOD(ImGUI, DrawListPushClipRectFullScreen, void, (), YonaiEditor)
{ GetDrawList()->PushClipRectFullScreen(); }

ADD_MANAGED_METHOD(ImGUI, DrawListPopClipRect, void, (), YonaiEditor)
{ GetDrawList()->PopClipRect(); }

// ImGuizmo //
ADD_MANAGED_METHOD(ImGUI, Gizmo_SetRect, void, (float x, float y, float width, float height), YonaiEditor)
{ ImGuizmo::SetRect(x, y, width, height); }

ADD_MANAGED_METHOD(ImGUI, Gizmo_SetDrawList, void, (), YonaiEditor)
{ ImGuizmo::SetDrawlist(); }

ADD_MANAGED_METHOD(ImGUI, Gizmo_Enable, void, (bool enable), YonaiEditor)
{ ImGuizmo::Enable(enable); }

ADD_MANAGED_METHOD(ImGUI, Gizmo_IsOver, bool, (), YonaiEditor)  { return ImGuizmo::IsOver(); }
ADD_MANAGED_METHOD(ImGUI, Gizmo_IsUsing, bool, (), YonaiEditor) { return ImGuizmo::IsUsing(); }

ADD_MANAGED_METHOD(ImGUI, _Gizmo_Manipulate, void, (void* cameraHandle, void* transformHandle, glm::vec2* drawRegion, unsigned int operation, bool local, float snapping), YonaiEditor)
{
	Components::Camera* camera = (Components::Camera*)cameraHandle;
	Components::Transform* transform = (Components::Transform*)transformHandle;
	Components::Transform* parent = transform->GetParent();

	ImGuizmo::SetOrthographic(camera->Orthographic);

	mat4 viewMatrix = camera->GetViewMatrix();
	mat4 projectionMatrix = camera->GetProjectionMatrix(drawRegion->x, drawRegion->y);

	mat4 modelMatrix = transform->GetModelMatrix();

	float snapValues[3] = { snapping, snapping, snapping };

	ImGuizmo::Manipulate(
		value_ptr(viewMatrix),
		value_ptr(projectionMatrix),
		(ImGuizmo::OPERATION)operation,
		local ? ImGuizmo::LOCAL : ImGuizmo::WORLD,
		value_ptr(modelMatrix),
		nullptr,
		snapping > 0 ? snapValues : nullptr
	);

	transform->SetModelMatrix(modelMatrix, true);
}

#pragma warning(pop) // Restore warning(s)
