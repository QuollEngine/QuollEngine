#include "quoll/core/Base.h"
#include "quoll/input/InputMap.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class InputMapLuaTableTest : public LuaScriptingInterfaceTestBase {
public:
  quoll::Entity createEntityWithInputMap(bool value = false,
                                         usize defaultScheme = 0) {
    auto entity = entityDatabase.create();

    quoll::InputMap map{};
    map.commandDataTypes.push_back(quoll::InputDataType::Boolean);
    map.commandDataTypes.push_back(quoll::InputDataType::Boolean);
    map.commandValues.resize(2, value);
    map.commandNameMap["Test"] = 1;

    map.schemes.resize(2);
    map.schemeNameMap.insert_or_assign("Default scheme", 0);
    map.schemeNameMap.insert_or_assign("Test scheme", 1);
    map.activeScheme = defaultScheme;

    entityDatabase.set(entity, map);
    return entity;
  }

  quoll::Entity createEntityWithInputMap(glm::vec2 value) {
    auto entity = entityDatabase.create();

    quoll::InputMap map{};
    map.commandDataTypes.push_back(quoll::InputDataType::Axis2d);
    map.commandDataTypes.push_back(quoll::InputDataType::Axis2d);
    map.commandValues.resize(2, value);
    map.commandNameMap["Test"] = 1;

    entityDatabase.set(entity, map);
    return entity;
  }
};

// getCommand
TEST_F(InputMapLuaTableTest, GetCommandReturnsCommandIndex) {
  auto entity = createEntityWithInputMap();
  auto state = call(entity, "inputGetCommand");

  EXPECT_FALSE(state["inputCommand"].is<sol::nil_t>());
  EXPECT_EQ(state["inputCommand"].get<u32>(), 1);
}

TEST_F(InputMapLuaTableTest, GetCommandReturnsNilIfEntityHasNoInputMap) {
  auto entity = entityDatabase.create();
  auto state = call(entity, "inputGetCommand");
  EXPECT_TRUE(state["inputCommand"].is<sol::nil_t>());
}

TEST_F(InputMapLuaTableTest, GetCommandReturnsNilIfCommandDoesNotExist) {
  auto entity = entityDatabase.create();

  quoll::InputMap map{};
  map.commandDataTypes.push_back(quoll::InputDataType::Boolean);
  map.commandValues.resize(1);
  map.commandNameMap["SomethingElse"] = 0;
  entityDatabase.set(entity, map);

  auto state = call(entity, "inputGetCommand");
  EXPECT_TRUE(state["inputCommand"].is<sol::nil_t>());
}

// getValueBoolean
TEST_F(InputMapLuaTableTest, GetCommandValueBooleanReturnsBooleanValue) {
  auto entity = createEntityWithInputMap(true);
  auto state = call(entity, "inputGetValueBoolean");

  EXPECT_FALSE(state["inputCommandValue"].is<sol::nil_t>());
  EXPECT_EQ(state["inputCommandValue"].get<bool>(), true);
}

TEST_F(InputMapLuaTableTest,
       GetCommandValueBooleanReturnsFalseIfCommandValueIsZeroAxis2d) {
  auto entity = createEntityWithInputMap(glm::vec2{0.0f, 0.0f});
  auto state = call(entity, "inputGetValueBoolean");

  EXPECT_FALSE(state["inputCommandValue"].is<sol::nil_t>());
  EXPECT_EQ(state["inputCommandValue"].get<bool>(), false);
}

TEST_F(InputMapLuaTableTest,
       GetCommandValueBooleanReturnsTrueIfCommandValueIsNotZeroAxis2d) {
  auto entity = createEntityWithInputMap(glm::vec2{0.1f, 0.0f});
  auto state = call(entity, "inputGetValueBoolean");

  EXPECT_FALSE(state["inputCommandValue"].is<sol::nil_t>());
  EXPECT_EQ(state["inputCommandValue"].get<bool>(), true);
}

TEST_F(InputMapLuaTableTest,
       GetCommandValueBooleanReturnsNilIfInputMapNotFound) {
  auto entity = entityDatabase.create();
  auto state = call(entity, "inputGetValueBooleanNonExistentCommand");

  EXPECT_TRUE(state["inputCommandValue"].is<sol::nil_t>());
}

TEST_F(InputMapLuaTableTest,
       GetCommandValueBooleanReturnsFalseIfCommandNotFound) {
  auto entity = createEntityWithInputMap();
  auto state = call(entity, "inputGetValueBooleanNonExistentCommand");

  EXPECT_TRUE(state["inputCommandValue"].is<sol::nil_t>());
}

// getValueAxis2d
TEST_F(InputMapLuaTableTest, GetCommandValueAxis2dReturnsVec2Value) {
  auto entity = createEntityWithInputMap(glm::vec2{0.2f, -0.3f});
  auto state = call(entity, "inputGetValueAxis2d");

  EXPECT_FALSE(state["inputCommandValueX"].is<sol::nil_t>());
  EXPECT_FALSE(state["inputCommandValueY"].is<sol::nil_t>());

  EXPECT_EQ(state["inputCommandValueX"].get<f32>(), 0.2f);
  EXPECT_EQ(state["inputCommandValueY"].get<f32>(), -0.3f);
}

TEST_F(InputMapLuaTableTest,
       GetCommandValueAxis2dReturnsZeroVec2IfCommandValueIsFalse) {
  auto entity = createEntityWithInputMap(false);
  auto state = call(entity, "inputGetValueAxis2d");

  EXPECT_FALSE(state["inputCommandValueX"].is<sol::nil_t>());
  EXPECT_FALSE(state["inputCommandValueY"].is<sol::nil_t>());

  EXPECT_EQ(state["inputCommandValueX"].get<f32>(), 0.0f);
  EXPECT_EQ(state["inputCommandValueY"].get<f32>(), 0.0f);
}

TEST_F(InputMapLuaTableTest,
       GetCommandValueAxis2dReturnsOneVec2IfCommandValueIsTrue) {
  auto entity = createEntityWithInputMap(true);
  auto state = call(entity, "inputGetValueAxis2d");

  EXPECT_FALSE(state["inputCommandValueX"].is<sol::nil_t>());
  EXPECT_FALSE(state["inputCommandValueY"].is<sol::nil_t>());

  EXPECT_EQ(state["inputCommandValueX"].get<f32>(), 1.0f);
  EXPECT_EQ(state["inputCommandValueY"].get<f32>(), 1.0f);
}

TEST_F(InputMapLuaTableTest,
       GetCommandValueAxis2dReturnsZeroVec2IfInputMapNotFound) {
  auto entity = entityDatabase.create();
  auto state = call(entity, "inputGetValueAxis2dNonExistentCommand");

  EXPECT_TRUE(state["inputCommandValueX"].is<sol::nil_t>());
  EXPECT_TRUE(state["inputCommandValueY"].is<sol::nil_t>());
}

TEST_F(InputMapLuaTableTest,
       GetCommandValueAxis2dReturnsZeroVec2IfCommandNotFound) {
  auto entity = createEntityWithInputMap();
  auto state = call(entity, "inputGetValueAxis2dNonExistentCommand");

  EXPECT_TRUE(state["inputCommandValueX"].is<sol::nil_t>());
  EXPECT_TRUE(state["inputCommandValueY"].is<sol::nil_t>());
}

// set_scheme
TEST_F(InputMapLuaTableTest, SetSchemeSetsSchemeToProvidedValue) {
  auto entity = createEntityWithInputMap();

  EXPECT_EQ(entityDatabase.get<quoll::InputMap>(entity).activeScheme, 0);
  call(entity, "inputSetScheme");
  EXPECT_EQ(entityDatabase.get<quoll::InputMap>(entity).activeScheme, 1);
}

TEST_F(InputMapLuaTableTest,
       SetSchemeDoesNothingIfInputMapComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "inputSetScheme");
  EXPECT_FALSE(entityDatabase.has<quoll::InputMap>(entity));
}

TEST_F(InputMapLuaTableTest, SetSchemeDoesNothingIfSchemeNotFound) {
  auto entity = createEntityWithInputMap(false, 0);
  entityDatabase.get<quoll::InputMap>(entity).schemeNameMap.erase(
      "Test scheme");

  call(entity, "inputSetScheme");
  EXPECT_EQ(entityDatabase.get<quoll::InputMap>(entity).activeScheme, 0);
}
