#include <spdlog/spdlog.h>
#include <Yonai/Graphics/Graphics.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>

// Backends //
#include <Yonai/Graphics/Backends/VulkanBackend.hpp>

#if defined(YONAI_PLATFORM_WINDOWS)
// TODO: DirectX backend
// #include <Yonai/Graphics/Backends/DirectXBackend.hpp>
#endif

using namespace std;
using namespace Yonai::Graphics;
using namespace Yonai::Graphics::Backends;

GraphicsAPI Graphics::s_API = GraphicsAPI::None;
unique_ptr<GraphicsBackend> Graphics::s_Backend = nullptr;

void Graphics::Init()
{
	if (s_Backend)
		return; // Already initialised

	switch (s_API)
	{
	case GraphicsAPI::None:
		spdlog::debug("No graphics API selected");
		break;
	case GraphicsAPI::Vulkan:
		s_Backend = make_unique<VulkanBackend>();
		break;
	}
}

void Graphics::Destroy()
{
	if(s_Backend)
		s_Backend.reset();
	s_Backend = nullptr;
}

GraphicsAPI Graphics::API() { return s_API; }

void Graphics::API(GraphicsAPI api)
{
	if (api == s_API)
		return; // No change

	if (s_Backend)
		Destroy();

	s_API = api;
	Init();
}

ADD_MANAGED_METHOD(Graphics, SetAPI, void, (int api), Yonai.Graphics)
{ Graphics::API((GraphicsAPI)api); }