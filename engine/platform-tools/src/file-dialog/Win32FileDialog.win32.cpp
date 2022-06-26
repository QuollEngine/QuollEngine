#include "liquid/core/Base.h"
#include "NativeFileDialog.h"
#include <windows.h>
#include <shobjidl.h>

namespace liquid::platform_tools {

/**
 * @brief Win32 file type entry
 */
struct Win32Entry {
  /**
   * @brief Entry label
   */
  std::wstring label;

  /**
   * @brief Entry extensions
   */
  std::wstring extensions;
};

static void checkWin32Error(HRESULT res, const liquid::String &message) {
  LIQUID_ASSERT(SUCCEEDED(res), "Failed to open file dialog " + message);
}

liquid::Path NativeFileDialog::getFilePathFromDialog(
    const std::vector<FileTypeEntry> &fileTypes) {
  IFileDialog *pFileOpen = nullptr;
  checkWin32Error(CoCreateInstance(CLSID_FileOpenDialog, nullptr,
                                   CLSCTX_INPROC_SERVER,
                                   IID_PPV_ARGS(&pFileOpen)),
                  "Cannot create file dialog instance");

  std::vector<COMDLG_FILTERSPEC> filters;
  filters.reserve(fileTypes.size() + 1);

  std::vector<Win32Entry> win32Entries;
  win32Entries.reserve(fileTypes.size());

  for (auto &entry : fileTypes) {
    Win32Entry win32Entry{};

    std::wstringstream wssLabel;
    std::wstringstream wssExt;

    for (auto &ext : entry.extensions) {
      wssExt << L"*." << ext.c_str() << ";";
    }

    win32Entry.extensions = wssExt.str();
    win32Entry.extensions.pop_back();

    wssLabel << String(entry.label).c_str();
    win32Entry.label = wssLabel.str();

    win32Entries.push_back(win32Entry);
  }

  for (auto &entry : win32Entries) {
    COMDLG_FILTERSPEC filter{};

    filter.pszName = entry.label.c_str();
    filter.pszSpec = entry.extensions.c_str();

    filters.push_back(filter);
  }

  pFileOpen->SetFileTypes(static_cast<uint32_t>(filters.size()),
                          filters.data());

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

liquid::Path NativeFileDialog::getFilePathFromCreateDialog(
    const std::vector<FileTypeEntry> &fileTypes) {
  IFileDialog *pFileOpen = nullptr;
  checkWin32Error(CoCreateInstance(CLSID_FileSaveDialog, nullptr,
                                   CLSCTX_INPROC_SERVER,
                                   IID_PPV_ARGS(&pFileOpen)),
                  "Cannot create save as file dialog instance");

  std::vector<COMDLG_FILTERSPEC> filters;
  filters.reserve(fileTypes.size() + 1);

  std::vector<Win32Entry> win32Entries;
  win32Entries.reserve(fileTypes.size());

  for (auto &entry : fileTypes) {
    Win32Entry win32Entry{};

    std::wstringstream wssLabel;
    std::wstringstream wssExt;

    for (auto &ext : entry.extensions) {
      wssExt << L"*." << ext.c_str() << ";";
    }

    win32Entry.extensions = wssExt.str();
    win32Entry.extensions.pop_back();

    wssLabel << String(entry.label).c_str();
    win32Entry.label = wssLabel.str();

    win32Entries.push_back(win32Entry);
  }

  for (auto &entry : win32Entries) {
    COMDLG_FILTERSPEC filter{};

    filter.pszName = entry.label.c_str();
    filter.pszSpec = entry.extensions.c_str();

    filters.push_back(filter);
  }

  pFileOpen->SetFileTypes(static_cast<uint32_t>(filters.size()),
                          filters.data());

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
