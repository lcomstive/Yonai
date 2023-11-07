#pragma once
#include <Yonai/UUID.hpp>

namespace Yonai
{
	class Resource; // Forward declaration

	struct ResourceBase
	{
		UUID m_ResourceID;

		friend class Yonai::Resource;

	public:
		YonaiAPI UUID& ID() { return m_ResourceID; }

		YonaiAPI virtual ~ResourceBase() {}
	};
}