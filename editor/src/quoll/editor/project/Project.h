#pragma once

namespace quoll::editor {

struct Project {
  String name;

  String version;

  Path assetsPath;

  Path assetsCachePath;

  Path settingsPath;

  Uuid startingScene;
};

} // namespace quoll::editor
