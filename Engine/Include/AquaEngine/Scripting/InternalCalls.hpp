/** @file */

#pragma once
#include <tuple>
#include <utility> // for std::pair
#include <glm/glm.hpp>
#include <mono/jit/jit.h>

/** @cond */
#define EXPAND(x) x
#define GET_MACRO(_1, _2, _3, _4, _5, NAME, ...) NAME

#define ADD_MANAGED_METHOD_5(className, functionName, returnType, params, namespaceName) \
	returnType _managed_internal_##className##functionName params
#define ADD_MANAGED_METHOD_4(className, functionName, returnType, params) ADD_MANAGED_METHOD_5(className, functionName, returnType, params, AquaEngine)
#define ADD_MANAGED_METHOD_3(className, functionName, returnType) ADD_MANAGED_METHOD_5(className, functionName, returnType, (), AquaEngine)
#define ADD_MANAGED_METHOD_2(className, functionName) ADD_MANAGED_METHOD_5(className, functionName, void, (), AquaEngine)
/** @endcond */

/**
	@hideinitializer
	
	@brief Creates a function definition that is made available in C# scripting.
	
	@param ClassName	 Name of class in C#
	@param FunctionName	 Name of function in C#. This is prepended by an underscore and made available using <code>[MethodImpl(MethodImplOptions.InternalCall)]</code>.
	@param ReturnType	 (Optional) Return type for function.
	@param Params		 (Optional) Parameter types and names. These must be encapsulated by parenthesis.
	@param NamespaceName (Optional) The namespace surrounding the C# class. Default is <code>AquaEngine</code>.
*/
#define ADD_MANAGED_METHOD(...) EXPAND(GET_MACRO(__VA_ARGS__, \
		ADD_MANAGED_METHOD_5,	\
		ADD_MANAGED_METHOD_4,	\
		ADD_MANAGED_METHOD_3,	\
		ADD_MANAGED_METHOD_2	\
	)(__VA_ARGS__))

#define GET_MANAGED_NAME(className, fn) _managed_internal_##className##functionName

#define ADD_MANAGED_TYPE(namespaceName, className, unmanagedClass)
