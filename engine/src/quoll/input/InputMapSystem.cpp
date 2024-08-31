#include "quoll/core/Base.h"
#include "quoll/asset/AssetRegistry.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/system/SystemView.h"
#include "InputDeviceManager.h"
#include "InputMapSystem.h"

namespace quoll {

InputMapSystem::InputMapSystem(InputDeviceManager &deviceManager)
    : mDeviceManager(deviceManager) {}

void InputMapSystem::update(SystemView &view) {
  auto &entityDatabase = view.scene->entityDatabase;
  for (auto [entity, ref] : entityDatabase.view<InputMapAssetRef>()) {
    if (ref.handle && !entityDatabase.has<InputMap>(entity)) {
      entityDatabase.set(entity,
                         createInputMap(ref.handle.get(), ref.defaultScheme));
    }
  }

  std::vector<Entity> componentsToDelete(0);
  for (auto [entity, ref] : entityDatabase.view<InputMap>()) {
    if (!entityDatabase.has<InputMapAssetRef>(entity)) {
      componentsToDelete.push_back(entity);
    }
  }

  for (auto entity : componentsToDelete) {
    entityDatabase.remove<InputMap>(entity);
  }

  for (auto [_, inputMap] : entityDatabase.view<InputMap>()) {
    for (usize i = 0; i < inputMap.commandValues.size(); ++i) {
      auto type = inputMap.commandDataTypes.at(i);
      if (type == InputDataType::Boolean) {
        inputMap.commandValues.at(i) = false;
      } else if (type == InputDataType::Axis2d) {
        inputMap.commandValues.at(i) = glm::vec2{0.0f, 0.0f};
      }
    }
  }

  for (auto [entity, inputMap] : entityDatabase.view<InputMap>()) {
    auto &scheme = inputMap.schemes.at(inputMap.activeScheme);

    for (auto &device : mDeviceManager.getDevices()) {
      for (const auto &[key, command] : scheme.inputKeyToCommandMap) {
        auto variant = device.stateFn(key);

        auto dataType = inputMap.commandDataTypes.at(command);
        if (dataType == InputDataType::Boolean) {
          auto &commandValue =
              std::get<bool>(inputMap.commandValues.at(command));

          if (auto *inputValue = std::get_if<bool>(&variant)) {
            commandValue |= *inputValue;
          } else if (auto *inputValue = std::get_if<f32>(&variant)) {
            commandValue |= (*inputValue != 0.0f);
          } else if (auto *inputValue = std::get_if<glm::vec2>(&variant)) {
            commandValue |= (inputValue->x != 0.0f || inputValue->y != 0.0f);
          }
        } else if (dataType == InputDataType::Axis2d) {
          auto &commandValue =
              std::get<glm::vec2>(inputMap.commandValues.at(command));

          if (std::get_if<bool>(&variant) != nullptr &&
              std::get<bool>(variant)) {
            auto field = scheme.inputKeyFields.at(key);
            if (field == InputDataTypeField::X0) {
              commandValue.x += -1.0f;
            } else if (field == InputDataTypeField::X1) {
              commandValue.x += 1.0f;
            } else if (field == InputDataTypeField::Y0) {
              commandValue.y += -1.0f;
            } else if (field == InputDataTypeField::Y1) {
              commandValue.y += 1.0f;
            }

          } else if (auto *inputValue = std::get_if<f32>(&variant)) {
            auto field = scheme.inputKeyFields.at(key);
            if (field == InputDataTypeField::X) {
              commandValue.x += *inputValue;
            } else if (field == InputDataTypeField::Y) {
              commandValue.y += *inputValue;
            }
          } else if (auto *inputValue = std::get_if<glm::vec2>(&variant)) {
            commandValue += *inputValue;
          }

          if (commandValue.x < 0.0f) {
            commandValue.x = std::max(-1.0f, commandValue.x);
          }

          if (commandValue.x > 0.0f) {
            commandValue.x = std::min(1.0f, commandValue.x);
          }

          if (commandValue.y < 0.0f) {
            commandValue.y = std::max(-1.0f, commandValue.y);
          }

          if (commandValue.y > 0.0f) {
            commandValue.y = std::min(1.0f, commandValue.y);
          }
        }
      }
    }
  }
}

InputMap InputMapSystem::createInputMap(InputMapAsset &asset,
                                        usize defaultScheme) {
  InputMap inputMap{};

  inputMap.activeScheme = std::min(defaultScheme, asset.schemes.size() - 1);

  inputMap.commandDataTypes.reserve(asset.commands.size());
  inputMap.commandValues.resize(asset.commands.size());
  inputMap.schemes.resize(asset.schemes.size());

  usize schemeIndex = 0;
  for (const auto &scheme : asset.schemes) {
    inputMap.schemeNameMap.insert_or_assign(scheme.name, schemeIndex++);
  }

  usize commandIndex = 0;
  for (const auto &command : asset.commands) {
    inputMap.commandNameMap.insert_or_assign(command.name, commandIndex++);
    inputMap.commandDataTypes.push_back(command.type);
  }

  for (const auto &binding : asset.bindings) {
    auto &scheme = inputMap.schemes.at(binding.scheme);

    if (const auto *value = std::get_if<InputMapValue>(&binding.value)) {
      if (*value >= 0) {
        scheme.inputKeyToCommandMap.insert_or_assign(*value, binding.command);
        scheme.inputKeyFields.insert_or_assign(*value,
                                               InputDataTypeField::Value);
      }
    } else if (const auto *axis2d =
                   std::get_if<InputMapAxis2dValue>(&binding.value)) {
      if (const auto *value = std::get_if<InputMapValue>(&axis2d->x)) {
        if (*value >= 0) {
          scheme.inputKeyToCommandMap.insert_or_assign(*value, binding.command);
          scheme.inputKeyFields.insert_or_assign(*value, InputDataTypeField::X);
        }
      } else if (const auto *value =
                     std::get_if<InputMapAxisSegment>(&axis2d->x)) {
        if (value->at(0) >= 0) {
          scheme.inputKeyToCommandMap.insert_or_assign(value->at(0),
                                                       binding.command);
          scheme.inputKeyFields.insert_or_assign(value->at(0),
                                                 InputDataTypeField::X0);
        }

        if (value->at(1) >= 0) {
          scheme.inputKeyToCommandMap.insert_or_assign(value->at(1),
                                                       binding.command);
          scheme.inputKeyFields.insert_or_assign(value->at(1),
                                                 InputDataTypeField::X1);
        }
      }

      if (const auto *value = std::get_if<InputMapValue>(&axis2d->y)) {
        if (*value >= 0) {
          scheme.inputKeyToCommandMap.insert_or_assign(*value, binding.command);
          scheme.inputKeyFields.insert_or_assign(*value, InputDataTypeField::Y);
        }
      } else if (const auto *value =
                     std::get_if<InputMapAxisSegment>(&axis2d->y)) {
        if (value->at(0) >= 0) {
          scheme.inputKeyToCommandMap.insert_or_assign(value->at(0),
                                                       binding.command);
          scheme.inputKeyFields.insert_or_assign(value->at(0),
                                                 InputDataTypeField::Y0);
        }

        if (value->at(1) >= 0) {
          scheme.inputKeyToCommandMap.insert_or_assign(value->at(1),
                                                       binding.command);
          scheme.inputKeyFields.insert_or_assign(value->at(1),
                                                 InputDataTypeField::Y1);
        }
      }
    }
  }

  return inputMap;
}

} // namespace quoll
