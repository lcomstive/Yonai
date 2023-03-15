# Aqua Engine
Author - [@lcomstive](https://lewiscomstive.com) | [GitHub](https://www.github.com/lcomstive)

[![Version](https://img.shields.io/badge/Version-v0.0.1-4287f5?style=flat-square)](https://github.com/lcomstive/AquaEngine/releases)
[![GitHub Workflow Status](https://img.shields.io/github/actions/workflow/status/lcomstive/AquaEngine/AquaEngine.yaml?logo=GitHub&style=flat-square)](https://github.com/lcomstive/AquaEngine/actions/workflows/AquaEngine.yaml)

Table of Contents
====================

* [Installation](#installation)
  * [Editor](#editor)
  * [Engine](#engine)
* [Usage](#usage)
    * [Creating a new application](#creating-a-new-application)
    * [Running the application](#running-the-application)
* [Features](#features)
  * [Completed](#completed)
  * [In Development](#in-development)
  * [Planned](#planned)
* [Libraries](#libraries)
* [Licenses](#licenses)
    * [Main License](#main-license)
    * [Library Licenses](#library-licenses)
* [Contributing](#contributing)

* [Extra Stuff](#extra-stuff)
  * [Future Plans](#future-plans)
  * [FAQ](#faq)
  * [Support](#support)
  * [Acknowledgements](#acknowledgements)
  * [Credits](#credits)
  * [Contact](#contact)

---

Installation
===
## Editor
All releases of the editor can be found [here](https://github.com/lcomstive/AquaEngine/releases).

## Engine
Currently the only way to implement the engine in your own application is to build from source.
This can be done by cloning or adding [the repository](https://github.com/lcomstive/AquaEngine) as a submodule.

> When cloning remember to get submodules.
> On the command line this can be done using the `--recurse-submodules` flag.

[CMake](https://cmake.org/) is used for cross-platform project generation.

Usage
=====
## Creating a new application
Inheriting from `AquaEngine::Application` provides basic functionality, or you can use `AquaEngine::WindowedApplication` for
a graphical application.
```cpp
class DemoApplication :
    public AquaEngine::WindowedApplication
{
public:
	// Application is being created
	// 	[from AquaEngine::Application]
	void Setup() override;

	// Application is shutting down
	// 	[from AquaEngine::Application]
	void Cleanup() override;

	// Called once per frame, prior to drawing
	// 	[from AquaEngine::Application]
	void OnUpdate() override;

	// Called once per frame, after update
	// 	[from AquaEngine::WindowedApplication]
	void OnDraw() override;
};
```

### Running the application
```cpp
int main(int argc, char** argv)
{
	// Create instance of app
	DemoApplication app;

	// If DemoApplication inherits from AquaEngine::Application
	app.ProcessArgs(argc, argv);

	// Start the game loop
	app.Run();

	// Exit application
	return 0;
}
```

Features
========
* ### Completed
	* Core Engine
		* Custom "hybrid ECS" style systems
		* Centralised resource management
	* Components & Systems
		* 2D Sprite & 3D Mesh renderers
		* FPS-style camera movement
	* Graphics
		* Model loading (*using [Assimp](https://github.com/assimp/assimp) for various popular formats*)
		* Texture loading (*using [stb](https://github.com/nothings/stb) for image loading*)
		* GLSL shader loading
			* w/ hot reloading
		* ImGUI implementation
	* Debugging
		* Logging to console & file

* ### In Development
	* Graphics
		* Texture Maps (normal, diffuse, specular)
	* Scripting - C#
		* Communication between managed & unmanaged code (C++ <-> C#)
		* A core C# library - *[AquaScriptCore](./Apps/AquaScriptCore)*
		* Reloadable scripting engine support

* ### Planned
	* Core Engine
		* Asset database
		* Scene serialization
	* Graphics
		* Instancing
		* Shadow mapping
		* Particles
		* Rigged Animations
	* Physics
		* Third party 2D physics library - [Box2D](https://box2d.org/)
		* Third party 3D physics library - TBD
		
Dependencies
=========
Dependencies are installed using [VCPKG](https://github.com/microsoft/vcpkg) during the CMake configuration step.

* [Assimp](https://github.com/assimp/assimp)
* [Glad](https://github.com/Dav1dde/glad)
* [GLFW3](https://github.com/glfw/glfw)
* [GLM](https://github.com/g-truc/glm)
* [GoogleTest](https://github.com/google/googletest)
* [ImGUI](https://github.com/ocornut/imgui)
* [Mono](https://github.com/mono/mono)
* [SPDLog](https://github.com/gabime/spdlog)
* [STB](https://github.com/nothings/stb)

Licenses
=======
This project is released under the **MIT license**, and can be found in the [LICENSE](./LICENSE) file.

## Dependency Licenses
  * [Assimp License](https://github.com/assimp/assimp/blob/master/LICENSE)
  * [Glad](https://github.com/Dav1dde/glad/blob/master/LICENSE)
  * [GLFW3 License](https://github.com/glfw/glfw/blob/master/LICENSE.md)
  * [GLM License](https://github.com/g-truc/glm/blob/master/copying.txt)
  * [GoogleTest License](https://github.com/google/googletest/blob/main/LICENSE)
  * [ImGUI License](https://github.com/ocornut/imgui/blob/master/LICENSE.txt)
  * [Mono License](https://github.com/mono/mono/blob/main/LICENSE)
  * [SPDLog License](https://github.com/gabime/spdlog/blob/v1.x/LICENSE)
  * [STB License](https://github.com/nothings/stb/blob/master/LICENSE)
  * [VCPKG License](https://github.com/microsoft/vcpkg/blob/master/LICENSE.txt)

Contact
==========
Feel free to contact me at [lewiscomstive@gmail.com](mailto:lewiscomstive@gmail.com) with any questions or feedback.


Resources of study
==========
> Please [contact](#contact) me for removal if you or your resources are in this section and you do not want them to be.

* [Thera Engine \[Celeste Soueid\]](https://github.com/BlackxSnow/TheraEngine)
* [Eclipse Engine \[James Greensill\]](https://github.com/jwrmg/Eclipse)
* [Hazel Engine \[Yan Chernikov\]](https://github.com/TheCherno/Hazel)
* [LearnOpenGL.com \[Joey de Vries\]](https://learnopengl.com/)
