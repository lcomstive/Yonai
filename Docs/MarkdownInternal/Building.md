# Building

 - Install [CMake](https://cmake.org/), if you don't have it already
 - Clone the [repository](https://github.com/lcomstive/Yonai), this can be done either by a GUI client or the command line.
 	> Make sure you recursively clone to get the submodules as well
	
	e.g. `git clone --recurse-submodules https://github.com/lcomstive/Yonai`
 - Configure CMake using the *Yonai* source directory
	
	e.g. `cmake -B ./build -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON`
 - If you're on Unix platforms (*this includes Mac OS*), you'll need to compile `YonaiScriptCore` and `YonaiScriptEditor` prior to building *Yonai*. This can be achieved using `msbuild` for instance.

	`msbuild Apps/YonaiScriptCore/YonaiScriptCore.csproj -property:Configuration=Release`

	`msbuild Apps/YonaiScriptEditor/YonaiScriptEditor.csproj -property:Configuration=Release`
 - Build the project using your chosen generator

	e.g. `cmake --build ./build --config Release`

## Targets
| Target | Description |
|-------:|:------------|
| Yonai | Engine library (C++) |
| YonaiScriptCore | C# library that interfaces with the engine |
| YonaiEditor | Executable GUI editor and game builder |
| YonaiScriptEditor | C# library that interfaces with the editor |
| GlueGenerator | Generates a file to map C++ functions to C# internal calls |
| BaseGame | Bootstrap executable to launch C# assemblies and load the first scene |
| YonaiTest | Unit tests for various parts of the engine |

## Options

## Linking managed and unmanaged calls