/** @file */

#pragma once
#include <tuple>
#include <glm/glm.hpp>
#include <mono/jit/jit.h>

/** @cond */
#define EXPAND(x) x
#define GET_MACRO(_1, _2, _3, _4, _5, NAME, ...) NAME
#define GET_PROPERTY_MACRO(_1, _2, _3, _4, NAME, ...) NAME

#define ADD_MANAGED_METHOD_5(className, functionName, returnType, params, namespaceName) \
	returnType _managed_internal_##className##functionName params
#define ADD_MANAGED_METHOD_4(className, functionName, returnType, params) ADD_MANAGED_METHOD_5(className, functionName, returnType, params, Yonai)
#define ADD_MANAGED_METHOD_3(className, functionName, returnType) ADD_MANAGED_METHOD_5(className, functionName, returnType, (), Yonai)
#define ADD_MANAGED_METHOD_2(className, functionName) ADD_MANAGED_METHOD_5(className, functionName, void, (), Yonai)
/** @endcond */

/**
	@hideinitializer
	
	@brief Creates a function definition that is made available in C# scripting.
	
	@param ClassName	 Name of class in C#
	@param FunctionName	 Name of function in C#. This is prepended by an underscore and made available using <code>[MethodImpl(MethodImplOptions.InternalCall)]</code>.
	@param ReturnType	 (Optional) Return type for function.
	@param Params		 (Optional) Parameter types and names. These must be encapsulated by parenthesis.
	@param NamespaceName (Optional) The namespace surrounding the C# class. Default is <code>Yonai</code>.
*/
#define ADD_MANAGED_METHOD(...) EXPAND(GET_MACRO(__VA_ARGS__, \
		ADD_MANAGED_METHOD_5,	\
		ADD_MANAGED_METHOD_4,	\
		ADD_MANAGED_METHOD_3,	\
		ADD_MANAGED_METHOD_2	\
	)(__VA_ARGS__))

#define GET_MANAGED_NAME(className, fn) _managed_internal_##className##functionName

#define ADD_MANAGED_GET_NS(className, property, type, namespaceName) \
	ADD_MANAGED_METHOD(className, Get##property, type, (void* handle), namespaceName) \
	{ return ((className*)handle)->property; }

#define ADD_MANAGED_GET_(className, property, type) ADD_MANAGED_GET_NS(className, property, type, Yonai)

#define ADD_MANAGED_GET(...) EXPAND(GET_PROPERTY_MACRO(__VA_ARGS__, \
		ADD_MANAGED_GET_NS,	\
		ADD_MANAGED_GET_	\
	)(__VA_ARGS__))

#define ADD_MANAGED_SET_NS(className, property, type, namespaceName) \
	ADD_MANAGED_METHOD(className, Set##property, void, (void* handle, type value), namespaceName) \
	{ ((className*)handle)->property = value; }
#define ADD_MANAGED_SET_(className, property, type) ADD_MANAGED_SET_NS(className, property, type, Yonai)

#define ADD_MANAGED_SET(...) EXPAND(GET_PROPERTY_MACRO(__VA_ARGS__, \
		ADD_MANAGED_SET_NS,	\
		ADD_MANAGED_SET_	\
	)(__VA_ARGS__))

#define ADD_MANAGED_GET_SET_NS(className, property, type, namespaceName) \
	ADD_MANAGED_GET(className, property, type, namespaceName) \
	ADD_MANAGED_SET(className, property, type, namespaceName)
#define ADD_MANAGED_GET_SET_(className, property, type) \
	ADD_MANAGED_GET(className, property, type) \
	ADD_MANAGED_SET(className, property, type)

#define ADD_MANAGED_GET_SET(...) EXPAND(GET_PROPERTY_MACRO(__VA_ARGS__, \
	ADD_MANAGED_GET_SET_NS,	\
	ADD_MANAGED_GET_SET_	\
)(__VA_ARGS__))

// string ClassName, FunctionName, ReturnType, Parameters, NamespaceName;