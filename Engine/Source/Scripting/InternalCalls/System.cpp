#include <memory>
#include <unordered_map>
#include <mono/jit/jit.h>
#include <spdlog/spdlog.h>
#include <Yonai/SystemManager.hpp>
#include <Yonai/Systems/System.hpp>
#include <Yonai/Scripting/Assembly.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>
#include <Yonai/Scripting/UnmanagedThunks.hpp>

using namespace std;
using namespace Yonai;
using namespace Yonai::Systems;
using namespace Yonai::Scripting;

extern EmptyMethodFn SystemMethodStart;
extern EmptyMethodFn SystemMethodUpdate;
extern EmptyMethodFn SystemMethodDestroyed;
extern SystemMethodEnabledFn SystemMethodEnabled;
extern SystemMethodInitialiseFn SystemMethodInitialise;
