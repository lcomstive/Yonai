#include <Yonai/Graphics/Pipelines/ScriptedPipeline.hpp>

using namespace Yonai::Graphics;
using namespace Yonai::Graphics::Pipelines;

ScriptedRenderPipeline::ScriptedRenderPipeline() {}
ScriptedRenderPipeline::~ScriptedRenderPipeline() {}

void ScriptedRenderPipeline::Draw(Components::Camera* camera) {}

Framebuffer* ScriptedRenderPipeline::GetOutput() { return nullptr; }

void ScriptedRenderPipeline::OnResized(glm::ivec2 resolution) { }