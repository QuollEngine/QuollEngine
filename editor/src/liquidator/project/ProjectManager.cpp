#include "liquid/core/Base.h"
#include "liquid/yaml/Yaml.h"
#include "liquid/platform-tools/NativeFileDialog.h"

#include "ProjectManager.h"

namespace liquidator {

bool ProjectManager::createProjectInPath() {
  liquid::platform_tools::NativeFileDialog dialog;

  auto projectPath =
      dialog.getFilePathFromCreateDialog({{"Liquid project", {"lqproj"}}});

  if (projectPath.empty()) {
    return false;
  }

  mProject.name = projectPath.stem().filename().string();
  mProject.version = "0.0.1";
  mProject.assetsPath = projectPath / "assets";
  mProject.settingsPath = projectPath / "settings";
  mProject.scenesPath = projectPath / "scenes";

  std::filesystem::create_directory(projectPath);
  std::filesystem::create_directory(mProject.assetsPath);
  std::filesystem::create_directory(mProject.settingsPath);
  std::filesystem::create_directory(mProject.scenesPath);
  std::filesystem::create_directory(mProject.scenesPath / "entities");

  {
    YAML::Node sceneObj;
    sceneObj["name"] = "MainScene";

    YAML::Node mainZone;
    mainZone["name"] = "MainZone";
    mainZone["entities"] = "./entities";
    sceneObj["zones"][0] = mainZone;
    sceneObj["persistentZone"] = 0;

    std::ofstream stream(mProject.scenesPath / "main.lqscene");
    stream << sceneObj;
    stream.close();
  }

  {
    YAML::Node projectObj;
    projectObj["name"] = mProject.name;
    projectObj["version"] = mProject.version;
    projectObj["paths"]["assets"] =
        std::filesystem::relative(mProject.assetsPath, projectPath).string();
    projectObj["paths"]["settings"] =
        std::filesystem::relative(mProject.settingsPath, projectPath).string();
    projectObj["paths"]["scenes"] =
        std::filesystem::relative(mProject.scenesPath, projectPath).string();

    auto projectFile = projectPath / (mProject.name + ".lqproj");

    std::ofstream stream(projectFile, std::ios::out);
    stream << projectObj;
    stream.close();
  }

  return true;
}

bool ProjectManager::openProjectInPath() {
  liquid::platform_tools::NativeFileDialog dialog;

  auto projectFilePath =
      dialog.getFilePathFromDialog({{"Liquid project", {"lqproj"}}});
  if (projectFilePath.empty()) {
    return false;
  }

  auto directory = std::filesystem::path(projectFilePath).parent_path();

  YAML::Node projectObj;

  std::ifstream in(projectFilePath, std::ios::in);

  try {
    projectObj = YAML::Load(in);
    in.close();
  } catch (std::exception &) {
    in.close();
    return false;
  }

  mProject.name = projectObj["name"].as<liquid::String>();
  mProject.version = projectObj["version"].as<liquid::String>();
  mProject.assetsPath =
      directory /
      liquid::String(projectObj["paths"]["assets"].as<liquid::String>());
  mProject.settingsPath =
      directory /
      liquid::String(projectObj["paths"]["settings"].as<liquid::String>());
  mProject.scenesPath =
      directory /
      liquid::String(projectObj["paths"]["scenes"].as<liquid::String>());

  return true;
}

} // namespace liquidator
