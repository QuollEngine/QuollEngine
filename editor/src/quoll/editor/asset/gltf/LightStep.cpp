#include "quoll/core/Base.h"

#include "LightStep.h"

namespace quoll::editor {

void loadLights(GLTFImportData &importData) {
  for (auto i = 0; i < importData.model.lights.size(); ++i) {
    const auto &light = importData.model.lights.at(i);

    if (light.type == "spot") {
      importData.warnings.push_back(
          "Light #" + std::to_string(i) +
          " is a spot light and is not supported at the moment");
      continue;
    }

    if (light.type == "directional") {
      DirectionalLight component{};
      for (auto i = 0;
           i < static_cast<glm::vec4::length_type>(light.color.size()); ++i) {
        component.color[i] = static_cast<float>(light.color.at(i));
      }

      component.intensity = static_cast<float>(light.intensity);

      importData.directionalLights.map.insert_or_assign(i, component);
    } else if (light.type == "point") {
      PointLight component{};
      for (auto i = 0; i < glm::vec4::length_type(light.color.size()); ++i) {
        component.color[i] = static_cast<float>(light.color.at(i));
      }

      component.intensity = static_cast<float>(light.intensity);
      component.range = static_cast<float>(light.range);

      importData.pointLights.map.insert_or_assign(i, component);
    }
  }
}

} // namespace quoll::editor
