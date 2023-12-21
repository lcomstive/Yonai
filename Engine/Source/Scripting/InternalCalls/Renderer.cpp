#include <glm/glm.hpp>
#include <glad/glad.h>
#include <Yonai/Scripting/InternalCalls.hpp>

#include <Yonai/Graphics/Mesh.hpp>
#include <Yonai/Graphics/Shader.hpp>
#include <Yonai/Components/Camera.hpp>

using namespace glm;
using namespace Yonai::Graphics;
using namespace Yonai::Components;

ADD_MANAGED_METHOD(Renderer, ClearColour, void, (glm::vec4* colour), Yonai.Graphics)
{ glClearColor(colour->r, colour->g, colour->b, colour->a); }

ADD_MANAGED_METHOD(Renderer, Clear, void, (unsigned int flags), Yonai.Graphics)
{ glClear(flags); }

ADD_MANAGED_METHOD(Renderer, SetViewport, void, (glm::ivec4* viewport), Yonai.Graphics)
{ glViewport(viewport->x, viewport->y, viewport->z, viewport->w); }

ADD_MANAGED_METHOD(Renderer, DrawMesh, void, (void* mesh), Yonai.Graphics)
{ ((Mesh*)mesh)->Draw(); }

ADD_MANAGED_METHOD(Renderer, UploadCamera, void, (void* shader, void* camera, glm::vec2* resolution), Yonai.Graphics)
{ ((Camera*)camera)->FillShader((Shader*)shader, *resolution); }

ADD_MANAGED_METHOD(Renderer, UploadTransform, void, (void* shader, void* transform), Yonai.Graphics)
{ ((Shader*)shader)->Set("modelMatrix", ((Transform*)transform)->GetModelMatrix()); }

ADD_MANAGED_METHOD(Renderer, Enable, void, (unsigned int flag), Yonai.Graphics)
{ glEnable(flag); }

ADD_MANAGED_METHOD(Renderer, Disable, void, (unsigned int flag), Yonai.Graphics)
{ glDisable(flag); }

ADD_MANAGED_METHOD(Renderer, CullFace, void, (unsigned int face), Yonai.Graphics)
{ glCullFace(face); }

ADD_MANAGED_METHOD(Renderer, BeginDebugGroup, void, (MonoString* name), Yonai.Graphics)
{
	char* description = mono_string_to_utf8(name);
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, description);
	mono_free(description);
}

ADD_MANAGED_METHOD(Renderer, EndDebugGroup, void, (MonoString* name), Yonai.Graphics)
{ glPopDebugGroup(); }

ADD_MANAGED_METHOD(Renderer, PolygonMode, void, (unsigned int mode), Yonai.Graphics)
{ glPolygonMode(GL_FRONT_AND_BACK, mode); }