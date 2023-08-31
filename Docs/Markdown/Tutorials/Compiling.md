# Compiling the Library {#Compiling}

## Setup
Clone the [repository](https://github.com/lcomstive/Yonai), remembering to get the submodules.

This can either be done through a GUI client like [GitHub Desktop](https://desktop.github.com/) or the command line

> `git clone --recurse-submodules https://github.com/lcomstive/Yonai`

## Command line
```bash
# Clone repository, if not done already
git clone --recurse-submodules https://github.com/lcomstive/Yonai.git
cd Yonai

# Configure source
cmake -B ./build -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON

# Build C# projects
msbuild Apps/YonaiScriptCore/YonaiScriptCore.csproj 	-property:Configuration=Release
msbuild Apps/YonaiScriptEditor/YonaiScriptEditor.csproj -property:Configuration=Release

# Build all
cmake --build ./build --config Release
```

The C# projects can only be built in either `Debug` or `Release` mode.<br>
Their output is copied from the respective `bin/` directories, if `CMAKE_BUILD_TYPE` is `Debug` it searches `bin/Debug` otherwise `bin/Release` (*you will get an error during editor compilation if the folder does not exist*).

> To create a universal binary for Mac OS, set `CMAKE_OSX_ARCHITECTURES` to `arm64;x86_64`
