#include <AquaEngine/Platform/NativeFileBrowser.hpp>

#if defined(AQUA_PLATFORM_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shobjidl.h> 
#endif

using namespace std;

namespace fs = std::filesystem;

#if defined(AQUA_PLATFORM_WINDOWS)
void Win32AddShellItem(IShellItem* item, vector<fs::path>& output)
{
	wchar_t* rawFilepath;
	item->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &rawFilepath);
	item->Release();

	fs::path filepath(rawFilepath);
	CoTaskMemFree(rawFilepath);
	output.emplace_back(filepath);
}

vector<fs::path> Win32OpenFileDialog(string& title, FILEOPENDIALOGOPTIONS options, vector<string> fileTypes = {})
{
	vector<fs::path> openedFiles = {};
	if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
		return {};

	IFileOpenDialog* dialog;
	HRESULT result = CoCreateInstance(
		CLSID_FileOpenDialog,
		NULL,
		CLSCTX_ALL,
		IID_IFileOpenDialog,
		(void**)&dialog
	);


	dialog->SetTitle((LPCWSTR)(wstring(title.begin(), title.end()).c_str()));
	dialog->SetOptions(options);

	vector<wstring> wFileTypes;
	vector<COMDLG_FILTERSPEC> filters;
	if (!fileTypes.empty())
	{
		for (size_t i = 0; i < fileTypes.size(); i++)
		{
			wFileTypes.emplace_back(fileTypes[i].begin(), fileTypes[i].end());
			filters.emplace_back(COMDLG_FILTERSPEC{ wFileTypes[i].data(), wFileTypes[i].data() });
		}

		dialog->SetFileTypes(filters.size(), filters.data());
	}

	if (FAILED(result))
	{
		CoUninitialize();
		return openedFiles;
	}

	result = dialog->Show(NULL);
	if (FAILED(result))
	{
		CoUninitialize();
		dialog->Release();
		return openedFiles;
	}

	if (options & FOS_ALLOWMULTISELECT)
	{
		IShellItemArray* items;
		dialog->GetResults(&items);
		DWORD itemsCount;
		items->GetCount(&itemsCount);
		for (DWORD i = 0; i < itemsCount; i++)
		{
			IShellItem* item;
			items->GetItemAt(i, &item);
			Win32AddShellItem(item, openedFiles);
		}
	}
	else
	{
		IShellItem* item;
		result = dialog->GetResult(&item);
		if (FAILED(result))
		{
			CoUninitialize();
			dialog->Release();
			return openedFiles;
		}

		Win32AddShellItem(item, openedFiles);
	}
	return openedFiles;
}
#endif

fs::path AquaEngine::Platform::OpenFolderDialog(string title)
{
#if defined(AQUA_PLATFORM_WINDOWS)
	vector<fs::path> paths = Win32OpenFileDialog(title, FOS_PICKFOLDERS);
	return paths.empty() ? fs::path() : paths[0];
#else
	// Failure, or closed window
	return fs::path();
#endif
}

fs::path AquaEngine::Platform::OpenFileDialog(string title, vector<string> extensionFilters)
{
#if defined(AQUA_PLATFORM_WINDOWS)
	vector<fs::path> paths = Win32OpenFileDialog(title, FOS_STRICTFILETYPES, extensionFilters);
	return paths.empty() ? fs::path() : paths[0];
#else
	// Failure, or closed window
	return fs::path();
#endif
}

vector<fs::path> AquaEngine::Platform::OpenFilesDialog(string title, vector<string> extensionFilters)
{
#if defined(AQUA_PLATFORM_WINDOWS)
	return Win32OpenFileDialog(title, FOS_STRICTFILETYPES | FOS_ALLOWMULTISELECT, extensionFilters);
#else
	// Failure, or closed window
	return { fs::path() };
#endif
}