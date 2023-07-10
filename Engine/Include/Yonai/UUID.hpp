#pragma once
#include <cstdlib>
#include <Yonai/API.hpp>

namespace Yonai
{
	class UUID
	{
		uint64_t m_UUID = 0;

	public:
		YonaiAPI UUID();
		YonaiAPI UUID(uint64_t uuid);
		YonaiAPI UUID(const UUID&) = default; // Copy constructor

		operator uint64_t() const { return m_UUID; }
	};
}

namespace std
{
	// template<typename T> struct hash;

	template<>
	struct hash<Yonai::UUID>
	{
		std::size_t operator()(const Yonai::UUID& uuid) const
		{ return (uint64_t)uuid; }
	};
}