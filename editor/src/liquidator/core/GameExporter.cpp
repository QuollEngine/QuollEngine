#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"
#include "liquid/yaml/Yaml.h"

#include "GameExporter.h"

namespace liquidator {

void GameExporter::exportGame(const Project &project,
                              const liquid::Path &destination) {
  using co = std::filesystem::copy_options;

  auto gameName = destination.filename();

  // Copy game data to destination
  std::filesystem::create_directory(destination);
  std::filesystem::copy(project.assetsPath,
                        destination / project.assetsPath.filename(),
                        co::overwrite_existing | co::recursive);
  std::filesystem::copy(project.scenePath,
                        destination / project.scenePath.filename(),
                        co::overwrite_existing | co::recursive);

  // Copy engine data
  auto enginePath = liquid::Engine::getEnginePath();
  std::filesystem::copy(enginePath, destination / enginePath.filename(),
                        co::overwrite_existing | co::recursive);

  // Copy runtime
  liquid::Path runtimePath;

  for (auto entry :
       std::filesystem::directory_iterator(std::filesystem::current_path())) {
    if (entry.path().stem().string() == "Runtime") {
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
  node["paths"]["scenes"] = project.scenePath.filename().string();

  std::ofstream stream(destination / "launch.yml", std::ios::out);
  stream << node;
  stream.close();
}

} // namespace liquidator
