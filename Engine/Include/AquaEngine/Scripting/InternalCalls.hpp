#pragma once
#include <tuple>
#include <utility> // for std::pair
#include <glm/glm.hpp>
#include <mono/jit/jit.h>

#define ADD_MANAGED_METHOD_5(className, functionName, returnType, params, namespaceName) \
	returnType _managed_internal_##className##functionName params
#define ADD_MANAGED_METHOD_4(className, functionName, returnType, params) ADD_MANAGED_METHOD_5(className, functionName, returnType, params, AquaEngine)
#define ADD_MANAGED_METHOD_3(className, functionName, returnType) ADD_MANAGED_METHOD_5(className, functionName, returnType, (), AquaEngine)
#define ADD_MANAGED_METHOD_2(className, functionName) ADD_MANAGED_METHOD_5(className, functionName, void, (), AquaEngine)

#define EXPAND(x) x

#define GET_MACRO(_1, _2, _3, _4, _5, NAME, ...) NAME
#define ADD_MANAGED_METHOD(...) EXPAND(GET_MACRO(__VA_ARGS__, \
		ADD_MANAGED_METHOD_5,	\
		ADD_MANAGED_METHOD_4,	\
		ADD_MANAGED_METHOD_3,	\
		ADD_MANAGED_METHOD_2	\
	)(__VA_ARGS__))

#define GET_MANAGED_NAME(className, fn) _managed_internal_##className##functionName

/*

	USAGE

	ADD_MANAGED_METHOD(
		className,
		functionName,
		returnType,
		params
	)

	className: Name of class. Matching C# binding

	functionName: Name of function. This is prefixed by '_' in the C# binding

	returnType: Type to return in managed code

	params: Input parameters for method. Must be encapsulated by parenthesis


	EXAMPLE

	// C++ 
	ADD_MANAGED_METHOD(TestClass, Greeting, MonoString*, (std::string name))
	{
		return mono_string_new(mono_domain_get(), ("Hello, " + name).c_str());
	}

	ADD_MANAGED_METHOD(TestClass, HelloWorld)
	{
		std::cout << "Hello, world!" << std::endl;
	}

	// C#
	using System.Runtime.CompilerServices; // For internal call attribute

	namespace AquaEngine
	{
		public class TestClass
		{
			// The internal C++ calls
			[MethodImpl(MethodImplOptions.InternalCall)]
			private static extern string _Greeting(string name);

			[MethodImpl(MethodImplOptions.InternalCall)]
			private static extern void _HelloWorld();

			// Provide public facing access to internal calls
			public static string Greeting(string name) => _Greeting(name);
			public static void HelloWorld() => _HelloWorld();
		}
	}
*/