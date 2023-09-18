#include "quoll/core/Base.h"
#include "quoll-tests/Testing.h"

#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class InputMapLuaScriptingInterfaceTest : public LuaScriptingInterfaceTestBase {
public:
  quoll::Entity createEntityWithInputMap(bool value = false) {
    auto entity = entityDatabase.create();

    quoll::InputMap map{};
    map.commandDataTypes.push_back(quoll::InputDataType::Boolean);
    map.commandDataTypes.push_back(quoll::InputDataType::Boolean);
    map.commandValues.resize(2, value);
    map.commandNameMap["Test"] = 1;

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

// get_command
TEST_F(InputMapLuaScriptingInterfaceTest, GetCommandReturnsCommandIndex) {
  auto entity = createEntityWithInputMap();
  auto &scope = call(entity, "input_get_command");

  EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command"));
  EXPECT_EQ(scope.getGlobal<uint32_t>("input_command"), 1);
}

TEST_F(InputMapLuaScriptingInterfaceTest,
       GetCommandReturnsNilIfEntityHasNoInputMap) {
  auto entity = entityDatabase.create();
  auto &scope = call(entity, "input_get_command");
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("input_command"));
}

TEST_F(InputMapLuaScriptingInterfaceTest,
       GetCommandReturnsNilIfCommandDoesNotExist) {
  auto entity = entityDatabase.create();

  quoll::InputMap map{};
  map.commandDataTypes.push_back(quoll::InputDataType::Boolean);
  map.commandValues.resize(1);
  map.commandNameMap["SomethingElse"] = 0;
  entityDatabase.set(entity, map);

  auto &scope = call(entity, "input_get_command");
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("input_command"));
}

TEST_F(InputMapLuaScriptingInterfaceTest,
       GetCommandReturnsNilIfProvidedArgumentIsInvalid) {
  auto entity = createEntityWithInputMap();

  {
    auto &scope = call(entity, "input_get_command_no_member");
    EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("input_command"));
  }

  {
    auto &scope = call(entity, "input_get_command_no_param");
    EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("input_command"));
  }

  {
    auto &scope = call(entity, "input_get_command_nil");
    EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("input_command"));
  }

  {
    auto &scope = call(entity, "input_get_command_number");
    EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("input_command"));
  }

  {
    auto &scope = call(entity, "input_get_command_boolean");
    EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("input_command"));
  }

  {
    auto &scope = call(entity, "input_get_command_function");
    EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("input_command"));
  }

  {
    auto &scope = call(entity, "input_get_command_table");
    EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("input_command"));
  }
}

// get_value_boolean
TEST_F(InputMapLuaScriptingInterfaceTest,
       GetCommandValueBooleanReturnsBooleanValue) {
  auto entity = createEntityWithInputMap(true);
  auto &scope = call(entity, "input_get_value_boolean");

  EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value"));
  EXPECT_EQ(scope.getGlobal<bool>("input_command_value"), true);
}

TEST_F(InputMapLuaScriptingInterfaceTest,
       GetCommandValueBooleanReturnsFalseIfCommandValueIsZeroAxis2d) {
  auto entity = createEntityWithInputMap(glm::vec2{0.0f, 0.0f});
  auto &scope = call(entity, "input_get_value_boolean");

  EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value"));
  EXPECT_EQ(scope.getGlobal<bool>("input_command_value"), false);
}

TEST_F(InputMapLuaScriptingInterfaceTest,
       GetCommandValueBooleanReturnsTrueIfCommandValueIsNotZeroAxis2d) {
  auto entity = createEntityWithInputMap(glm::vec2{0.1f, 0.0f});
  auto &scope = call(entity, "input_get_value_boolean");

  EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value"));
  EXPECT_EQ(scope.getGlobal<bool>("input_command_value"), true);
}

TEST_F(InputMapLuaScriptingInterfaceTest,
       GetCommandValueBooleanReturnsFalseIfInputMapNotFound) {
  auto entity = entityDatabase.create();
  auto &scope = call(entity, "input_get_value_boolean_non_existent_command");

  EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value"));
  EXPECT_EQ(scope.getGlobal<bool>("input_command_value"), false);
}

TEST_F(InputMapLuaScriptingInterfaceTest,
       GetCommandValueBooleanReturnsFalseIfCommandNotFound) {
  auto entity = createEntityWithInputMap();
  auto &scope = call(entity, "input_get_value_boolean_non_existent_command");

  EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value"));
  EXPECT_EQ(scope.getGlobal<bool>("input_command_value"), false);
}

TEST_F(InputMapLuaScriptingInterfaceTest,
       GetCommandValueBooleanReturnsFalseIfProvidedArgumentIsInvalid) {
  auto entity = createEntityWithInputMap();

  {
    auto &scope = call(entity, "input_get_value_boolean_no_member");
    EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value"));
    EXPECT_EQ(scope.getGlobal<bool>("input_command_value"), false);
  }

  {
    auto &scope = call(entity, "input_get_value_boolean_no_param");
    EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value"));
    EXPECT_EQ(scope.getGlobal<bool>("input_command_value"), false);
  }

  {
    auto &scope = call(entity, "input_get_value_boolean_nil");
    EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value"));
    EXPECT_EQ(scope.getGlobal<bool>("input_command_value"), false);
  }

  {
    auto &scope = call(entity, "input_get_value_boolean_string");
    EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value"));
    EXPECT_EQ(scope.getGlobal<bool>("input_command_value"), false);
  }

  {
    auto &scope = call(entity, "input_get_value_boolean_function");
    EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value"));
    EXPECT_EQ(scope.getGlobal<bool>("input_command_value"), false);
  }

  {
    auto &scope = call(entity, "input_get_value_boolean_table");
    EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value"));
    EXPECT_EQ(scope.getGlobal<bool>("input_command_value"), false);
  }
}

// get_value_axis_2d
TEST_F(InputMapLuaScriptingInterfaceTest,
       GetCommandValueAxis2dReturnsVec2Value) {
  auto entity = createEntityWithInputMap(glm::vec2{0.2f, -0.3f});
  auto &scope = call(entity, "input_get_value_axis_2d");

  EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value_x"));
  EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value_y"));

  EXPECT_EQ(scope.getGlobal<float>("input_command_value_x"), 0.2f);
  EXPECT_EQ(scope.getGlobal<float>("input_command_value_y"), -0.3f);
}

TEST_F(InputMapLuaScriptingInterfaceTest,
       GetCommandValueAxis2dReturnsZeroVec2IfCommandValueIsFalse) {
  auto entity = createEntityWithInputMap(false);
  auto &scope = call(entity, "input_get_value_axis_2d");

  EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value_x"));
  EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value_y"));

  EXPECT_EQ(scope.getGlobal<float>("input_command_value_x"), 0.0f);
  EXPECT_EQ(scope.getGlobal<float>("input_command_value_y"), 0.0f);
}

TEST_F(InputMapLuaScriptingInterfaceTest,
       GetCommandValueAxis2dReturnsOneVec2IfCommandValueIsTrue) {
  auto entity = createEntityWithInputMap(true);
  auto &scope = call(entity, "input_get_value_axis_2d");

  EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value_x"));
  EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value_y"));

  EXPECT_EQ(scope.getGlobal<float>("input_command_value_x"), 1.0f);
  EXPECT_EQ(scope.getGlobal<float>("input_command_value_y"), 1.0f);
}

TEST_F(InputMapLuaScriptingInterfaceTest,
       GetCommandValueAxis2dReturnsZeroVec2IfInputMapNotFound) {
  auto entity = entityDatabase.create();
  auto &scope = call(entity, "input_get_value_axis_2d_non_existent_command");

  EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value_x"));
  EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value_y"));

  EXPECT_EQ(scope.getGlobal<float>("input_command_value_x"), 0.0f);
  EXPECT_EQ(scope.getGlobal<float>("input_command_value_y"), 0.0f);
}

TEST_F(InputMapLuaScriptingInterfaceTest,
       GetCommandValueAxis2dReturnsZeroVec2IfCommandNotFound) {
  auto entity = createEntityWithInputMap();
  auto &scope = call(entity, "input_get_value_axis_2d_non_existent_command");

  EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value_x"));
  EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value_y"));

  EXPECT_EQ(scope.getGlobal<float>("input_command_value_x"), 0.0f);
  EXPECT_EQ(scope.getGlobal<float>("input_command_value_y"), 0.0f);
}

TEST_F(InputMapLuaScriptingInterfaceTest,
       GetCommandValueAxis2dReturnsZeroVec2IfProvidedArgumentIsInvalid) {
  auto entity = createEntityWithInputMap();

  {
    auto &scope = call(entity, "input_get_value_axis_2d_no_member");
    EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value_x"));
    EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value_y"));

    EXPECT_EQ(scope.getGlobal<float>("input_command_value_x"), 0.0f);
    EXPECT_EQ(scope.getGlobal<float>("input_command_value_y"), 0.0f);
  }

  {
    auto &scope = call(entity, "input_get_value_axis_2d_no_param");
    EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value_x"));
    EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value_y"));

    EXPECT_EQ(scope.getGlobal<float>("input_command_value_x"), 0.0f);
    EXPECT_EQ(scope.getGlobal<float>("input_command_value_y"), 0.0f);
  }

  {
    auto &scope = call(entity, "input_get_value_axis_2d_nil");
    EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value_x"));
    EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value_y"));

    EXPECT_EQ(scope.getGlobal<float>("input_command_value_x"), 0.0f);
    EXPECT_EQ(scope.getGlobal<float>("input_command_value_y"), 0.0f);
  }

  {
    auto &scope = call(entity, "input_get_value_axis_2d_string");
    EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value_x"));
    EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value_y"));

    EXPECT_EQ(scope.getGlobal<float>("input_command_value_x"), 0.0f);
    EXPECT_EQ(scope.getGlobal<float>("input_command_value_y"), 0.0f);
  }

  {
    auto &scope = call(entity, "input_get_value_axis_2d_function");
    EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value_x"));
    EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value_y"));

    EXPECT_EQ(scope.getGlobal<float>("input_command_value_x"), 0.0f);
    EXPECT_EQ(scope.getGlobal<float>("input_command_value_y"), 0.0f);
  }

  {
    auto &scope = call(entity, "input_get_value_axis_2d_table");
    EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value_x"));
    EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("input_command_value_y"));

    EXPECT_EQ(scope.getGlobal<float>("input_command_value_x"), 0.0f);
    EXPECT_EQ(scope.getGlobal<float>("input_command_value_y"), 0.0f);
  }
}
