#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"
#include "liquid/yaml/Yaml.h"

#include "GameExporter.h"

namespace liquid::editor {

void GameExporter::exportGame(const Project &project, const Path &destination) {
  using co = std::filesystem::copy_options;

  auto gameName = destination.filename();

  if (gameName.empty()) {
    return;
  }

  auto destinationAssetsPath = destination / project.assetsPath.filename();

  // Copy game data to destination
  std::filesystem::create_directory(destination);
  std::filesystem::copy(project.assetsCachePath, destinationAssetsPath,
                        co::overwrite_existing | co::recursive);
  std::filesystem::copy(project.scenesPath,
                        destination / project.scenesPath.filename(),
                        co::overwrite_existing | co::recursive);

  for (auto &entry :
       std::filesystem::recursive_directory_iterator(destinationAssetsPath)) {
    if (entry.path().extension() == ".lqhash") {
      std::filesystem::remove(entry.path());
    }
  }

  // Copy engine data
  auto enginePath = Engine::getEnginePath();
  std::filesystem::copy(enginePath, destination / enginePath.filename(),
                        co::overwrite_existing | co::recursive);

  // Copy runtime
  Path runtimePath;

  for (auto entry :
       std::filesystem::directory_iterator(std::filesystem::current_path())) {
    if (entry.path().stem().string() == "LiquidRuntime") {
      runtimePath = entry.path();
    }
  }

  auto gameExecutable = destination / gameName;
  gameExecutable.replace_extension(runtimePath.extension());
  std::filesystem::copy(runtimePath, gameExecutable);

  // Create launch file
  YAML::Node node;
  node["name"] = gameName.string();
  node["paths"]["assets"] = project.assetsPath.filename().string();
  node["paths"]["scenes"] = project.scenesPath.filename().string();

  std::ofstream stream(destination / "launch.yml", std::ios::out);
  stream << node;
  stream.close();

  Engine::getLogger().info() << "Game exported to " << destination;
}

} // namespace liquid::editor
