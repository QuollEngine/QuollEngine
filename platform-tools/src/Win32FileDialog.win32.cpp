#include "liquid/core/Base.h"
#include "platform-tools/NativeFileDialog.h"
#include <windows.h>
#include <shobjidl.h>

namespace liquid::platform_tools {

static void checkWin32Error(HRESULT res, const liquid::String &message) {
  LIQUID_ASSERT(SUCCEEDED(res), "Failed to open file dialog " + message);
}

liquid::String NativeFileDialog::getFilePathFromDialog(
    const std::vector<liquid::String> &extensions) {
  IFileDialog *pFileOpen = nullptr;
  checkWin32Error(CoCreateInstance(CLSID_FileOpenDialog, nullptr,
                                   CLSCTX_INPROC_SERVER,
                                   IID_PPV_ARGS(&pFileOpen)),
                  "Cannot create file dialog instance");

  COMDLG_FILTERSPEC filters;
  filters.pszName = L"All supported files";
  std::wstring wideExtensions;

  std::wstringstream wss;
  for (auto &x : extensions) {
    wss << L"*." << x.c_str() << ";";
  }
  wideExtensions = wss.str();
  filters.pszSpec = wideExtensions.c_str();

  std::string str = std::string(wideExtensions.begin(), wideExtensions.end());

  pFileOpen->SetFileTypes(1, &filters);

  if (!SUCCEEDED(pFileOpen->Show(NULL))) {
    return "";
  }

  IShellItem *pItem = nullptr;
  if (!SUCCEEDED(pFileOpen->GetResult(&pItem))) {
    return "";
  }

  LPWSTR pszFilePath = nullptr;
  if (!SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath))) {
    return "";
  }

  std::wstring ws(pszFilePath);
  std::string filePath = std::string(ws.begin(), ws.end());

  CoTaskMemFree(pszFilePath);
  pItem->Release();
  pFileOpen->Release();
  CoUninitialize();
  return filePath;
}

} // namespace liquid::platform_tools
