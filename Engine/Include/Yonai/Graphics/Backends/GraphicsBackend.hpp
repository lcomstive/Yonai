#pragma once
#include <Yonai/API.hpp>

namespace Yonai::Graphics::Backends
{
	struct GraphicsBackend
	{
		YonaiAPI virtual void Init() = 0;
		YonaiAPI virtual void Draw() = 0;
		YonaiAPI virtual void Destroy() = 0;

		YonaiAPI virtual void OnResized() {}

		YonaiAPI virtual ~GraphicsBackend() {}
	};
}