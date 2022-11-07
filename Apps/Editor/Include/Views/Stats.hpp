#pragma once
#include <Views/View.hpp>

namespace AquaEditor
{
	struct StatsView : public View
	{
		void Open() override;
		void Close() override;

		void Draw() override;

	private:
		bool m_IsOpen = false;
	};
}