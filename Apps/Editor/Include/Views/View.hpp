#pragma once

namespace AquaEditor
{
	class EditorApp;

	struct View
	{
		virtual ~View() { }

		virtual void Open() { }
		virtual void Close() { }

		virtual void Update() { }
		virtual void Draw() = 0;
	};
}