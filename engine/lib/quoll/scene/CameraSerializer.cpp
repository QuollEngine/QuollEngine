#include "quoll/core/Base.h"
#include "AutoAspectRatio.h"
#include "Camera.h"
#include "CameraSerializer.h"
#include "PerspectiveLens.h"

namespace quoll {

void CameraSerializer::serialize(YAML::Node &node,
                                 EntityDatabase &entityDatabase,
                                 Entity entity) {
  if (entityDatabase.has<PerspectiveLens>(entity)) {
    const auto &camera = entityDatabase.get<PerspectiveLens>(entity);

    node["camera"]["type"] = 0;
    node["camera"]["near"] = camera.near;
    node["camera"]["far"] = camera.far;
    node["camera"]["aperture"] = camera.aperture;
    node["camera"]["sensorSize"] = camera.sensorSize;
    node["camera"]["focalLength"] = camera.focalLength;
    node["camera"]["shutterSpeed"] = camera.shutterSpeed;
    node["camera"]["sensitivity"] = camera.sensitivity;

    if (entityDatabase.has<AutoAspectRatio>(entity)) {
      node["camera"]["aspectRatio"] = "auto";
    } else {
      node["camera"]["aspectRatio"] = camera.aspectRatio;
    }
  }
}

void CameraSerializer::deserialize(const YAML::Node &node,
                                   EntityDatabase &entityDatabase,
                                   Entity entity) {
  if (node["camera"] && node["camera"].IsMap()) {
    PerspectiveLens lens{};
    const f32 near = node["camera"]["near"].as<f32>(lens.near);
    if (near >= 0.0f) {
      lens.near = near;
    }

    const f32 far = node["camera"]["far"].as<f32>(lens.far);
    if (far >= 0.0f) {
      lens.far = far;
    }

    const glm::vec2 sensorSize =
        node["camera"]["sensorSize"].as<glm::vec2>(lens.sensorSize);

    if (sensorSize.x >= 0.0f && sensorSize.y >= 0.0f) {
      lens.sensorSize = sensorSize;
    }

    const f32 focalLength =
        node["camera"]["focalLength"].as<f32>(lens.focalLength);
    if (focalLength >= 0.0f) {
      lens.focalLength = focalLength;
    }

    const f32 aperture = node["camera"]["aperture"].as<f32>(lens.aperture);
    if (aperture >= 0.0f) {
      lens.aperture = aperture;
    }

    const f32 shutterSpeed =
        node["camera"]["shutterSpeed"].as<f32>(lens.shutterSpeed);
    if (shutterSpeed >= 0.0f) {
      lens.shutterSpeed = shutterSpeed;
    }

    lens.sensitivity = node["camera"]["sensitivity"].as<u32>(lens.sensitivity);

    bool autoRatio = true;
    if (node["camera"]["aspectRatio"] &&
        node["camera"]["aspectRatio"].IsScalar()) {
      auto res = node["camera"]["aspectRatio"].as<String>("");
      if (res.empty()) {
        res = "auto";
      }
      autoRatio = res == "auto";
    }

    if (autoRatio) {
      entityDatabase.set<AutoAspectRatio>(entity, {});
    } else {
      const f32 aspectRatio =
          node["camera"]["aspectRatio"].as<f32>(lens.aspectRatio);
      if (aspectRatio >= 0.0f) {
        lens.aspectRatio = aspectRatio;
      }
    }

    entityDatabase.set<Camera>(entity, {});
    entityDatabase.set(entity, lens);
  }
}

} // namespace quoll
