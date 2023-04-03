#include <AquaEngine/Graphics/Pipelines/ScriptedPipeline.hpp>

using namespace AquaEngine::Graphics;
using namespace AquaEngine::Graphics::Pipelines;

ScriptedRenderPipeline::ScriptedRenderPipeline() {}
ScriptedRenderPipeline::~ScriptedRenderPipeline() {}

void ScriptedRenderPipeline::Draw(Components::Camera* camera) {}

Framebuffer* ScriptedRenderPipeline::GetOutput() { return nullptr; }

void ScriptedRenderPipeline::OnResized(glm::ivec2 resolution) { }