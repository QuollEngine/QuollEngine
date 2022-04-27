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
  int length = WideCharToMultiByte(
      CP_UTF8, 0, &ws.at(0), static_cast<int>(ws.size()), NULL, 0, NULL, NULL);

  liquid::String filePath(length, 0);
  WideCharToMultiByte(CP_UTF8, 0, &ws.at(0), static_cast<int>(ws.size()),
                      &filePath.at(0), length, NULL, NULL);

  CoTaskMemFree(pszFilePath);
  pItem->Release();
  pFileOpen->Release();
  CoUninitialize();
  return filePath;
}

/**
 * @brief Get file path from OS create file dialog
 *
 * @param extensions File extensions to show
 * @return Chosen file path or empty string if cancelled
 */
std::filesystem::path NativeFileDialog::getFilePathFromCreateDialog(
    const std::vector<liquid::String> &extensions) {
  IFileDialog *pFileOpen = nullptr;
  checkWin32Error(CoCreateInstance(CLSID_FileSaveDialog, nullptr,
                                   CLSCTX_INPROC_SERVER,
                                   IID_PPV_ARGS(&pFileOpen)),
                  "Cannot create save as file dialog instance");

  COMDLG_FILTERSPEC filters;
  filters.pszName = L"All supported files";
  std::wstring wideExtensions;

  std::wstringstream wss;
  for (auto &x : extensions) {
    wss << L"*." << x.c_str() << ";";
  }
  wideExtensions = wss.str();
  filters.pszSpec = wideExtensions.c_str();

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
  int length = WideCharToMultiByte(
      CP_UTF8, 0, &ws.at(0), static_cast<int>(ws.size()), NULL, 0, NULL, NULL);

  liquid::String filePath(length, 0);
  WideCharToMultiByte(CP_UTF8, 0, &ws.at(0), static_cast<int>(ws.size()),
                      &filePath.at(0), length, NULL, NULL);

  CoTaskMemFree(pszFilePath);
  pItem->Release();
  pFileOpen->Release();
  CoUninitialize();
  return filePath;
}

} // namespace liquid::platform_tools
