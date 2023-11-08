#pragma once
#include <Yonai/API.hpp>

namespace Yonai::Graphics::Backends
{
	struct GraphicsBackend
	{
		YonaiAPI virtual void Init() abstract;
		YonaiAPI virtual void Draw() abstract;
		YonaiAPI virtual void Destroy() abstract;

		YonaiAPI virtual void OnResized() {}

		YonaiAPI virtual ~GraphicsBackend() {}
	};
}