#include "liquid/core/Base.h"
#include <json/json.hpp>
#include "ProjectManager.h"

#include "platform-tools/NativeFileDialog.h"

namespace liquidator {

bool ProjectManager::createProjectInPath() {
  using json = nlohmann::json;

  liquid::platform_tools::NativeFileDialog dialog;

  auto projectPath = dialog.getFilePathFromCreateDialog({});

  if (projectPath.empty()) {
    return false;
  }

  auto assetsPath = projectPath / "assets";
  auto settingsPath = projectPath / "settings";

  std::filesystem::create_directory(projectPath);
  std::filesystem::create_directory(assetsPath);
  std::filesystem::create_directory(settingsPath);

  mProject.name = projectPath.stem().filename().string();
  mProject.version = "0.0.1";
  mProject.assetsPath = assetsPath;
  mProject.settingsPath = settingsPath;

  json projectObj;
  projectObj["name"] = mProject.name;
  projectObj["version"] = mProject.version;
  projectObj["paths"]["assets"] =
      std::filesystem::relative(mProject.assetsPath, projectPath).string();
  projectObj["paths"]["settings"] =
      std::filesystem::relative(mProject.settingsPath, projectPath).string();

  auto projectFile = projectPath / (mProject.name + ".lqproj");

  std::ofstream out(projectFile, std::ios::out);
  out << projectObj;
  out.close();

  return true;
}

bool ProjectManager::openProjectInPath() {
  using json = nlohmann::json;

  liquid::platform_tools::NativeFileDialog dialog;

  auto projectFilePath = dialog.getFilePathFromDialog({"lqproj"});
  if (projectFilePath.empty()) {
    return false;
  }

  auto directory = std::filesystem::path(projectFilePath).parent_path();

  json projectObj;

  std::ifstream in(projectFilePath, std::ios::in);
  in >> projectObj;
  in.close();

  mProject.name = projectObj["name"];
  mProject.version = projectObj["version"];
  mProject.assetsPath =
      directory / liquid::String(projectObj["paths"]["assets"]);
  mProject.settingsPath =
      directory / liquid::String(projectObj["paths"]["settings"]);

  return true;
}

} // namespace liquidator
