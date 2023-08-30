#include "liquid/core/Base.h"
#include "liquid/yaml/Yaml.h"
#include "liquid/platform/tools/FileDialog.h"

#include "ProjectManager.h"

namespace quoll::editor {

bool ProjectManager::createProjectInPath() {
  auto projectPath = platform::FileDialog::getFilePathFromCreateDialog(
      {{"Liquid project", {"liquid"}}});

  if (projectPath.empty()) {
    return false;
  }

  mProject.name = projectPath.stem().filename().string();
  mProject.version = "0.0.1";
  mProject.assetsPath = projectPath / "assets";
  mProject.assetsCachePath = projectPath / "cache";
  mProject.settingsPath = projectPath / "settings";

  std::filesystem::create_directory(projectPath);
  std::filesystem::create_directory(mProject.assetsPath);
  std::filesystem::create_directory(mProject.assetsCachePath);
  std::filesystem::create_directory(mProject.settingsPath);

  {
    YAML::Node sceneObj;
    sceneObj["name"] = "MainScene";
    sceneObj["version"] = "0.1";
    sceneObj["type"] = "scene";

    YAML::Node mainZone;
    mainZone["name"] = "MainZone";
    sceneObj["zones"][0] = mainZone;
    sceneObj["entities"] = YAML::Node(YAML::NodeType::Sequence);

    std::ofstream stream(mProject.assetsPath / "main.scene");
    stream << sceneObj;
    stream.close();
  }

  {
    YAML::Node projectObj;
    projectObj["name"] = mProject.name;
    projectObj["version"] = mProject.version;
    projectObj["paths"]["assets"] =
        std::filesystem::relative(mProject.assetsPath, projectPath).string();
    projectObj["paths"]["assetsCache"] =
        std::filesystem::relative(mProject.assetsCachePath, projectPath)
            .string();
    projectObj["paths"]["settings"] =
        std::filesystem::relative(mProject.settingsPath, projectPath).string();

    auto projectFile = projectPath / (mProject.name + ".liquid");

    std::ofstream stream(projectFile, std::ios::out);
    stream << projectObj;
    stream.close();
  }

  return true;
}

bool ProjectManager::openProjectInPath() {
  auto projectFilePath = platform::FileDialog::getFilePathFromDialog(
      {{"Liquid project", {"liquid"}}});
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

  mProject.name = projectObj["name"].as<String>();
  mProject.version = projectObj["version"].as<String>();
  mProject.assetsPath =
      directory / String(projectObj["paths"]["assets"].as<String>());
  mProject.assetsCachePath =
      directory / String(projectObj["paths"]["assetsCache"].as<String>());
  mProject.settingsPath =
      directory / String(projectObj["paths"]["settings"].as<String>());

  return true;
}

} // namespace quoll::editor
