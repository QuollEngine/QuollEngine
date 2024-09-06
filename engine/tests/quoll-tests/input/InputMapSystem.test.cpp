#include "quoll/core/Base.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/input/InputDeviceManager.h"
#include "quoll/input/InputMapSystem.h"
#include "quoll/input/KeyMappings.h"
#include "quoll/system/SystemView.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/AssetCacheUtils.h"
#include <GLFW/glfw3.h>

class InputMapSystemTest : public ::testing::Test {
public:
  InputMapSystemTest() : assetCache("/") {
    deviceManager.addDevice({.type = quoll::InputDeviceType::Keyboard,
                             .name = "Test device",
                             .index = 0,
                             .stateFn = [this](int key) {
                               return inputStates.contains(key)
                                          ? inputStates.at(key)
                                          : false;
                             }});
  }

  void setInputState(int key, bool value) {
    inputStates.insert_or_assign(key, value);
  }

  void setInputState(int key, f32 value) {
    inputStates.insert_or_assign(key, value);
  }

  void setInputState(int key, glm::vec2 value) {
    inputStates.insert_or_assign(key, value);
  }

  std::unordered_map<int, quoll::InputStateValue> inputStates;

  quoll::Entity createInputMap() {
    quoll::InputMapAsset data{};
    data.schemes.push_back({"Explore"});
    data.schemes.push_back({"Combat"});

    data.commands.push_back({"Jump", quoll::InputDataType::Boolean});
    data.commands.push_back({"Move", quoll::InputDataType::Axis2d});
    data.commands.push_back({"Look", quoll::InputDataType::Axis2d});
    data.commands.push_back({"Aim", quoll::InputDataType::Axis2d});
    data.commands.push_back({"Shoot", quoll::InputDataType::Boolean});
    data.commands.push_back({"Pick", quoll::InputDataType::Boolean});

    // Null data for testing purposes
    data.bindings.push_back({0, 2, quoll::InputMapAxis2dValue{-1, -1}});

    // Jump
    data.bindings.push_back({0, 0, quoll::input::get("GAMEPAD_SOUTH")});
    data.bindings.push_back({0, 0, quoll::input::get("KEY_SPACE")});

    // Move
    data.bindings.push_back(
        {0, 1,
         quoll::InputMapAxis2dValue{quoll::input::get("GAMEPAD_LEFT_X"),
                                    quoll::input::get("GAMEPAD_LEFT_Y")}});
    data.bindings.push_back(
        {0, 1,
         quoll::InputMapAxis2dValue{
             quoll::InputMapAxisSegment{quoll::input::get("KEY_A"),
                                        quoll::input::get("KEY_D")},
             quoll::InputMapAxisSegment{quoll::input::get("KEY_W"),
                                        quoll::input::get("KEY_S")}}});

    // Look
    data.bindings.push_back({0, 2, quoll::input::get("MOUSE_MOVE")});

    // Aim
    data.bindings.push_back(
        {1, 3,
         quoll::InputMapAxis2dValue{quoll::input::get("GAMEPAD_LEFT_X"),
                                    quoll::input::get("GAMEPAD_LEFT_Y")}});

    data.bindings.push_back(
        {1, 3,
         quoll::InputMapAxis2dValue{
             quoll::InputMapAxisSegment{quoll::input::get("KEY_A"),
                                        quoll::input::get("KEY_D")},
             quoll::InputMapAxisSegment{quoll::input::get("KEY_W"),
                                        quoll::input::get("KEY_S")}}});

    // Shoot
    data.bindings.push_back({1, 4, quoll::input::get("MOUSE_LEFT")});
    data.bindings.push_back({1, 4, quoll::input::get("GAMEPAD_BUMPER_RIGHT")});

    // Shoot
    data.bindings.push_back({0, 5, quoll::input::get("MOUSE_LEFT")});
    data.bindings.push_back({0, 5, quoll::input::get("GAMEPAD_BUMPER_RIGHT")});

    auto asset = createAssetInCache(assetCache, data);

    auto entity = db.create();
    db.set<quoll::InputMapAssetRef>(entity, {asset});

    return entity;
  }

  quoll::AssetCache assetCache;
  quoll::InputDeviceManager deviceManager;
  quoll::InputMapSystem inputMapSystem{deviceManager};
  quoll::Scene scene;
  quoll::EntityDatabase &db = scene.entityDatabase;
  quoll::SystemView view{&scene};
};

TEST_F(InputMapSystemTest,
       CreateInputMapComponentFromAssetRefIfInputMapComponentDoesNotExist) {
  auto entity = createInputMap();

  inputMapSystem.update(view);

  EXPECT_TRUE(db.has<quoll::InputMap>(entity));
  auto inputMap = db.get<quoll::InputMap>(entity);

  EXPECT_EQ(inputMap.schemes.size(), 2);
  EXPECT_EQ(inputMap.schemeNameMap.size(), 2);
  EXPECT_EQ(inputMap.schemeNameMap.at("Explore"), 0);
  EXPECT_EQ(inputMap.schemeNameMap.at("Combat"), 1);

  EXPECT_EQ(inputMap.commandNameMap.size(), 6);
  EXPECT_EQ(inputMap.commandNameMap.at("Jump"), 0);
  EXPECT_EQ(inputMap.commandNameMap.at("Move"), 1);
  EXPECT_EQ(inputMap.commandNameMap.at("Look"), 2);
  EXPECT_EQ(inputMap.commandNameMap.at("Aim"), 3);
  EXPECT_EQ(inputMap.commandNameMap.at("Shoot"), 4);
  EXPECT_EQ(inputMap.commandNameMap.at("Pick"), 5);

  EXPECT_EQ(inputMap.commandDataTypes.size(), 6);
  EXPECT_EQ(inputMap.commandDataTypes.at(0), quoll::InputDataType::Boolean);
  EXPECT_EQ(inputMap.commandDataTypes.at(1), quoll::InputDataType::Axis2d);
  EXPECT_EQ(inputMap.commandDataTypes.at(2), quoll::InputDataType::Axis2d);
  EXPECT_EQ(inputMap.commandDataTypes.at(3), quoll::InputDataType::Axis2d);
  EXPECT_EQ(inputMap.commandDataTypes.at(4), quoll::InputDataType::Boolean);
  EXPECT_EQ(inputMap.commandDataTypes.at(5), quoll::InputDataType::Boolean);

  // Explore
  {
    auto &layout = inputMap.schemes.at(0);

    auto &bindings = layout.inputKeyToCommandMap;
    EXPECT_EQ(bindings.size(), 11);

    EXPECT_EQ(bindings.at(quoll::input::get("GAMEPAD_SOUTH")), 0);
    EXPECT_EQ(bindings.at(quoll::input::get("KEY_SPACE")), 0);

    EXPECT_EQ(bindings.at(quoll::input::get("GAMEPAD_LEFT_X")), 1);
    EXPECT_EQ(bindings.at(quoll::input::get("GAMEPAD_LEFT_Y")), 1);
    EXPECT_EQ(bindings.at(quoll::input::get("KEY_A")), 1);
    EXPECT_EQ(bindings.at(quoll::input::get("KEY_D")), 1);
    EXPECT_EQ(bindings.at(quoll::input::get("KEY_W")), 1);
    EXPECT_EQ(bindings.at(quoll::input::get("KEY_S")), 1);

    EXPECT_EQ(bindings.at(quoll::input::get("MOUSE_MOVE")), 2);

    EXPECT_EQ(bindings.at(quoll::input::get("MOUSE_LEFT")), 5);
    EXPECT_EQ(bindings.at(quoll::input::get("GAMEPAD_BUMPER_RIGHT")), 5);

    auto &keyFields = layout.inputKeyFields;
    EXPECT_EQ(keyFields.at(quoll::input::get("GAMEPAD_LEFT_X")),
              quoll::InputDataTypeField::X);
    EXPECT_EQ(keyFields.at(quoll::input::get("GAMEPAD_LEFT_Y")),
              quoll::InputDataTypeField::Y);
    EXPECT_EQ(keyFields.at(quoll::input::get("KEY_A")),
              quoll::InputDataTypeField::X0);
    EXPECT_EQ(keyFields.at(quoll::input::get("KEY_D")),
              quoll::InputDataTypeField::X1);
    EXPECT_EQ(keyFields.at(quoll::input::get("KEY_W")),
              quoll::InputDataTypeField::Y0);
    EXPECT_EQ(keyFields.at(quoll::input::get("KEY_S")),
              quoll::InputDataTypeField::Y1);
  }

  // Combat
  {
    auto &layout = inputMap.schemes.at(1);

    auto &bindings = layout.inputKeyToCommandMap;
    EXPECT_EQ(bindings.size(), 8);

    EXPECT_EQ(bindings.at(quoll::input::get("GAMEPAD_LEFT_X")), 3);
    EXPECT_EQ(bindings.at(quoll::input::get("GAMEPAD_LEFT_Y")), 3);
    EXPECT_EQ(bindings.at(quoll::input::get("KEY_A")), 3);
    EXPECT_EQ(bindings.at(quoll::input::get("KEY_D")), 3);
    EXPECT_EQ(bindings.at(quoll::input::get("KEY_W")), 3);
    EXPECT_EQ(bindings.at(quoll::input::get("KEY_S")), 3);

    EXPECT_EQ(bindings.at(quoll::input::get("MOUSE_LEFT")), 4);
    EXPECT_EQ(bindings.at(quoll::input::get("GAMEPAD_BUMPER_RIGHT")), 4);

    auto &keyFields = layout.inputKeyFields;
    EXPECT_EQ(keyFields.at(quoll::input::get("GAMEPAD_LEFT_X")),
              quoll::InputDataTypeField::X);
    EXPECT_EQ(keyFields.at(quoll::input::get("GAMEPAD_LEFT_Y")),
              quoll::InputDataTypeField::Y);
    EXPECT_EQ(keyFields.at(quoll::input::get("KEY_A")),
              quoll::InputDataTypeField::X0);
    EXPECT_EQ(keyFields.at(quoll::input::get("KEY_D")),
              quoll::InputDataTypeField::X1);
    EXPECT_EQ(keyFields.at(quoll::input::get("KEY_W")),
              quoll::InputDataTypeField::Y0);
    EXPECT_EQ(keyFields.at(quoll::input::get("KEY_S")),
              quoll::InputDataTypeField::Y1);
  }
}

TEST_F(InputMapSystemTest, DeleteInputMapComponentIfAssetRefIsDeleted) {
  auto entity = db.create();
  db.set<quoll::InputMap>(entity, {});

  inputMapSystem.update(view);

  EXPECT_FALSE(db.has<quoll::InputMap>(entity));
}

TEST_F(InputMapSystemTest,
       ChangingActiveSchemeWritesDataToCommandsInThatScheme) {
  auto key = quoll::input::get("MOUSE_LEFT");

  auto entity = createInputMap();
  inputMapSystem.update(view);
  setInputState(key, true);

  auto &inputMap = db.get<quoll::InputMap>(entity);
  auto pick = inputMap.commandNameMap.at("Pick");
  auto shoot = inputMap.commandNameMap.at("Shoot");

  EXPECT_EQ(std::get<bool>(inputMap.commandValues.at(pick)), false);
  EXPECT_EQ(std::get<bool>(inputMap.commandValues.at(shoot)), false);

  inputMapSystem.update(view);
  EXPECT_EQ(std::get<bool>(inputMap.commandValues.at(pick)), true);
  EXPECT_EQ(std::get<bool>(inputMap.commandValues.at(shoot)), false);

  inputMap.activeScheme = 1;

  inputMapSystem.update(view);
  EXPECT_EQ(std::get<bool>(inputMap.commandValues.at(pick)), false);
  EXPECT_EQ(std::get<bool>(inputMap.commandValues.at(shoot)), true);
}

using InputMapSystemBooleanValueTest = InputMapSystemTest;

TEST_F(InputMapSystemBooleanValueTest,
       SetsValueToTrueWhenBooleanKeyStateIsTrue) {
  auto key = quoll::input::get("KEY_SPACE");
  auto entity = createInputMap();

  setInputState(key, true);
  inputMapSystem.update(view);

  auto inputMap = db.get<quoll::InputMap>(entity);
  auto &scheme = inputMap.schemes.at(0);
  auto command = scheme.inputKeyToCommandMap.at(key);

  EXPECT_EQ(std::get<bool>(inputMap.commandValues.at(command)), true);
}

TEST_F(InputMapSystemBooleanValueTest,
       SetsValueToFalseWhenBooleanKeyStateIsFalse) {
  auto key = quoll::input::get("KEY_SPACE");
  auto entity = createInputMap();

  inputMapSystem.update(view);

  auto inputMap = db.get<quoll::InputMap>(entity);
  auto &scheme = inputMap.schemes.at(0);
  auto command = scheme.inputKeyToCommandMap.at(key);

  EXPECT_EQ(std::get<bool>(inputMap.commandValues.at(command)), false);
}

TEST_F(InputMapSystemBooleanValueTest,
       SetsValueToTrueIfFloatKeyStateIsNotZero) {
  auto key = quoll::input::get("KEY_SPACE");
  auto entity = createInputMap();

  setInputState(key, -1.0f);
  inputMapSystem.update(view);

  auto inputMap = db.get<quoll::InputMap>(entity);
  auto &scheme = inputMap.schemes.at(0);
  auto command = scheme.inputKeyToCommandMap.at(key);

  EXPECT_EQ(std::get<bool>(inputMap.commandValues.at(command)), true);
}

TEST_F(InputMapSystemBooleanValueTest, SetsValueToFalseIfFloatKeyStateIsZero) {
  auto key = quoll::input::get("KEY_SPACE");
  auto entity = createInputMap();

  setInputState(key, 0.0f);
  inputMapSystem.update(view);

  auto inputMap = db.get<quoll::InputMap>(entity);
  auto &scheme = inputMap.schemes.at(0);
  auto command = scheme.inputKeyToCommandMap.at(key);

  EXPECT_EQ(std::get<bool>(inputMap.commandValues.at(command)), false);
}

TEST_F(InputMapSystemBooleanValueTest, SetsValueToTrueIfVec2StateIsNotZero) {
  auto key = quoll::input::get("KEY_SPACE");
  auto entity = createInputMap();

  setInputState(key, glm::vec2{0.0f, 0.2f});
  inputMapSystem.update(view);

  auto inputMap = db.get<quoll::InputMap>(entity);
  auto &scheme = inputMap.schemes.at(0);
  auto command = scheme.inputKeyToCommandMap.at(key);

  EXPECT_EQ(std::get<bool>(inputMap.commandValues.at(command)), true);
}

TEST_F(InputMapSystemBooleanValueTest, SetsValueToFalseIfVec2StateIsZero) {
  auto key = quoll::input::get("KEY_SPACE");
  auto entity = createInputMap();

  setInputState(key, glm::vec2{0.0f, 0.0f});
  inputMapSystem.update(view);

  auto inputMap = db.get<quoll::InputMap>(entity);
  auto &scheme = inputMap.schemes.at(0);
  auto command = scheme.inputKeyToCommandMap.at(key);

  EXPECT_EQ(std::get<bool>(inputMap.commandValues.at(command)), false);
}

using InputMapSystemAxis2dValueTest = InputMapSystemTest;

TEST_F(InputMapSystemAxis2dValueTest, SetsValueToIncomingAxis2dInput) {
  auto key = quoll::input::get("MOUSE_MOVE");
  auto entity = createInputMap();

  setInputState(key, glm::vec2{-0.4f, 0.2f});
  inputMapSystem.update(view);

  auto inputMap = db.get<quoll::InputMap>(entity);
  auto &scheme = inputMap.schemes.at(0);
  auto command = scheme.inputKeyToCommandMap.at(key);

  EXPECT_EQ(std::get<glm::vec2>(inputMap.commandValues.at(command)),
            glm::vec2(-0.4f, 0.2f));
}

TEST_F(InputMapSystemAxis2dValueTest, SetsXValueToIncomingFloatInput) {
  auto key = quoll::input::get("GAMEPAD_LEFT_X");
  auto entity = createInputMap();

  setInputState(key, 0.5f);
  inputMapSystem.update(view);

  auto inputMap = db.get<quoll::InputMap>(entity);
  auto &scheme = inputMap.schemes.at(0);
  auto command = scheme.inputKeyToCommandMap.at(key);

  EXPECT_EQ(std::get<glm::vec2>(inputMap.commandValues.at(command)),
            glm::vec2(0.5f, 0.0f));
}

TEST_F(InputMapSystemAxis2dValueTest, SetsYValueToIncomingFloatInput) {
  auto key = quoll::input::get("GAMEPAD_LEFT_Y");
  auto entity = createInputMap();

  setInputState(key, -0.8f);
  inputMapSystem.update(view);

  auto inputMap = db.get<quoll::InputMap>(entity);
  auto &scheme = inputMap.schemes.at(0);
  auto command = scheme.inputKeyToCommandMap.at(key);

  EXPECT_EQ(std::get<glm::vec2>(inputMap.commandValues.at(command)),
            glm::vec2(0.0f, -0.8f));
}

TEST_F(InputMapSystemAxis2dValueTest,
       SetsXValueToNegativeOneIfIncomingBooleanIsX0) {
  auto key = quoll::input::get("KEY_A");
  auto entity = createInputMap();

  setInputState(key, true);
  inputMapSystem.update(view);

  auto inputMap = db.get<quoll::InputMap>(entity);
  auto &scheme = inputMap.schemes.at(0);
  auto command = scheme.inputKeyToCommandMap.at(key);

  EXPECT_EQ(std::get<glm::vec2>(inputMap.commandValues.at(command)),
            glm::vec2(-1.0f, 0.0f));
}

TEST_F(InputMapSystemAxis2dValueTest, SetsXValueToOneIfIncomingBooleanIsX1) {
  auto key = quoll::input::get("KEY_D");
  auto entity = createInputMap();

  setInputState(key, true);
  inputMapSystem.update(view);

  auto inputMap = db.get<quoll::InputMap>(entity);
  auto &scheme = inputMap.schemes.at(0);
  auto command = scheme.inputKeyToCommandMap.at(key);

  EXPECT_EQ(std::get<glm::vec2>(inputMap.commandValues.at(command)),
            glm::vec2(1.0f, 0.0f));
}

TEST_F(InputMapSystemAxis2dValueTest,
       SetsYValueToNegativeOneIfIncomingBooleanIsY0) {
  auto key = quoll::input::get("KEY_W");
  auto entity = createInputMap();

  setInputState(key, true);
  inputMapSystem.update(view);

  auto inputMap = db.get<quoll::InputMap>(entity);
  auto &scheme = inputMap.schemes.at(0);
  auto command = scheme.inputKeyToCommandMap.at(key);

  EXPECT_EQ(std::get<glm::vec2>(inputMap.commandValues.at(command)),
            glm::vec2(0.0f, -1.0f));
}

TEST_F(InputMapSystemAxis2dValueTest, SetsYValueToOneIfIncomingBooleanIsY1) {
  auto key = quoll::input::get("KEY_S");
  auto entity = createInputMap();

  setInputState(key, true);
  inputMapSystem.update(view);

  auto inputMap = db.get<quoll::InputMap>(entity);
  auto &scheme = inputMap.schemes.at(0);
  auto command = scheme.inputKeyToCommandMap.at(key);

  EXPECT_EQ(std::get<glm::vec2>(inputMap.commandValues.at(command)),
            glm::vec2(0.0f, 1.0f));
}

TEST_F(InputMapSystemAxis2dValueTest, ClampsPositiveValueToOne) {
  auto keyX = quoll::input::get("GAMEPAD_LEFT_X");
  auto keyY = quoll::input::get("GAMEPAD_LEFT_Y");

  auto entity = createInputMap();

  setInputState(keyX, 2.5f);
  setInputState(keyY, 2.5f);

  inputMapSystem.update(view);

  auto inputMap = db.get<quoll::InputMap>(entity);
  auto &scheme = inputMap.schemes.at(0);
  auto command = scheme.inputKeyToCommandMap.at(keyX);

  EXPECT_EQ(std::get<glm::vec2>(inputMap.commandValues.at(command)),
            glm::vec2(1.0f, 1.0f));
}

TEST_F(InputMapSystemAxis2dValueTest, ClampsNegativeValueToNegativeOne) {
  auto keyX = quoll::input::get("GAMEPAD_LEFT_X");
  auto keyY = quoll::input::get("GAMEPAD_LEFT_Y");

  auto entity = createInputMap();

  setInputState(keyX, -2.5f);
  setInputState(keyY, -2.5f);

  inputMapSystem.update(view);

  auto inputMap = db.get<quoll::InputMap>(entity);
  auto &scheme = inputMap.schemes.at(0);
  auto command = scheme.inputKeyToCommandMap.at(keyX);

  EXPECT_EQ(std::get<glm::vec2>(inputMap.commandValues.at(command)),
            glm::vec2(-1.0f, -1.0f));
}

TEST_F(InputMapSystemAxis2dValueTest, AddsMultipleInputValuesTogether) {
  auto keyX = quoll::input::get("GAMEPAD_LEFT_X");
  auto keyY = quoll::input::get("GAMEPAD_LEFT_Y");

  auto entity = createInputMap();

  setInputState(keyX, -0.2f);
  setInputState(keyY, 0.4f);

  inputMapSystem.update(view);

  auto inputMap = db.get<quoll::InputMap>(entity);
  auto &scheme = inputMap.schemes.at(0);
  auto command = scheme.inputKeyToCommandMap.at(keyX);

  EXPECT_EQ(std::get<glm::vec2>(inputMap.commandValues.at(command)),
            glm::vec2(-0.2f, 0.4f));
}
