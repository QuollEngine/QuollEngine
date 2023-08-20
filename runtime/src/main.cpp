#include "liquid/core/Base.h"
#include "liquid/core/Version.h"
#include "liquid/core/Engine.h"
#include "liquid/yaml/Yaml.h"

#include "runtime/Runtime.h"

int main() {
  auto gamePath = std::filesystem::current_path();

  liquid::Engine::setPath(gamePath / "engine");

  auto launchPath = gamePath / "launch.yml";

  std::ifstream stream(launchPath);

  if (stream.bad()) {
    std::cerr << "Failed to load the launch" << std::endl;

    return 1;
  }

  auto node = YAML::Load(stream);

  liquid::runtime::LaunchConfig launchConfig{};

  launchConfig.name = node["name"].as<liquid::String>();
  launchConfig.startingScene = node["startingScene"].as<liquid::String>();

  liquid::runtime::Runtime runtime(launchConfig);

  runtime.start();

  return 0;
}
