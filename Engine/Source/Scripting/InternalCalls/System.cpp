#include <memory>
#include <unordered_map>
#include <mono/jit/jit.h>
#include <spdlog/spdlog.h>
#include <AquaEngine/SystemManager.hpp>
#include <AquaEngine/Systems/System.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>
#include <AquaEngine/Scripting/UnmanagedThunks.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Systems;
using namespace AquaEngine::Scripting;

extern EmptyMethodFn SystemMethodStart;
extern EmptyMethodFn SystemMethodUpdate;
extern EmptyMethodFn SystemMethodDestroyed;
extern SystemMethodEnabledFn SystemMethodEnabled;
extern SystemMethodInitialiseFn SystemMethodInitialise;
