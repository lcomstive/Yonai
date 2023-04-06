#pragma once
#include <cstdlib>
#include <AquaEngine/API.hpp>

namespace AquaEngine
{
	class UUID
	{
		uint64_t m_UUID = 0;

	public:
		AquaAPI UUID();
		AquaAPI UUID(uint64_t uuid);
		AquaAPI UUID(const UUID&) = default; // Copy constructor

		operator uint64_t() const { return m_UUID; }
	};
}

namespace std
{
	// template<typename T> struct hash;

	template<>
	struct hash<AquaEngine::UUID>
	{
		std::size_t operator()(const AquaEngine::UUID& uuid) const
		{ return (uint64_t)uuid; }
	};
}