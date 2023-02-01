#include <regex>
#include <fstream>
#include <utility>
#include <iostream>
#include <filesystem>

#define SHOW_DEBUG_OUTPUT 0

using namespace std;

namespace fs = std::filesystem;

string ReadFile(fs::path path);

struct FunctionInfo
{
	string ClassName, FunctionName, ReturnType, Parameters, NamespaceName;
};

string BaseContent = string("#pragma once\n") +
					"#include <vector>\n" +
					"#include <string>\n" +
					"#include <glm/glm.hpp>\n" +
					"#include <mono/jit/jit.h>\n\n" +
					"using namespace std; \n\n";

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		cout << "Usage: " << argv[0] << " <Source Directory> <Output File>" << endl;
		return -1;
	}

	string sourceDirectory = argv[1];
	string outputFile = argv[2];

	if (!fs::exists(sourceDirectory))
	{
		cerr << "Source directory could not be found" << endl;
		return -2;
	}

	cout << "Checking source files in '" << sourceDirectory << "'" << endl;

	vector<FunctionInfo> internalMethods;

	// Find all internal methods in C++ source files
	smatch matches;
	regex methodRegex("ADD_MANAGED_METHOD\\((\\w+),\\s*(\\w+)(,\\s*([_ A-Za-z\\*]+)(,\\s*\\(([\\w\\* _,:]+)\\)(,\\s*([\\w\\._]+))?)?)?");
	for (fs::recursive_directory_iterator it(sourceDirectory), end; it != end; it++)
	{
		fs::path path = it->path();
		if (!path.has_extension() || path.extension().compare(".cpp") != 0)
			continue; // Not a .cpp file

		string content = ReadFile(path);

		while (regex_search(content, matches, methodRegex))
		{
			// matches[1] = class name
			// matches[2] = function name
			FunctionInfo info = { matches[1], matches[2] };
			info.ReturnType = matches[4].matched ? matches[4] : string("void");
			info.NamespaceName = matches[8].matched ? matches[8] : string("AquaEngine");

			if (matches[6].matched)
				info.Parameters = matches[6];
				
			internalMethods.push_back(info);

#if SHOW_DEBUG_OUTPUT // Debugging output
			cout << info.ReturnType << " " << info.NamespaceName;
			cout << "." << info.ClassName << "::" << info.FunctionName << "(" << info.Parameters << ")" << endl;
#endif

			// Prevent searching same portion of content
			content = matches.suffix().str();
		}
	}

	// Iterate over all matches and add to header file
	string contents = BaseContent;

	for (FunctionInfo info : internalMethods)
		contents += info.ReturnType + " _managed_internal_" + info.ClassName + info.FunctionName + "(" + info.Parameters + ");\n";

	// Array of methods
	contents += "\n\nconst vector<pair<const char*, const void*>> _InternalMethods = {\n";
	for (size_t i = 0; i < internalMethods.size(); i++)
	{
		FunctionInfo info = internalMethods[i];
		contents += "\tpair<const char*, const void*>(\"" + info.NamespaceName + "." + info.ClassName + "::_" + info.FunctionName + "\", ";
		contents += "(const void*)_managed_internal_" + info.ClassName + info.FunctionName + ")";

		if (i < internalMethods.size() - 1)
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