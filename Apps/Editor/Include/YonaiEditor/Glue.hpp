#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <mono/jit/jit.h>

using namespace std; 

typedef pair<const char*, const void*> MethodType; 

void _managed_internal_EditorWindowInitContext();
void _managed_internal_EditorWindowDestroyContext();
bool _managed_internal_EditorWindowContextIsInitialised();
void _managed_internal_EditorWindowCreate();
void _managed_internal_EditorWindowDestroy();
void _managed_internal_EditorWindow_CreationHintBool(int hint, bool value);
void _managed_internal_EditorWindow_CreationHintInt(int hint, int value);
void _managed_internal_EditorWindowResetHints();
MonoString* _managed_internal_FileDialog_OpenFolder(MonoString* titleRaw, MonoString* initialDirRaw);
MonoArray* _managed_internal_FileDialog_OpenFile(MonoString* titleRaw, MonoString* initialDirRaw, MonoArray* filtersRaw, bool multiselect);
void _managed_internal_ImGUISetIniFilename(MonoString* pathRaw);
void _managed_internal_ImGUISetDisplayFramebufferScale(float scaleX, float scaleY);
void _managed_internal_ImGUILoadIniSettingsFromDisk(MonoString* pathRaw);
void _managed_internal_ImGUISaveIniSettingsToDisk(MonoString* pathRaw);
void _managed_internal_ImGUIAddFontFromFile(MonoString* pathRaw, int fontSize);
void _managed_internal_ImGUIAddFont(MonoArray* dataRaw, int fontSize);
void _managed_internal_ImGUISetFontGlobalScale(float scale);
void _managed_internal_ImGUI_Begin(MonoString* nameRaw, int flags);
bool _managed_internal_ImGUI_BeginClosable(MonoString* nameRaw, bool* open, int flags);
void _managed_internal_ImGUIEnd();
bool _managed_internal_ImGUI_BeginChild(MonoString* idRaw, glm::vec2* sizeRaw, bool border, int flags);
void _managed_internal_ImGUIEndChild();
bool _managed_internal_ImGUI_IsItemHovered(int flags);
bool _managed_internal_ImGUIIsItemClicked();
bool _managed_internal_ImGUIIsItemActive();
bool _managed_internal_ImGUI_IsMouseClicked(int button);
bool _managed_internal_ImGUI_IsMouseDoubleClicked(int button);
bool _managed_internal_ImGUI_IsMouseDragging(int button);
bool _managed_internal_ImGUI_IsMouseReleased(int button);
bool _managed_internal_ImGUIIsItemEdited();
void _managed_internal_ImGUIBeginDisabled();
void _managed_internal_ImGUIEndDisabled();
void _managed_internal_ImGUISetItemDefaultFocus();
bool _managed_internal_ImGUI_IsKeyDown(int key);
bool _managed_internal_ImGUI_IsKeyPressed(int key);
bool _managed_internal_ImGUI_IsKeyReleased(int key);
void _managed_internal_ImGUIPushID(MonoString* idRaw);
void _managed_internal_ImGUIPopID();
void _managed_internal_ImGUI_CalculateTextWidth(MonoString* textRaw, glm::vec2* output);
bool _managed_internal_ImGUIFoldout(MonoString* labelRaw, bool openByDefault);
void _managed_internal_ImGUIIndent(float width);
void _managed_internal_ImGUIUnindent(float width);
float _managed_internal_ImGUIGetCursorPosX();
float _managed_internal_ImGUIGetCursorPosY();
void _managed_internal_ImGUI_GetCursorPos(glm::vec2* output);
float _managed_internal_ImGUIGetCursorScreenPosX();
float _managed_internal_ImGUIGetCursorScreenPosY();
void _managed_internal_ImGUI_GetCursorScreenPos(glm::vec2* output);
void _managed_internal_ImGUI_SetCursorPos(glm::vec2* input);
void _managed_internal_ImGUI_SetCursorScreenPos(glm::vec2* input);
void _managed_internal_ImGUISetCursorPosX(float value);
void _managed_internal_ImGUISetCursorPosY(float value);
int _managed_internal_ImGUI_GetMouseCursor();
void _managed_internal_ImGUI_SetMouseCursor(int type);
void _managed_internal_ImGUIBeginGroup();
void _managed_internal_ImGUIEndGroup();
bool _managed_internal_ImGUI_BeginDragDropSource(int flags);
void _managed_internal_ImGUIEndDragDropSource();
bool _managed_internal_ImGUIBeginDragDropTarget();
void _managed_internal_ImGUIEndDragDropTarget();
void _managed_internal_ImGUISetDragDropPayload(MonoString* labelRaw, MonoObject* payload);
MonoObject* _managed_internal_ImGUIGetDragDropPayload(MonoString* labelRaw, int flags);
MonoString* _managed_internal_ImGUIGetDragDropPayloadType();
bool _managed_internal_ImGUIIsDragDropPayloadType(MonoString* labelRaw);
MonoObject* _managed_internal_ImGUI_AcceptDragDropPayload(MonoString* labelRaw, int flags);
bool _managed_internal_ImGUIDragDropPayloadIsDelivery();
void _managed_internal_ImGUI_Text(MonoString* labelRaw);
void _managed_internal_ImGUI_TextColoured(MonoString* labelRaw, glm::vec4* colour);
void _managed_internal_ImGUI_Image(uint64_t textureID, glm::vec2* size, glm::vec4* tint, glm::vec4* borderCol);
void _managed_internal_ImGUI_ImageRenderTexture(void* handle, glm::vec2* size, glm::vec4* tint, glm::vec4* borderCol);
bool _managed_internal_ImGUI_ImageButton(uint64_t textureID, glm::vec2* size, int framePadding, glm::vec4* tint, glm::vec4* bgCol);
bool _managed_internal_ImGUI_InputText(MonoString* labelRaw, MonoString** valueRaw, int maxCharacters, int flags);
bool _managed_internal_ImGUI_InputTextMultiline(MonoString* labelRaw, MonoString** valueRaw, int maxCharacters, glm::vec2* size, int flags);
bool _managed_internal_ImGUI_InputFloat(MonoString* labelRaw, float* value, float step, float stepFast, MonoString* formatRaw);
bool _managed_internal_ImGUI_InputFloat2(MonoString* labelRaw, glm::vec2* value, MonoString* formatRaw);
bool _managed_internal_ImGUI_InputFloat3(MonoString* labelRaw, glm::vec3* value, MonoString* formatRaw);
bool _managed_internal_ImGUI_InputFloat4(MonoString* labelRaw, glm::vec4* value, MonoString* formatRaw);
bool _managed_internal_ImGUI_InputQuat(MonoString* labelRaw, glm::vec4* value, MonoString* formatRaw);
bool _managed_internal_ImGUI_InputInt(MonoString* labelRaw, int* value, int step, int stepFast);
bool _managed_internal_ImGUI_InputInt2(MonoString* labelRaw, glm::ivec2* value);
bool _managed_internal_ImGUI_InputInt3(MonoString* labelRaw, glm::ivec3* value);
bool _managed_internal_ImGUI_InputInt4(MonoString* labelRaw, glm::ivec4* value);
bool _managed_internal_ImGUI_DragFloat(MonoString* labelRaw, float* value, float speed, float min, float max, MonoString* formatRaw);
bool _managed_internal_ImGUI_DragFloat2(MonoString* labelRaw, glm::vec2* value, float speed, float min, float max, MonoString* formatRaw);
bool _managed_internal_ImGUI_DragFloat3(MonoString* labelRaw, glm::vec3* value, float speed, float min, float max, MonoString* formatRaw);
bool _managed_internal_ImGUI_DragFloat4(MonoString* labelRaw, glm::vec4* value, float speed, float min, float max, MonoString* formatRaw);
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
void _managed_internal_ImGUISpace();
void _managed_internal_ImGUI_Dummy(glm::vec2* size);
float _managed_internal_ImGUI_GetTextLineHeight();
void _managed_internal_ImGUISameLine(float offset, float spacing);
bool _managed_internal_ImGUI_Button(MonoString* labelRaw, glm::vec2* sizeRaw);
bool _managed_internal_ImGUI_InvisibleButton(MonoString* labelRaw, glm::vec2* size);
bool _managed_internal_ImGUI_Selectable(MonoString* labelRaw, bool selected, int flags, glm::vec2* size);
bool _managed_internal_ImGUICheckbox(MonoString* labelRaw, bool* checkState);
bool _managed_internal_ImGUIColourEdit3(MonoString* labelRaw, float* colour);
bool _managed_internal_ImGUIColourEdit4(MonoString* labelRaw, float* colour);
void _managed_internal_ImGUISetTooltip(MonoString* textRaw);
bool _managed_internal_ImGUIBeginTooltip();
void _managed_internal_ImGUIEndTooltip();
bool _managed_internal_ImGUI_BeginPopupContextItem(MonoString* idRaw, int flags);
bool _managed_internal_ImGUI_BeginPopup(MonoString* idRaw, int flags);
void _managed_internal_ImGUIEndPopup();
bool _managed_internal_ImGUIIsPopupOpen(MonoString* idRaw);
bool _managed_internal_ImGUI_BeginPopupModalOpen(MonoString* idRaw, bool* isOpen, int flags);
bool _managed_internal_ImGUI_BeginPopupModal(MonoString* idRaw, int flags);
void _managed_internal_ImGUI_OpenPopup(MonoString* idRaw, int flags);
void _managed_internal_ImGUICloseCurrentPopup();
void _managed_internal_ImGUISeparator();
void _managed_internal_ImGUIPushItemWidth(float width);
void _managed_internal_ImGUIPopItemWidth();
void _managed_internal_ImGUISetNextItemWidth(float width);
void _managed_internal_ImGUI_PlotLines(MonoString* labelRaw, MonoArray* pointsRaw, MonoString* overlayText, glm::vec2* graphSize);
void _managed_internal_ImGUI_DockSpace(MonoString* idRaw, glm::vec2* size);
void _managed_internal_ImGUI_GetMouseDelta(glm::vec2* output);
float _managed_internal_ImGUIGetMouseDeltaX();
float _managed_internal_ImGUIGetMouseDeltaY();
void _managed_internal_ImGUI_GetViewport(unsigned int ID,  glm::vec2* outPosition,  glm::vec2* outSize,  glm::vec2* outWorkPosition,  glm::vec2* outWorkSize,  int* outParentViewportID  );
unsigned int _managed_internal_ImGUIGetMainViewportID();
unsigned int _managed_internal_ImGUIGetWindowViewportID();
void _managed_internal_ImGUI_SetNextWindowPos(glm::vec2* position);
void _managed_internal_ImGUI_SetNextWindowSize(glm::vec2* position);
void _managed_internal_ImGUISetNextWindowViewport(unsigned int viewportID);
void _managed_internal_ImGUI_GetWindowContentRegionMin(glm::vec2* outRegion);
void _managed_internal_ImGUI_GetWindowContentRegionMax(glm::vec2* outRegion);
void _managed_internal_ImGUI_GetContentRegionAvail(glm::vec2* outRegion);
void _managed_internal_ImGUI_GetWindowPosition(glm::vec2* outPos);
void _managed_internal_ImGUI_GetWindowSize(glm::vec2* outPos);
bool _managed_internal_ImGUI_IsWindowFocused(int flags);
bool _managed_internal_ImGUI_IsWindowHovered(int flags);
bool _managed_internal_ImGUI_IsWindowCollapsed();
void _managed_internal_ImGUI_PushStyleVar(int flag, float value);
void _managed_internal_ImGUI_PushStyleVarV(int flag, glm::vec2* value);
void _managed_internal_ImGUIPopStyleVar(int amount);
void _managed_internal_ImGUI_PushStyleColour(int var, glm::vec4* value);
void _managed_internal_ImGUIPopStyleColour(int amount);
bool _managed_internal_ImGUIBeginMenuBar();
void _managed_internal_ImGUIEndMenuBar();
bool _managed_internal_ImGUIBeginMenu(MonoString* labelRaw, bool enabled);
void _managed_internal_ImGUIEndMenu();
bool _managed_internal_ImGUIMenuItem(MonoString* labelRaw, MonoString* shortcutRaw, bool enabled);
bool _managed_internal_ImGUI_BeginTable(MonoString* labelRaw, unsigned int columns, int flags, glm::vec2* outerSize, float innerWidth);
void _managed_internal_ImGUIEndTable();
void _managed_internal_ImGUITableNextRow(float minHeight, bool isHeader);
void _managed_internal_ImGUITableSetColumnIndex(int column);
void _managed_internal_ImGUITableNextColumn();
void _managed_internal_ImGUITableHeadersRow();
void _managed_internal_ImGUITableHeader(MonoString* labelRaw);
void _managed_internal_ImGUITableSetColumnEnabled(int column, bool enabled);
int _managed_internal_ImGUI_TableGetColumnFlags(int column);
void _managed_internal_ImGUITableSetupScrollFreeze(int column, int row);
void _managed_internal_ImGUI_TableSetupColumn(MonoString* labelRaw, int flags, float size);
void _managed_internal_ImGUIColumns(int count, MonoString* idRaw, bool border);
void _managed_internal_ImGUINextColumn();
void _managed_internal_ImGUISetColumnWidth(int index, float value);
void _managed_internal_ImGUISetColumnOffset(int index, float value);
bool _managed_internal_ImGUI_BeginCombo(MonoString* labelRaw, MonoString* previewRaw, int flags);
void _managed_internal_ImGUIEndCombo();
void _managed_internal_ImGUI_AddLine(glm::vec2* p1, glm::vec2* p2, glm::vec4* colour, float thickness);
void _managed_internal_ImGUI_AddRect(glm::vec2* p1, glm::vec2* p2, glm::vec4* colour, float rounding, int flags, float thickness);
void _managed_internal_ImGUI_AddRectFilled(glm::vec2* p1, glm::vec2* p2, glm::vec4* colour, float rounding, int flags);
void _managed_internal_ImGUI_AddQuad(glm::vec2* p1, glm::vec2* p2, glm::vec2* p3, glm::vec2* p4, glm::vec4* colour, float thickness);
void _managed_internal_ImGUI_AddQuadFilled(glm::vec2* p1, glm::vec2* p2, glm::vec2* p3, glm::vec2* p4, glm::vec4* colour);
void _managed_internal_ImGUI_AddTriangle(glm::vec2* p1, glm::vec2* p2, glm::vec2* p3, glm::vec4* colour, float thickness);
void _managed_internal_ImGUI_AddTriangleFilled(glm::vec2* p1, glm::vec2* p2, glm::vec2* p3, glm::vec4* colour);
void _managed_internal_ImGUI_AddCircle(glm::vec2* center, float radius, glm::vec4* colour, int numSegments, float thickness);
void _managed_internal_ImGUI_AddCircleFilled(glm::vec2* center, float radius, glm::vec4* colour, int numSegments);
void _managed_internal_ImGUI_AddNgon(glm::vec2* center, float radius, glm::vec4* colour, int numSegments, float thickness);
void _managed_internal_ImGUI_AddNgonFilled(glm::vec2* center, float radius, glm::vec4* colour, int numSegments);
void _managed_internal_ImGUI_AddText(glm::vec2* position, MonoString* textRaw, glm::vec4* colour, float fontSize);
void _managed_internal_ImGUI_AddBezierCubic(glm::vec2* p1, glm::vec2* p2, glm::vec2* p3, glm::vec2* p4, glm::vec4* colour, float thickness, int segments);
void _managed_internal_ImGUI_AddBezierCurve(glm::vec2* p1, glm::vec2* p2, glm::vec2* p3, glm::vec2* p4, glm::vec4* colour, float thickness, int segments);
void _managed_internal_ImGUI_AddBezierQuadratic(glm::vec2* p1, glm::vec2* p2, glm::vec2* p3, glm::vec4* colour, float thickness, int segments);
void _managed_internal_ImGUI_AddImage(uint64_t textureID, glm::vec2* min, glm::vec2* max, glm::vec4* colour);
void _managed_internal_ImGUI_AddImageRounded(uint64_t textureID, glm::vec2* min, glm::vec2* max, glm::vec4* colour, float rounding, int flags);
void _managed_internal_ImGUI_PushClipRect(glm::vec2* p1, glm::vec2* p2, bool intersectWithCurrentClipRect);
void _managed_internal_ImGUIPopClipRect();
void _managed_internal_ImGUI_DrawListPushClipRect(glm::vec2* p1, glm::vec2* p2, bool intersectWithCurrentClipRect);
void _managed_internal_ImGUIDrawListPushClipRectFullScreen();
void _managed_internal_ImGUIDrawListPopClipRect();
void _managed_internal_ImGUIGizmo_SetRect(float x, float y, float width, float height);
void _managed_internal_ImGUIGizmo_SetDrawList();
void _managed_internal_ImGUIGizmo_Enable(bool enable);
void _managed_internal_ImGUI_Gizmo_Manipulate(void* cameraHandle, void* transformHandle, glm::vec2* drawRegion, unsigned int operation, bool local, float snapping);
void _managed_internal_ScriptingReload();
bool _managed_internal_Scripting_DebuggingEnabled();
MonoAssembly* _managed_internal_ScriptingLoadAssembly(MonoString* pathRaw, bool shouldWatch);
bool _managed_internal_ScriptingIsAssemblyLoaded(MonoString* pathRaw);


const vector<pair<const char*, const void*>> _InternalMethods = {
	MethodType("YonaiEditor.EditorWindow::InitContext", (const void*)_managed_internal_EditorWindowInitContext),
	MethodType("YonaiEditor.EditorWindow::DestroyContext", (const void*)_managed_internal_EditorWindowDestroyContext),
	MethodType("YonaiEditor.EditorWindow::ContextIsInitialised", (const void*)_managed_internal_EditorWindowContextIsInitialised),
	MethodType("YonaiEditor.EditorWindow::Create", (const void*)_managed_internal_EditorWindowCreate),
	MethodType("YonaiEditor.EditorWindow::Destroy", (const void*)_managed_internal_EditorWindowDestroy),
	MethodType("YonaiEditor.EditorWindow::_CreationHintBool", (const void*)_managed_internal_EditorWindow_CreationHintBool),
	MethodType("YonaiEditor.EditorWindow::_CreationHintInt", (const void*)_managed_internal_EditorWindow_CreationHintInt),
	MethodType("YonaiEditor.EditorWindow::ResetHints", (const void*)_managed_internal_EditorWindowResetHints),
	MethodType("YonaiEditor.FileDialog::_OpenFolder", (const void*)_managed_internal_FileDialog_OpenFolder),
	MethodType("YonaiEditor.FileDialog::_OpenFile", (const void*)_managed_internal_FileDialog_OpenFile),
	MethodType("YonaiEditor.ImGUI::SetIniFilename", (const void*)_managed_internal_ImGUISetIniFilename),
	MethodType("YonaiEditor.ImGUI::SetDisplayFramebufferScale", (const void*)_managed_internal_ImGUISetDisplayFramebufferScale),
	MethodType("YonaiEditor.ImGUI::LoadIniSettingsFromDisk", (const void*)_managed_internal_ImGUILoadIniSettingsFromDisk),
	MethodType("YonaiEditor.ImGUI::SaveIniSettingsToDisk", (const void*)_managed_internal_ImGUISaveIniSettingsToDisk),
	MethodType("YonaiEditor.ImGUI::AddFontFromFile", (const void*)_managed_internal_ImGUIAddFontFromFile),
	MethodType("YonaiEditor.ImGUI::AddFont", (const void*)_managed_internal_ImGUIAddFont),
	MethodType("YonaiEditor.ImGUI::SetFontGlobalScale", (const void*)_managed_internal_ImGUISetFontGlobalScale),
	MethodType("YonaiEditor.ImGUI::_Begin", (const void*)_managed_internal_ImGUI_Begin),
	MethodType("YonaiEditor.ImGUI::_BeginClosable", (const void*)_managed_internal_ImGUI_BeginClosable),
	MethodType("YonaiEditor.ImGUI::End", (const void*)_managed_internal_ImGUIEnd),
	MethodType("YonaiEditor.ImGUI::_BeginChild", (const void*)_managed_internal_ImGUI_BeginChild),
	MethodType("YonaiEditor.ImGUI::EndChild", (const void*)_managed_internal_ImGUIEndChild),
	MethodType("YonaiEditor.ImGUI::_IsItemHovered", (const void*)_managed_internal_ImGUI_IsItemHovered),
	MethodType("YonaiEditor.ImGUI::IsItemClicked", (const void*)_managed_internal_ImGUIIsItemClicked),
	MethodType("YonaiEditor.ImGUI::IsItemActive", (const void*)_managed_internal_ImGUIIsItemActive),
	MethodType("YonaiEditor.ImGUI::_IsMouseClicked", (const void*)_managed_internal_ImGUI_IsMouseClicked),
	MethodType("YonaiEditor.ImGUI::_IsMouseDoubleClicked", (const void*)_managed_internal_ImGUI_IsMouseDoubleClicked),
	MethodType("YonaiEditor.ImGUI::_IsMouseDragging", (const void*)_managed_internal_ImGUI_IsMouseDragging),
	MethodType("YonaiEditor.ImGUI::_IsMouseReleased", (const void*)_managed_internal_ImGUI_IsMouseReleased),
	MethodType("YonaiEditor.ImGUI::IsItemEdited", (const void*)_managed_internal_ImGUIIsItemEdited),
	MethodType("YonaiEditor.ImGUI::BeginDisabled", (const void*)_managed_internal_ImGUIBeginDisabled),
	MethodType("YonaiEditor.ImGUI::EndDisabled", (const void*)_managed_internal_ImGUIEndDisabled),
	MethodType("YonaiEditor.ImGUI::SetItemDefaultFocus", (const void*)_managed_internal_ImGUISetItemDefaultFocus),
	MethodType("YonaiEditor.ImGUI::_IsKeyDown", (const void*)_managed_internal_ImGUI_IsKeyDown),
	MethodType("YonaiEditor.ImGUI::_IsKeyPressed", (const void*)_managed_internal_ImGUI_IsKeyPressed),
	MethodType("YonaiEditor.ImGUI::_IsKeyReleased", (const void*)_managed_internal_ImGUI_IsKeyReleased),
	MethodType("YonaiEditor.ImGUI::PushID", (const void*)_managed_internal_ImGUIPushID),
	MethodType("YonaiEditor.ImGUI::PopID", (const void*)_managed_internal_ImGUIPopID),
	MethodType("YonaiEditor.ImGUI::_CalculateTextWidth", (const void*)_managed_internal_ImGUI_CalculateTextWidth),
	MethodType("YonaiEditor.ImGUI::Foldout", (const void*)_managed_internal_ImGUIFoldout),
	MethodType("YonaiEditor.ImGUI::Indent", (const void*)_managed_internal_ImGUIIndent),
	MethodType("YonaiEditor.ImGUI::Unindent", (const void*)_managed_internal_ImGUIUnindent),
	MethodType("YonaiEditor.ImGUI::GetCursorPosX", (const void*)_managed_internal_ImGUIGetCursorPosX),
	MethodType("YonaiEditor.ImGUI::GetCursorPosY", (const void*)_managed_internal_ImGUIGetCursorPosY),
	MethodType("YonaiEditor.ImGUI::_GetCursorPos", (const void*)_managed_internal_ImGUI_GetCursorPos),
	MethodType("YonaiEditor.ImGUI::GetCursorScreenPosX", (const void*)_managed_internal_ImGUIGetCursorScreenPosX),
	MethodType("YonaiEditor.ImGUI::GetCursorScreenPosY", (const void*)_managed_internal_ImGUIGetCursorScreenPosY),
	MethodType("YonaiEditor.ImGUI::_GetCursorScreenPos", (const void*)_managed_internal_ImGUI_GetCursorScreenPos),
	MethodType("YonaiEditor.ImGUI::_SetCursorPos", (const void*)_managed_internal_ImGUI_SetCursorPos),
	MethodType("YonaiEditor.ImGUI::_SetCursorScreenPos", (const void*)_managed_internal_ImGUI_SetCursorScreenPos),
	MethodType("YonaiEditor.ImGUI::SetCursorPosX", (const void*)_managed_internal_ImGUISetCursorPosX),
	MethodType("YonaiEditor.ImGUI::SetCursorPosY", (const void*)_managed_internal_ImGUISetCursorPosY),
	MethodType("YonaiEditor.ImGUI::_GetMouseCursor", (const void*)_managed_internal_ImGUI_GetMouseCursor),
	MethodType("YonaiEditor.ImGUI::_SetMouseCursor", (const void*)_managed_internal_ImGUI_SetMouseCursor),
	MethodType("YonaiEditor.ImGUI::BeginGroup", (const void*)_managed_internal_ImGUIBeginGroup),
	MethodType("YonaiEditor.ImGUI::EndGroup", (const void*)_managed_internal_ImGUIEndGroup),
	MethodType("YonaiEditor.ImGUI::_BeginDragDropSource", (const void*)_managed_internal_ImGUI_BeginDragDropSource),
	MethodType("YonaiEditor.ImGUI::EndDragDropSource", (const void*)_managed_internal_ImGUIEndDragDropSource),
	MethodType("YonaiEditor.ImGUI::BeginDragDropTarget", (const void*)_managed_internal_ImGUIBeginDragDropTarget),
	MethodType("YonaiEditor.ImGUI::EndDragDropTarget", (const void*)_managed_internal_ImGUIEndDragDropTarget),
	MethodType("YonaiEditor.ImGUI::SetDragDropPayload", (const void*)_managed_internal_ImGUISetDragDropPayload),
	MethodType("YonaiEditor.ImGUI::GetDragDropPayload", (const void*)_managed_internal_ImGUIGetDragDropPayload),
	MethodType("YonaiEditor.ImGUI::GetDragDropPayloadType", (const void*)_managed_internal_ImGUIGetDragDropPayloadType),
	MethodType("YonaiEditor.ImGUI::IsDragDropPayloadType", (const void*)_managed_internal_ImGUIIsDragDropPayloadType),
	MethodType("YonaiEditor.ImGUI::_AcceptDragDropPayload", (const void*)_managed_internal_ImGUI_AcceptDragDropPayload),
	MethodType("YonaiEditor.ImGUI::DragDropPayloadIsDelivery", (const void*)_managed_internal_ImGUIDragDropPayloadIsDelivery),
	MethodType("YonaiEditor.ImGUI::_Text", (const void*)_managed_internal_ImGUI_Text),
	MethodType("YonaiEditor.ImGUI::_TextColoured", (const void*)_managed_internal_ImGUI_TextColoured),
	MethodType("YonaiEditor.ImGUI::_Image", (const void*)_managed_internal_ImGUI_Image),
	MethodType("YonaiEditor.ImGUI::_ImageRenderTexture", (const void*)_managed_internal_ImGUI_ImageRenderTexture),
	MethodType("YonaiEditor.ImGUI::_ImageButton", (const void*)_managed_internal_ImGUI_ImageButton),
	MethodType("YonaiEditor.ImGUI::_InputText", (const void*)_managed_internal_ImGUI_InputText),
	MethodType("YonaiEditor.ImGUI::_InputTextMultiline", (const void*)_managed_internal_ImGUI_InputTextMultiline),
	MethodType("YonaiEditor.ImGUI::_InputFloat", (const void*)_managed_internal_ImGUI_InputFloat),
	MethodType("YonaiEditor.ImGUI::_InputFloat2", (const void*)_managed_internal_ImGUI_InputFloat2),
	MethodType("YonaiEditor.ImGUI::_InputFloat3", (const void*)_managed_internal_ImGUI_InputFloat3),
	MethodType("YonaiEditor.ImGUI::_InputFloat4", (const void*)_managed_internal_ImGUI_InputFloat4),
	MethodType("YonaiEditor.ImGUI::_InputQuat", (const void*)_managed_internal_ImGUI_InputQuat),
	MethodType("YonaiEditor.ImGUI::_InputInt", (const void*)_managed_internal_ImGUI_InputInt),
	MethodType("YonaiEditor.ImGUI::_InputInt2", (const void*)_managed_internal_ImGUI_InputInt2),
	MethodType("YonaiEditor.ImGUI::_InputInt3", (const void*)_managed_internal_ImGUI_InputInt3),
	MethodType("YonaiEditor.ImGUI::_InputInt4", (const void*)_managed_internal_ImGUI_InputInt4),
	MethodType("YonaiEditor.ImGUI::_DragFloat", (const void*)_managed_internal_ImGUI_DragFloat),
	MethodType("YonaiEditor.ImGUI::_DragFloat2", (const void*)_managed_internal_ImGUI_DragFloat2),
	MethodType("YonaiEditor.ImGUI::_DragFloat3", (const void*)_managed_internal_ImGUI_DragFloat3),
	MethodType("YonaiEditor.ImGUI::_DragFloat4", (const void*)_managed_internal_ImGUI_DragFloat4),
	MethodType("YonaiEditor.ImGUI::_DragInt", (const void*)_managed_internal_ImGUI_DragInt),
	MethodType("YonaiEditor.ImGUI::_DragInt2", (const void*)_managed_internal_ImGUI_DragInt2),
	MethodType("YonaiEditor.ImGUI::_DragInt3", (const void*)_managed_internal_ImGUI_DragInt3),
	MethodType("YonaiEditor.ImGUI::SliderAngle", (const void*)_managed_internal_ImGUISliderAngle),
	MethodType("YonaiEditor.ImGUI::_SliderFloat", (const void*)_managed_internal_ImGUI_SliderFloat),
	MethodType("YonaiEditor.ImGUI::_SliderFloat2", (const void*)_managed_internal_ImGUI_SliderFloat2),
	MethodType("YonaiEditor.ImGUI::_SliderFloat3", (const void*)_managed_internal_ImGUI_SliderFloat3),
	MethodType("YonaiEditor.ImGUI::_SliderInt", (const void*)_managed_internal_ImGUI_SliderInt),
	MethodType("YonaiEditor.ImGUI::_SliderInt2", (const void*)_managed_internal_ImGUI_SliderInt2),
	MethodType("YonaiEditor.ImGUI::_SliderInt3", (const void*)_managed_internal_ImGUI_SliderInt3),
	MethodType("YonaiEditor.ImGUI::Space", (const void*)_managed_internal_ImGUISpace),
	MethodType("YonaiEditor.ImGUI::_Dummy", (const void*)_managed_internal_ImGUI_Dummy),
	MethodType("YonaiEditor.ImGUI::_GetTextLineHeight", (const void*)_managed_internal_ImGUI_GetTextLineHeight),
	MethodType("YonaiEditor.ImGUI::SameLine", (const void*)_managed_internal_ImGUISameLine),
	MethodType("YonaiEditor.ImGUI::_Button", (const void*)_managed_internal_ImGUI_Button),
	MethodType("YonaiEditor.ImGUI::_InvisibleButton", (const void*)_managed_internal_ImGUI_InvisibleButton),
	MethodType("YonaiEditor.ImGUI::_Selectable", (const void*)_managed_internal_ImGUI_Selectable),
	MethodType("YonaiEditor.ImGUI::Checkbox", (const void*)_managed_internal_ImGUICheckbox),
	MethodType("YonaiEditor.ImGUI::ColourEdit3", (const void*)_managed_internal_ImGUIColourEdit3),
	MethodType("YonaiEditor.ImGUI::ColourEdit4", (const void*)_managed_internal_ImGUIColourEdit4),
	MethodType("YonaiEditor.ImGUI::SetTooltip", (const void*)_managed_internal_ImGUISetTooltip),
	MethodType("YonaiEditor.ImGUI::BeginTooltip", (const void*)_managed_internal_ImGUIBeginTooltip),
	MethodType("YonaiEditor.ImGUI::EndTooltip", (const void*)_managed_internal_ImGUIEndTooltip),
	MethodType("YonaiEditor.ImGUI::_BeginPopupContextItem", (const void*)_managed_internal_ImGUI_BeginPopupContextItem),
	MethodType("YonaiEditor.ImGUI::_BeginPopup", (const void*)_managed_internal_ImGUI_BeginPopup),
	MethodType("YonaiEditor.ImGUI::EndPopup", (const void*)_managed_internal_ImGUIEndPopup),
	MethodType("YonaiEditor.ImGUI::IsPopupOpen", (const void*)_managed_internal_ImGUIIsPopupOpen),
	MethodType("YonaiEditor.ImGUI::_BeginPopupModalOpen", (const void*)_managed_internal_ImGUI_BeginPopupModalOpen),
	MethodType("YonaiEditor.ImGUI::_BeginPopupModal", (const void*)_managed_internal_ImGUI_BeginPopupModal),
	MethodType("YonaiEditor.ImGUI::_OpenPopup", (const void*)_managed_internal_ImGUI_OpenPopup),
	MethodType("YonaiEditor.ImGUI::CloseCurrentPopup", (const void*)_managed_internal_ImGUICloseCurrentPopup),
	MethodType("YonaiEditor.ImGUI::Separator", (const void*)_managed_internal_ImGUISeparator),
	MethodType("YonaiEditor.ImGUI::PushItemWidth", (const void*)_managed_internal_ImGUIPushItemWidth),
	MethodType("YonaiEditor.ImGUI::PopItemWidth", (const void*)_managed_internal_ImGUIPopItemWidth),
	MethodType("YonaiEditor.ImGUI::SetNextItemWidth", (const void*)_managed_internal_ImGUISetNextItemWidth),
	MethodType("YonaiEditor.ImGUI::_PlotLines", (const void*)_managed_internal_ImGUI_PlotLines),
	MethodType("YonaiEditor.ImGUI::_DockSpace", (const void*)_managed_internal_ImGUI_DockSpace),
	MethodType("YonaiEditor.ImGUI::_GetMouseDelta", (const void*)_managed_internal_ImGUI_GetMouseDelta),
	MethodType("YonaiEditor.ImGUI::GetMouseDeltaX", (const void*)_managed_internal_ImGUIGetMouseDeltaX),
	MethodType("YonaiEditor.ImGUI::GetMouseDeltaY", (const void*)_managed_internal_ImGUIGetMouseDeltaY),
	MethodType("YonaiEditor.ImGUI::_GetViewport", (const void*)_managed_internal_ImGUI_GetViewport),
	MethodType("YonaiEditor.ImGUI::GetMainViewportID", (const void*)_managed_internal_ImGUIGetMainViewportID),
	MethodType("YonaiEditor.ImGUI::GetWindowViewportID", (const void*)_managed_internal_ImGUIGetWindowViewportID),
	MethodType("YonaiEditor.ImGUI::_SetNextWindowPos", (const void*)_managed_internal_ImGUI_SetNextWindowPos),
	MethodType("YonaiEditor.ImGUI::_SetNextWindowSize", (const void*)_managed_internal_ImGUI_SetNextWindowSize),
	MethodType("YonaiEditor.ImGUI::SetNextWindowViewport", (const void*)_managed_internal_ImGUISetNextWindowViewport),
	MethodType("YonaiEditor.ImGUI::_GetWindowContentRegionMin", (const void*)_managed_internal_ImGUI_GetWindowContentRegionMin),
	MethodType("YonaiEditor.ImGUI::_GetWindowContentRegionMax", (const void*)_managed_internal_ImGUI_GetWindowContentRegionMax),
	MethodType("YonaiEditor.ImGUI::_GetContentRegionAvail", (const void*)_managed_internal_ImGUI_GetContentRegionAvail),
	MethodType("YonaiEditor.ImGUI::_GetWindowPosition", (const void*)_managed_internal_ImGUI_GetWindowPosition),
	MethodType("YonaiEditor.ImGUI::_GetWindowSize", (const void*)_managed_internal_ImGUI_GetWindowSize),
	MethodType("YonaiEditor.ImGUI::_IsWindowFocused", (const void*)_managed_internal_ImGUI_IsWindowFocused),
	MethodType("YonaiEditor.ImGUI::_IsWindowHovered", (const void*)_managed_internal_ImGUI_IsWindowHovered),
	MethodType("YonaiEditor.ImGUI::_IsWindowCollapsed", (const void*)_managed_internal_ImGUI_IsWindowCollapsed),
	MethodType("YonaiEditor.ImGUI::_PushStyleVar", (const void*)_managed_internal_ImGUI_PushStyleVar),
	MethodType("YonaiEditor.ImGUI::_PushStyleVarV", (const void*)_managed_internal_ImGUI_PushStyleVarV),
	MethodType("YonaiEditor.ImGUI::PopStyleVar", (const void*)_managed_internal_ImGUIPopStyleVar),
	MethodType("YonaiEditor.ImGUI::_PushStyleColour", (const void*)_managed_internal_ImGUI_PushStyleColour),
	MethodType("YonaiEditor.ImGUI::PopStyleColour", (const void*)_managed_internal_ImGUIPopStyleColour),
	MethodType("YonaiEditor.ImGUI::BeginMenuBar", (const void*)_managed_internal_ImGUIBeginMenuBar),
	MethodType("YonaiEditor.ImGUI::EndMenuBar", (const void*)_managed_internal_ImGUIEndMenuBar),
	MethodType("YonaiEditor.ImGUI::BeginMenu", (const void*)_managed_internal_ImGUIBeginMenu),
	MethodType("YonaiEditor.ImGUI::EndMenu", (const void*)_managed_internal_ImGUIEndMenu),
	MethodType("YonaiEditor.ImGUI::MenuItem", (const void*)_managed_internal_ImGUIMenuItem),
	MethodType("YonaiEditor.ImGUI::_BeginTable", (const void*)_managed_internal_ImGUI_BeginTable),
	MethodType("YonaiEditor.ImGUI::EndTable", (const void*)_managed_internal_ImGUIEndTable),
	MethodType("YonaiEditor.ImGUI::TableNextRow", (const void*)_managed_internal_ImGUITableNextRow),
	MethodType("YonaiEditor.ImGUI::TableSetColumnIndex", (const void*)_managed_internal_ImGUITableSetColumnIndex),
	MethodType("YonaiEditor.ImGUI::TableNextColumn", (const void*)_managed_internal_ImGUITableNextColumn),
	MethodType("YonaiEditor.ImGUI::TableHeadersRow", (const void*)_managed_internal_ImGUITableHeadersRow),
	MethodType("YonaiEditor.ImGUI::TableHeader", (const void*)_managed_internal_ImGUITableHeader),
	MethodType("YonaiEditor.ImGUI::TableSetColumnEnabled", (const void*)_managed_internal_ImGUITableSetColumnEnabled),
	MethodType("YonaiEditor.ImGUI::_TableGetColumnFlags", (const void*)_managed_internal_ImGUI_TableGetColumnFlags),
	MethodType("YonaiEditor.ImGUI::TableSetupScrollFreeze", (const void*)_managed_internal_ImGUITableSetupScrollFreeze),
	MethodType("YonaiEditor.ImGUI::_TableSetupColumn", (const void*)_managed_internal_ImGUI_TableSetupColumn),
	MethodType("YonaiEditor.ImGUI::Columns", (const void*)_managed_internal_ImGUIColumns),
	MethodType("YonaiEditor.ImGUI::NextColumn", (const void*)_managed_internal_ImGUINextColumn),
	MethodType("YonaiEditor.ImGUI::SetColumnWidth", (const void*)_managed_internal_ImGUISetColumnWidth),
	MethodType("YonaiEditor.ImGUI::SetColumnOffset", (const void*)_managed_internal_ImGUISetColumnOffset),
	MethodType("YonaiEditor.ImGUI::_BeginCombo", (const void*)_managed_internal_ImGUI_BeginCombo),
	MethodType("YonaiEditor.ImGUI::EndCombo", (const void*)_managed_internal_ImGUIEndCombo),
	MethodType("YonaiEditor.ImGUI::_AddLine", (const void*)_managed_internal_ImGUI_AddLine),
	MethodType("YonaiEditor.ImGUI::_AddRect", (const void*)_managed_internal_ImGUI_AddRect),
	MethodType("YonaiEditor.ImGUI::_AddRectFilled", (const void*)_managed_internal_ImGUI_AddRectFilled),
	MethodType("YonaiEditor.ImGUI::_AddQuad", (const void*)_managed_internal_ImGUI_AddQuad),
	MethodType("YonaiEditor.ImGUI::_AddQuadFilled", (const void*)_managed_internal_ImGUI_AddQuadFilled),
	MethodType("YonaiEditor.ImGUI::_AddTriangle", (const void*)_managed_internal_ImGUI_AddTriangle),
	MethodType("YonaiEditor.ImGUI::_AddTriangleFilled", (const void*)_managed_internal_ImGUI_AddTriangleFilled),
	MethodType("YonaiEditor.ImGUI::_AddCircle", (const void*)_managed_internal_ImGUI_AddCircle),
	MethodType("YonaiEditor.ImGUI::_AddCircleFilled", (const void*)_managed_internal_ImGUI_AddCircleFilled),
	MethodType("YonaiEditor.ImGUI::_AddNgon", (const void*)_managed_internal_ImGUI_AddNgon),
	MethodType("YonaiEditor.ImGUI::_AddNgonFilled", (const void*)_managed_internal_ImGUI_AddNgonFilled),
	MethodType("YonaiEditor.ImGUI::_AddText", (const void*)_managed_internal_ImGUI_AddText),
	MethodType("YonaiEditor.ImGUI::_AddBezierCubic", (const void*)_managed_internal_ImGUI_AddBezierCubic),
	MethodType("YonaiEditor.ImGUI::_AddBezierCurve", (const void*)_managed_internal_ImGUI_AddBezierCurve),
	MethodType("YonaiEditor.ImGUI::_AddBezierQuadratic", (const void*)_managed_internal_ImGUI_AddBezierQuadratic),
	MethodType("YonaiEditor.ImGUI::_AddImage", (const void*)_managed_internal_ImGUI_AddImage),
	MethodType("YonaiEditor.ImGUI::_AddImageRounded", (const void*)_managed_internal_ImGUI_AddImageRounded),
	MethodType("YonaiEditor.ImGUI::_PushClipRect", (const void*)_managed_internal_ImGUI_PushClipRect),
	MethodType("YonaiEditor.ImGUI::PopClipRect", (const void*)_managed_internal_ImGUIPopClipRect),
	MethodType("YonaiEditor.ImGUI::_DrawListPushClipRect", (const void*)_managed_internal_ImGUI_DrawListPushClipRect),
	MethodType("YonaiEditor.ImGUI::DrawListPushClipRectFullScreen", (const void*)_managed_internal_ImGUIDrawListPushClipRectFullScreen),
	MethodType("YonaiEditor.ImGUI::DrawListPopClipRect", (const void*)_managed_internal_ImGUIDrawListPopClipRect),
	MethodType("YonaiEditor.ImGUI::Gizmo_SetRect", (const void*)_managed_internal_ImGUIGizmo_SetRect),
	MethodType("YonaiEditor.ImGUI::Gizmo_SetDrawList", (const void*)_managed_internal_ImGUIGizmo_SetDrawList),
	MethodType("YonaiEditor.ImGUI::Gizmo_Enable", (const void*)_managed_internal_ImGUIGizmo_Enable),
	MethodType("YonaiEditor.ImGUI::_Gizmo_Manipulate", (const void*)_managed_internal_ImGUI_Gizmo_Manipulate),
	MethodType("YonaiEditor.Scripting::Reload", (const void*)_managed_internal_ScriptingReload),
	MethodType("YonaiEditor.Scripting::_DebuggingEnabled", (const void*)_managed_internal_Scripting_DebuggingEnabled),
	MethodType("YonaiEditor.Scripting::LoadAssembly", (const void*)_managed_internal_ScriptingLoadAssembly),
	MethodType("YonaiEditor.Scripting::IsAssemblyLoaded", (const void*)_managed_internal_ScriptingIsAssemblyLoaded)
};