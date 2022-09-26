#pragma once

namespace AquaEditor
{
	struct View
	{
		virtual ~View() { }

		virtual void Update() { }
		virtual void Draw() = 0;
	};
}