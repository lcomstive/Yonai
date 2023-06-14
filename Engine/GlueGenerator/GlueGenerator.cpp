#include <regex>
#include <fstream>
#include <utility>
#include <iostream>
#include <filesystem>

#define SHOW_DEBUG_OUTPUT 0

using namespace std;

namespace fs = std::filesystem;

string ReadFile(fs::path path);
void ReplaceText(string& content, const string& from, const string& to);
void AddGetter(const string& className, const string& propertyName, const string& type, const string& namespaceName);
void AddSetter(const string& className, const string& propertyName, const string& type, const string& namespaceName);

struct FunctionInfo
{
	string ClassName, FunctionName, ReturnType, Parameters, NamespaceName;
};

string BaseContent = string("#pragma once\n")		+
					"#include <vector>\n"			+
					"#include <string>\n"			+
					"#include <glm/glm.hpp>\n"		+
					"#include <mono/jit/jit.h>\n\n" +
					"using namespace std; \n\n"		+
					"typedef pair<const char*, const void*> MethodType; \n\n";

vector<FunctionInfo> InternalMethods = {};

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		cout << "Usage: " << argv[0] << " [options] <Source Directory> <Output File>" << endl;
		cout << "Options:" << endl;
		cout << " -prefix=<string_prefix>\tSets the function name prefix when binding C# internal calls" << endl;
		return -1;
	}

	string sourceDirectory = argv[argc - 2];
	string outputFile = argv[argc - 1];
	string prefix = "";

	for (int i = 0; i < argc; i++)
	{
		string arg(argv[i]);
		if (arg.rfind("-prefix=", 0) == 0)
			prefix = arg.substr(string("-prefix=").length());
	}

#if SHOW_DEBUG_OUTPUT // Debugging output
	cout << "Prefix: " << prefix << endl;
#endif

	if (!fs::exists(sourceDirectory))
	{
		cerr << "Source directory could not be found" << endl;
		return -2;
	}

	cout << "Checking source files in '" << sourceDirectory << "'" << endl;

	// Find all internal methods in C++ source files
	smatch matches;
	regex methodRegex("ADD_MANAGED_METHOD\\((\\w+),\\s*(\\w+)(,\\s*([_ A-Za-z\\*:0-9]+)(,\\s*\\(([\\w\\*\\s_,:]*)\\)(,\\s*([\\w\\._]+))?)?)?");
	regex propertyRegex("ADD_MANAGED_(GET|SET|GET_SET)\\((\\w+),\\s*(\\w+)(,\\s*(([_ A-Za-z\\*:0-9]+)(,\\s*([\\w\\._]+))?))?");
	for (fs::recursive_directory_iterator it(sourceDirectory), end; it != end; it++)
	{
		fs::path path = it->path();
		if (!path.has_extension() ||
			(path.extension().compare(".cpp") != 0 && path.extension().compare(".mm") != 0))
			continue; // Not a valid source file

		string originalContent = ReadFile(path);

#if SHOW_DEBUG_OUTPUT // Debugging output
		cout << "Reading file " << path << "..." << endl;
#endif


		string content = originalContent;
		while (regex_search(content, matches, methodRegex))
		{
			// matches[1] = class name
			// matches[2] = function name
			// matches[4] = return type
			// matches[6] = parameters
			// matches[8] = namespace
			FunctionInfo info = { matches[1], matches[2] };
			info.ReturnType = matches[4].matched ? matches[4] : string("void");
			info.NamespaceName = matches[8].matched ? matches[8] : string("AquaEngine");

			if (matches[6].matched)
			{
				info.Parameters = matches[6];

				// Remove newlines and tabs.
				// Somewhat improves readability of output file for debugging issues
				ReplaceText(info.Parameters, "\n", "");
				ReplaceText(info.Parameters, "\r", "");
				ReplaceText(info.Parameters, "\t", " ");
			}
				
			InternalMethods.push_back(info);

#if SHOW_DEBUG_OUTPUT // Debugging output
			cout << info.ReturnType << " " << info.NamespaceName;
			cout << "." << info.ClassName << "::" << info.FunctionName << "(" << info.Parameters << ")" << endl;
#endif

			// Prevent searching same portion of content
			content = matches.suffix().str();
		}

		// Reset content
		matches = smatch();
		content = originalContent;
		while(regex_search(content, matches, propertyRegex))
		{
			// matches[1] = type (get, set, get_set)
			// matches[2] = class name
			// matches[3] = property name
			// matches[6] = property type
			// matches[8] = namespace
			
			auto type = matches[1];
			auto className = matches[2];
			auto propertyName = matches[3];
			auto propertyType = matches[6];
			auto namespaceName = matches[8].matched ? matches[8] : string("AquaEngine");

			if(type.compare("GET") == 0 || type.compare("GET_SET") == 0)
				AddGetter(className, propertyName, propertyType, namespaceName);
			if(type.compare("SET") == 0 || type.compare("GET_SET") == 0)
				AddSetter(className, propertyName, propertyType, namespaceName);

			// Prevent searching same portion of content
			content = matches.suffix().str();
		}
	}

	// Iterate over all matches and add to header file
	string contents = BaseContent;

	for (FunctionInfo info : InternalMethods)
		contents += info.ReturnType + " _managed_internal_" + info.ClassName + info.FunctionName + "(" + info.Parameters + ");\n";

	// Array of methods
	contents += "\n\nconst vector<pair<const char*, const void*>> _InternalMethods = {\n";
	for (size_t i = 0; i < InternalMethods.size(); i++)
	{
		FunctionInfo info = InternalMethods[i];
		contents += "\tMethodType(\"" + info.NamespaceName + "." + info.ClassName + "::" + prefix + info.FunctionName + "\", ";
		contents += "(const void*)_managed_internal_" + info.ClassName + info.FunctionName + ")";

		if (i < InternalMethods.size() - 1)
			contents += ",";
		contents += "\n";
	};
	contents += "};";

	ofstream outputStream(outputFile, ios::out | ios::trunc);
	outputStream << contents;
	outputStream.flush();

	cout << "Written glue contents to '" << outputFile << "'" << endl;

#if SHOW_DEBUG_OUTPUT
	int _ = getchar();
#endif
	return 0;
}

string ReadFile(fs::path path)
{
	// Open file for read and get size
	ifstream filestream(path.string(), ios::in | ios::binary);

	// Get file length
	streampos size = fs::file_size(path);

	// Read in contents of file
	string contents(size, '\0');
	filestream.read(contents.data(), size);

	return contents;
}

void ReplaceText(string& content, const string& from, const string& to)
{
	if (from.empty())
		return;

	size_t startPos = 0;
	while ((startPos = content.find(from, startPos)) != string::npos)
	{
		content.replace(startPos, from.length(), to);
		startPos += to.length();
	}
}

void AddGetter(const string& className, const string& propertyName, const string& type, const string& namespaceName)
{
	InternalMethods.push_back(FunctionInfo
	{
		className,
		"Get" + propertyName,
		type,
		"void* handle",
		namespaceName
	});

#if SHOW_DEBUG_OUTPUT // Debugging output
			cout << "[Getter] " << namespaceName << "." << className << "->" << propertyName << endl;
#endif
}

void AddSetter(const string& className, const string& propertyName, const string& type, const string& namespaceName)
{
	InternalMethods.push_back(FunctionInfo
	{
		className,
		"Set" + propertyName,
		"void",
		"void* handle, " + type + " value",
		namespaceName
	});

#if SHOW_DEBUG_OUTPUT // Debugging output
			cout << "[Setter] " << namespaceName << "." << className << "->" << propertyName << endl;
#endif
}
