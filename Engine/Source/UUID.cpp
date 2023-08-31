#include <random>
#include <unordered_map>
#include <Yonai/UUID.hpp>

using namespace std;
using namespace Yonai;

static random_device s_RandomDevice;
static mt19937_64 s_Engine(s_RandomDevice());
static uniform_int_distribution<uint64_t> s_UniformDistribution;

// Generate UUID
UUID::UUID() : m_UUID(s_UniformDistribution(s_Engine)) { }

// Set UUID
UUID::UUID(uint64_t uuid) : m_UUID(uuid) { }