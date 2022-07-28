#include "liquid/core/Base.h"
#include "liquid/asset/AssetManager.h"
#include "liquid/scripting/ScriptingSystem.h"

#include "liquid-tests/Testing.h"

class RigidBodyLuaScriptingInterfaceTest : public ::testing::Test {
public:
  RigidBodyLuaScriptingInterfaceTest()
      : assetManager(std::filesystem::current_path()),
        scriptingSystem(eventSystem, assetManager.getRegistry()) {}

  liquid::LuaScope &call(liquid::Entity entity,
                         const liquid::String &functionName) {
    auto handle =
        assetManager
            .loadLuaScriptFromFile(std::filesystem::current_path() /
                                   "scripting-system-component-tester.lua")
            .getData();

    entityDatabase.setComponent<liquid::ScriptingComponent>(entity, {handle});

    scriptingSystem.start(entityDatabase);

    auto &scripting =
        entityDatabase.getComponent<liquid::ScriptingComponent>(entity);

    scripting.scope.luaGetGlobal(functionName);
    scripting.scope.call(0);

    return scripting.scope;
  }

  liquid::EntityDatabase entityDatabase;
  liquid::EventSystem eventSystem;
  liquid::AssetManager assetManager;
  liquid::ScriptingSystem scriptingSystem;
};

TEST_F(RigidBodyLuaScriptingInterfaceTest,
       ApplyForceDoesNothingIfProvidedArgumentsAreInvalid) {
  auto entity = entityDatabase.createEntity();

  EXPECT_FALSE(entityDatabase.hasComponent<liquid::ForceComponent>(entity));
  call(entity, "rigid_body_apply_force_invalid");
  EXPECT_FALSE(entityDatabase.hasComponent<liquid::ForceComponent>(entity));
}

TEST_F(RigidBodyLuaScriptingInterfaceTest, ApplyForceSetsForceComponent) {
  auto entity = entityDatabase.createEntity();
  EXPECT_FALSE(entityDatabase.hasComponent<liquid::ForceComponent>(entity));

  call(entity, "rigid_body_apply_force");
  EXPECT_TRUE(entityDatabase.hasComponent<liquid::ForceComponent>(entity));

  const auto &force =
      entityDatabase.getComponent<liquid::ForceComponent>(entity).force;

  EXPECT_EQ(force, glm::vec3(10.0f, 0.2f, 5.0f));
}

TEST_F(RigidBodyLuaScriptingInterfaceTest,
       ApplyTorqueDoesNothingIfProvidedArgumentsAreInvalid) {
  auto entity = entityDatabase.createEntity();
  EXPECT_FALSE(entityDatabase.hasComponent<liquid::TorqueComponent>(entity));

  call(entity, "rigid_body_apply_torque_invalid");
  EXPECT_FALSE(entityDatabase.hasComponent<liquid::TorqueComponent>(entity));
}

TEST_F(RigidBodyLuaScriptingInterfaceTest, ApplyTorqueSetsTorqueComponent) {
  auto entity = entityDatabase.createEntity();
  EXPECT_FALSE(entityDatabase.hasComponent<liquid::TorqueComponent>(entity));

  call(entity, "rigid_body_apply_torque");
  EXPECT_TRUE(entityDatabase.hasComponent<liquid::TorqueComponent>(entity));

  const auto &torque =
      entityDatabase.getComponent<liquid::TorqueComponent>(entity).torque;

  EXPECT_EQ(torque, glm::vec3(2.5f, 3.5f, 1.2f));
}
